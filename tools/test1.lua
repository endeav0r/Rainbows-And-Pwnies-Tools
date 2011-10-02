TERM_COLOR_RED     = "\027[31m"
TERM_COLOR_GREEN   = "\027[32m"
TERM_COLOR_YELLOW  = "\027[33m"
TERM_COLOR_BLUE    = "\027[34m"
TERM_COLOR_CYAN    = "\027[36m"
TERM_COLOR_DEFAULT = "\027[39m"
TERM_BOLD          = "\027[1m"
TERM_NORMAL        = "\027[22m"


function table.contains(t, v)
    for i, j in pairs(t) do
        if j == v then
            return true
        end
    end
    return false
end


function is_jump (mnemonic)
    if mnemonic == "jmp" or
       mnemonic == "jo" or
       mnemonic == "jno" or
       mnemonic == "jb" or
       mnemonic == "jae" or
       mnemonic == "jz" or
       mnemonic == "jnz" or
       mnemonic == "jbe" or
       mnemonic == "ja" or
       mnemonic == "js" or
       mnemonic == "jp" or
       mnemonic == "jnp" or
       mnemonic == "jl" or
       mnemonic == "jge" or
       mnemonic == "jle" or
       mnemonic == "jg" then
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


function operand_abs (operand, address, size)
    local absolute = address + uint_t.new(32, size)
    
    if operand["lval"]:int() < 0 then
        absolute = absolute - (operand["lval"] * int_t.new(8, -1)):uint_t()
    else
        absolute = absolute + operand["lval"]:uint_t()
    end
    
    return absolute
end


function relative_offset_description (elf, instruction)
    local target_address
    local description = nil
    
    target_address = operand_abs(instruction["operands"][1],
                                 instruction["address"],
                                 instruction["size"])
    
    -- check symbols
    local symtab = elf:section(".symtab")
    for i = 0,symtab:num()-1 do
        local symbol = symtab:symbol(i)
        if symbol:value() <= target_address and
           symbol:value() + symbol:size():uint_t() > target_address then
            description = symbol:name() .. 
                          " ( " .. target_address:strx() .. " | " .. 
                          tostring(target_address - symbol:value()) .. " )"
            break
        end
    end
    
    -- maybe it's in PLT?
    if description == nil then
        local plt = elf:section(".plt")
        if target_address >= plt:address() and
           target_address < plt:address() + plt:size():uint_t() then
            local plt_jmp = plt:disassemble(target_address)
            local op_address = operand_abs(plt_jmp["operands"][1],
                                           target_address,
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
                if relocation:offset() == op_address then
                    description = relocation:name() .. "@PLT"
                    break
                end
            end
        end
    end
            
    
    if description ~= nil then
        return description
    else
        return "(" .. target_address:strx() .. ")"
    end
end



elf = elf_t.new(argv[1])

instructions = disassemble_function(elf, argv[2])
-- first pass, find addresses we jump to in this function
jump_destinations = {}
jump_origins = {}
for index, instruction in pairs(instructions) do
    if is_jump(instruction["mnemonic"]) then
        table.insert(jump_origins, instruction["address"])
        table.insert(jump_destinations, (instruction["address"]:int_t() + 
                                         instruction["operands"][1]["lval"] +
                                         int_t.new(8, instruction["size"])):uint_t())
    end
end

-- now plan the routes
jump_routes = {}
for index, instruction in pairs(instructions) do
    jump_routes[index] = {}
    for route, destination in pairs(jump_destinations) do
        if (destination >= instruction["address"] and
            jump_origins[route] <= instruction["address"])
           or
           (destination <= instruction["address"] and
            jump_origins[route] >= instruction["address"]) then
            table.insert(jump_routes[index], route)
        end
    end
end
    

for index,instruction in pairs(instructions) do
    -- is this address one of our jump locations
    address = TERM_COLOR_GREEN .. instruction["address"]:strx() .. 
              TERM_COLOR_DEFAULT
    for i,jump_destination in pairs(jump_destinations) do
        if jump_destination == instruction["address"] then
            address = TERM_COLOR_BLUE .. TERM_BOLD .. 
                      instruction["address"]:strx() ..
                      TERM_NORMAL .. TERM_COLOR_DEFAULT
            break
        end
    end
    
    -- trace the jump paths
    jump_path = ''
    jump_origin = false
    jump_destination = false
    for route, destination in pairs(jump_destinations) do
        if table.contains(jump_routes[index], route) then
            if jump_origins[route] == instruction["address"] then
                jump_origin = true
                if jump_destinations[route] > instruction["address"] then
                    jump_path = jump_path .. "/"
                else
                    jump_path = jump_path .. "\\"
                end
            elseif jump_destinations[route] == instruction["address"] then
                jump_destination = true
                if jump_origins[route] < instruction["address"] then
                    jump_path = jump_path .. "\\"
                else
                    jump_path = jump_path .. '/'
                end
            else
                jump_path = jump_path .. '|'
            end
        elseif jump_origin or jump_destination then
            jump_path = jump_path .. '-'
        else
            jump_path = jump_path .. ' '
        end
    end
    if jump_origin and jump_destination then
        jump_path = jump_path .. '*'
    elseif jump_origin then
        jump_path = jump_path .. '<'
    elseif jump_destination then
        jump_path = jump_path .. '>'
    else
        jump_path = jump_path .. ' '
    end
    
    jump_path = TERM_COLOR_CYAN .. TERM_BOLD .. jump_path .. TERM_NORMAL ..
                TERM_COLOR_DEFAULT
    
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
    print(address .. " " .. jump_path .. " " .. instruction)
end
