TERM_COLOR_BLACK   = "\027[30m"
TERM_COLOR_RED     = "\027[31m"
TERM_COLOR_GREEN   = "\027[32m"
TERM_COLOR_YELLOW  = "\027[33m"
TERM_COLOR_BLUE    = "\027[34m"
TERM_COLOR_MAGENTA = "\027[35m"
TERM_COLOR_CYAN    = "\027[36m"
TERM_COLOR_WHITE   = "\027[37m"
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


function operand_abs (operand, address, size)
    local absolute
--[[
    for k,v in pairs(operand) do
        print(k .. " " .. tostring(v))
    end
]]
    if (operand["base"] ~= "none" and operand["base"] ~= "rip") or 
        operand["index"] ~= "none" then
        return false
    elseif operand["type"] == "jimm" or 
           (operand["type"] == "mem" and operand["base"] == "rip") then
        absolute = address + uint_t.new(32, size)
        if operand["lval"]:int() < 0 then
            absolute = absolute - (operand["lval"] * int_t.new(8, -1)):uint_t()
        else
            absolute = absolute + operand["lval"]:uint_t()
        end
    elseif operand["type"] == "mem" then
        absolute = operand["lval"]:uint_t()
    else
        return false
    end
    return absolute
end


function relative_offset_description (elf, instruction)
    local target_address
    local description = nil
    
    target_address = operand_abs(instruction["operands"][1],
                                 instruction["address"],
                                 instruction["size"])
    if not target_address then
        return string.sub(instruction["description"], #instruction["mnemonic"] + 1)
    end
    
    -- check symbols
    for i, symbol in pairs(elf:symbols()) do
        if symbol:type() == "func" and
           symbol:value() <= target_address and
           symbol:value() + symbol:size():uint_t() >= target_address then
            description = symbol:name() .. "+" .. 
                          tostring(target_address - symbol:value()) ..
                          " (" .. target_address:strx() .. ")"
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


function print_instructions (elf, instructions)
    
    -- first pass, find addresses we jump to in this function
    local jump_destinations = {}
    local jump_origins = {}
    for index, instruction in pairs(instructions) do
        abs = operand_abs(instruction["operands"][1],
                          instruction["address"], instruction["size"])
        if is_jump(instruction["mnemonic"]) and 
                   abs and
                   abs >= instructions[1]["address"] and
                   abs <= instructions[#instructions]["address"]
                          + uint_t.new(32, instructions[#instructions]["size"]) then
            table.insert(jump_origins, instruction["address"])
            table.insert(jump_destinations, (instruction["address"]:int_t() + 
                                             instruction["operands"][1]["lval"] +
                                             int_t.new(8, instruction["size"])):uint_t())
        end
    end

    -- second pass, plan the routes
    local jump_routes = {}
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
        
    -- last pass, print it all out
    for index,instruction in pairs(instructions) do
        -- is this address one of our jump locations
        local address = TERM_COLOR_GREEN .. instruction["address"]:strx() .. 
                        TERM_COLOR_DEFAULT
        for i,jump_destination in pairs(jump_destinations) do
            if jump_destination == instruction["address"] then
                address = TERM_COLOR_CYAN .. TERM_BOLD .. 
                          instruction["address"]:strx() ..
                          TERM_NORMAL .. TERM_COLOR_DEFAULT
                break
            end
        end
        
        -- trace the jump paths
        local jump_path = ''
        local jump_origin = false
        local jump_destination = false
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
                elseif jump_origin or jump_destination then
                    jump_path = jump_path .. '+'
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
                    
        -- oh hell, let's get the bytes for this instruction and print that out
        local mem = elf:section(".text"):mem_at_address(instruction["address"],
                                                        instruction["size"],
                                                        8);
        local bytes = ""
        local bytes_written = 0
        for address, byte in pairs(mem) do
            bytes = bytes .. byte:str0x()
            bytes_written = bytes_written + 1
            if bytes_written >= 8 then break end
        end
        for i=1,8-bytes_written do
            bytes = bytes .. ".."
        end
        bytes = TERM_COLOR_WHITE .. bytes .. TERM_COLOR_DEFAULT
        
        -- pretty instruction description
        local description
        if instruction["mnemonic"] == "call" then
            description = TERM_COLOR_RED .. TERM_BOLD .. instruction["mnemonic"] ..
                          " " .. relative_offset_description(elf, instruction) .. 
                          TERM_NORMAL .. TERM_COLOR_DEFAULT
        elseif is_jump(instruction["mnemonic"]) then
            description = TERM_COLOR_CYAN .. TERM_BOLD .. instruction["mnemonic"] ..
                          " " .. relative_offset_description(elf, instruction) ..
                          TERM_NORMAL .. TERM_COLOR_DEFAULT
        elseif instruction["mnemonic"] == "ret" then
            description = TERM_COLOR_YELLOW .. TERM_BOLD .. 
                          instruction["description"] ..
                          TERM_NORMAL .. TERM_COLOR_DEFAULT
        else
            description = instruction["description"]
        end
        print(address .. " " .. bytes .. " " .. jump_path .. " " .. description)
    end
end


print(argv[1])
elf = elf_t.new(argv[1])
text = elf:section(".text")
if argv[2] ~= nil then
    symbol = elf:section(".symtab"):symbol(argv[2])
    print(TERM_COLOR_MAGENTA .. TERM_BOLD .. symbol:value():str0x() ..
          " " .. symbol:name() .. ':' .. TERM_NORMAL .. TERM_COLOR_DEFAULT)
    print_instructions(elf, symbol:disassemble())
else
    symbols = elf:symbols()
    table.sort(symbols, function(a,b) return a:value() < b:value() end)
    for i, symbol in pairs(symbols) do
        if symbol:type() == "func" and
            symbol:value():int() > 0 then
            print(TERM_COLOR_MAGENTA .. TERM_BOLD .. symbol:value():str0x() ..
                  " " .. symbol:name() .. ':' .. TERM_NORMAL .. TERM_COLOR_DEFAULT)
            print_instructions(elf, symbol:disassemble())
            print()
            --symbol:disassemble()
        end
    end
end

