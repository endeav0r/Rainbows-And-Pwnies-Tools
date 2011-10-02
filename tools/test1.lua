TERM_COLOR_RED     = "\027[31m"
TERM_COLOR_GREEN   = "\027[32m"
TERM_COLOR_YELLOW  = "\027[33m"
TERM_COLOR_BLUE    = "\027[34m"
TERM_COLOR_CYAN    = "\027[36m"
TERM_COLOR_DEFAULT = "\027[39m"
TERM_BOLD          = "\027[1m"
TERM_NORMAL        = "\027[22m"


function is_jump (mnemonic)
    if mnemonic == "jmp" or
       mnemonic == "jle" or
       mnemonic == "jnz" or
       mnemonic == "jz" then
        return true
    end
    return false
end


function disassemble_function (elf, function_name)
    local symtab = elf:section(".symtab")
    local symbol = symtab:symbol(function_name)
    local text = elf:section(".text")
    local disassembly = text:disassemble()
    local instructions = {}
    
    for i,instruction in pairs(disassembly) do
        if instruction["address"] >= symbol:value() and
           instruction["address"] <  symbol:value() + symbol:size():uint_t() then
            table.insert(instructions, instruction)
        end
    end
    return instructions
end


function operand_address (operand, address, size)
    local op_address = operand["lval"]
    
    if operand["base"] == "rip" then
        op_address = op_address + address:int_t() + int_t.new(32, size)
    end
    return op_address
end


function relative_offset_description (elf, instruction)
    local address = instruction["address"] + uint_t.new(32, instruction["size"])
    local lval = instruction["operands"][1]["lval"]
    local description = nil
    
    if lval:int() < 0 then
        lval = lval * int_t.new(8, -1)
        address = address - lval:uint_t()
    else
        address = address + lval:uint_t()
    end
    
    -- check symbols
    local symtab = elf:section(".symtab")
    for i = 0,symtab:num()-1 do
        local symbol = symtab:symbol(i)
        if symbol:value() <= address and
           symbol:value() + symbol:size():uint_t() > address then
            --address = address - symbol:value()
            description = symbol:name() .. 
                          " ( " .. address:strx() .. " | " .. 
                          tostring(address - symbol:value()) .. " )"
            break
        end
    end
    
    -- maybe it's in PLT?
    if description == nil then
        local plt = elf:section(".plt")
        if address >= plt:address() and
           address < plt:address() + plt:size():uint_t() then
            local plt_jmp = plt:disassemble(address)
            local op_address = operand_address(plt_jmp["operands"][1],
                                         address,
                                         plt_jmp["size"])
            -- find relocation for op_address
            local relplt
            if elf:section_exists(".rel.plt") then
                relplt = elf:section(".rel.plt")
            else
                relplt = elf:section(".rela.plt")
            end
            for i = 0,relplt:num()-1 do
                local relocation = relplt:relocation(i)
                if relocation:offset() == op_address:uint_t() then
                    description = relocation:name() .. "@PLT"
                    break
                end
            end
        end
    end
            
    
    if description ~= nil then
        return description
    else
        return "(" .. address:strx() .. ")"
    end
end



elf = elf_t.new(argv[1])

instructions = disassemble_function(elf, argv[2])
-- first pass, find addresses we jump to in this function
jump_locations = {}
for i, instruction in pairs(instructions) do
    if is_jump(instruction["mnemonic"]) then
        table.insert(jump_locations, (instruction["address"]:int_t() + 
                                      instruction["operands"][1]["lval"] +
                                      int_t.new(8, instruction["size"])):uint_t())
    end
end

for i,instruction in pairs(instructions) do
    -- is this address one of our jump locations
    address = TERM_COLOR_GREEN .. instruction["address"]:strx() .. 
              TERM_COLOR_DEFAULT
    for i,jump_location in pairs(jump_locations) do
        if jump_location == instruction["address"] then
            address = TERM_COLOR_CYAN .. TERM_BOLD .. 
                      instruction["address"]:strx() ..
                      TERM_NORMAL .. TERM_COLOR_DEFAULT
            break
        end
    end
    
    if instruction["mnemonic"] == "call" then
        instruction = TERM_COLOR_RED .. TERM_BOLD .. instruction["mnemonic"] ..
                      " " .. relative_offset_description(elf, instruction) .. 
                      TERM_NORMAL .. TERM_COLOR_DEFAULT
    elseif is_jump(instruction["mnemonic"]) then
        instruction = TERM_COLOR_CYAN .. TERM_BOLD .. instruction["mnemonic"] ..
                      " " .. relative_offset_description(elf, instruction) ..
                      TERM_NORMAL .. TERM_COLOR_DEFAULT
    elseif instruction["mnemonic"] == "ret" then
        instruction = TERM_COLOR_YELLOW .. TERM_BOLD .. 
                      instruction["description"] ..
                      TERM_NORMAL .. TERM_COLOR_DEFAULT
    else
        instruction = instruction["description"]
    end
    print(address .. "   " .. instruction)
end
