TERM_COLOR_GREEN   = "\027[32m"
TERM_COLOR_RED     = "\027[31m"
TERM_COLOR_BLUE    = "\027[34m"
TERM_COLOR_CYAN    = "\027[36m"
TERM_COLOR_DEFAULT = "\027[39m"
TERM_BOLD          = "\027[1m"
TERM_NORMAL        = "\027[22m"

function disassemble_function (elf, function_name)
    symtab = elf:section(".symtab")
    symbol = symtab:symbol(function_name)
    text = elf:section(".text")
    disassembly = text:disassemble()
    
    instructions = {}
    for i,instruction in pairs(disassembly) do
        if instruction["address"] >= symbol:value() and
           instruction["address"] <  symbol:value() + symbol:size():uint_t() then
            table.insert(instructions, instruction)
        end
    end
    return instructions
end


function operand_address (operand, address, size)
    op_address = operand["lval"]
    if operand["base"] == "rip" then
        op_address = op_address + address:int_t() + int_t.new(32, size)
    end
    return op_address
end


function relative_offset_description (elf, instruction)
    address = instruction["address"] + uint_t.new(32, instruction["size"])
    lval = instruction["operands"][1]["lval"]
    if lval:int() < 0 then
        lval = lval * int_t.new(8, -1)
        address = address - lval:uint_t()
    else
        address = address + lval:uint_t()
    end
    
    -- check symbols
    symtab = elf:section(".symtab")
    description = nil
    for i = 0,symtab:num()-1 do
        symbol = symtab:symbol(i)
        if symbol:value() <= address and
           symbol:value() + symbol:size():uint_t() > address then
            --address = address - symbol:value()
            description = symbol:name() .. 
                          " (" .. address:strx() .. " | " .. 
                          tostring(address - symbol:value()) .. ")"
            break
        end
    end
    
    -- maybe it's in PLT?
    if description == nil then
        plt = elf:section(".plt")
        if address >= plt:address() and
           address < plt:address() + plt:size():uint_t() then
            plt_jmp = plt:disassemble(address)
            op_address = operand_address(plt_jmp["operands"][1],
                                         address,
                                         plt_jmp["size"])
            -- find relocation for op_address
            relaplt = elf:section(".rela.plt")
            for i = 0,relaplt:num()-1 do
                relocation = relaplt:relocation(i)
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

elf = elf_t.new("test_programs/test1")

instructions = disassemble_function(elf, "dowork")
for i,instruction in pairs(instructions) do
    if instruction["mnemonic"] == "call" or
       instruction["mnemonic"] == "jmp" or 
       instruction["mnemonic"] == "jle" or 
       instruction["mnemonic"] == "jz" then
        print(TERM_COLOR_GREEN .. instruction["address"]:strx() .. "   " ..
              TERM_COLOR_RED .. TERM_BOLD .. instruction["mnemonic"] .. " " ..
              relative_offset_description(elf, instruction) ..
              TERM_NORMAL .. TERM_COLOR_DEFAULT)
    elseif instruction["mnemonic"] == "ret" then
        print(TERM_COLOR_GREEN .. instruction["address"]:strx() .. "   " .. 
              TERM_COLOR_RED .. TERM_BOLD .. instruction["description"] ..
              TERM_NORMAL .. TERM_COLOR_DEFAULT)
    else
        print(TERM_COLOR_GREEN .. instruction["address"]:strx() .. "   " .. 
              TERM_COLOR_DEFAULT .. instruction["description"])
    end
end
