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

-- assumes tables has been sorted with cmp function
function table.find(t, value, cmp)
    if #t == 0 then
        return false
    end
    
    local min = 1
    local max = #t
    local mid
    while min < max do
        mid = (min + max) / 2
        if cmp(t[mid], value) < 0 then
            max = mid - 1
        elseif cmp(t[mid], value) > 0 then
            min = min + 1
        else
            return mid
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


function functions_from_symbols (symbols)
    -- make sure we have only function symbols with addresses
    i = 1
    while i <= #symbols do
        if symbols[i]:type() ~= "function" or 
           symbols[i]:address():int() == 0 then
            table.remove(symbols, i)
        else
            i = i + 1
        end
    end
    
    -- sort them by address, ascending order
    table.sort(symbols, function (a,b) return a:address() < b:address() end)
    
    function_symbols = {}
    for i,symbol in pairs(symbols) do
        -- if this symbol has a size, use that
        if symbol:size() > 0 then
            function_symbols[symbol:name()] = {begin=symbol:address(),
                                               -- endaddr because end is reserved
                                               -- keyword
                                               endaddr=symbol:address()
                                                   + uint_t.new(32, symbol:size())}
        -- otherwise, use the beginning of the next function symbol
        -- if there is no other function symbol... 100 (ugly hack)
        else
            if i == #symbols then
                function_symbols[symbol:name()] = {begin=symbol:address(),
                                                   endaddr=symbol:address()
                                                       + uint_t.new(32, 100)}
            else
                function_symbols[symbol:name()] = {begin=symbol:address(),
                                                   endaddr=symbols[i+1]:address()
                                                           - uint_t.new(32, 1)}
            end
        end
    end
    
    return function_symbols
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
        print(operand["type"])
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
    -- functions_from_symbols should already be in ascending order sorted by
    -- ["begin"]
    -- make target_address look like a result from functions_from_symbols first
    for name, func in pairs(functions_from_symbols(exec:symbols())) do
        if func["begin"] <= target_address and 
           func["endaddr"] >= target_address then
            description = name .. "+0x" .. (target_address - func["begin"]):strx() ..
                          " (" .. target_address:strx() .. ")"
            break
        end
    end
    --[[
    for i, symbol in pairs(elf:symbols()) do
        if symbol:type() == "function" and
           symbol:address() <= target_address and
           symbol:address() + uint_t.new(32, symbol:size()) >= target_address then
            description = symbol:name() .. "+" .. 
                          tostring(target_address - symbol:address()) ..
                          " (" .. target_address:strx() .. ")"
            break
        end
    end 
    ]]    
    
    if description ~= nil then
        return description
    else
        return "(" .. target_address:strx() .. ")"
    end
end


function print_instructions (exec, instructions)
    
    -- first pass, find addresses we jump to in this function
    local jump_destinations = {}
    local jump_origins = {}
    for index, instruction in pairs(instructions) do
        if is_jump(instruction["mnemonic"]) and 
           operand_abs(instruction["operands"][1],
                       instruction["address"],
                       instruction["size"]) and -- if this is false, then this
                                                -- jmp doesn't evaluate to an
                                                -- address
           operand_abs(instruction["operands"][1],
                       instruction["address"],
                       instruction["size"]) >= instructions[1]["address"] and
           operand_abs(instruction["operands"][1],
                       instruction["address"],
                       instruction["size"]) <= instructions[#instructions]["address"] +
                        uint_t.new(32, instructions[#instructions]["size"]) then
            table.insert(jump_origins, instruction["address"])
            table.insert(jump_destinations, (instruction["address"]:int_t() + 
                                             instruction["operands"][1]["lval"] +
                                             int_t.new(8, instruction["size"])):uint_t())
        end
    end
    
    -- last pass, print it all out
    function_symbols = functions_from_symbols(exec:symbols())
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
        
        -- pretty instruction description
        local description
        
        if instruction["mnemonic"] == "call" then
            description = TERM_COLOR_RED .. TERM_BOLD .. instruction["mnemonic"] ..
                          " " .. relative_offset_description(exec, instruction) .. 
                          TERM_NORMAL .. TERM_COLOR_DEFAULT
        elseif is_jump(instruction["mnemonic"]) then
            description = TERM_COLOR_CYAN .. TERM_BOLD .. instruction["mnemonic"] ..
                          " " .. relative_offset_description(exec, instruction) ..
                          TERM_NORMAL .. TERM_COLOR_DEFAULT
        elseif instruction["mnemonic"] == "ret" then
            description = TERM_COLOR_YELLOW .. TERM_BOLD .. 
                          instruction["description"] ..
                          TERM_NORMAL .. TERM_COLOR_DEFAULT
        else
            description = instruction["description"]
        end
        
        -- function symbol here? print name
        for name, func in pairs(function_symbols) do
            if func["begin"] == instruction["address"] then
                print("  " .. TERM_COLOR_MAGENTA .. TERM_BOLD .. name ..
                      TERM_NORMAL .. TERM_COLOR_DEFAULT)
            end
        end
        print(address .. "  " .. description)
    end
end


print(argv[1])
exec = exec_t.new(argv[1])
for section_i, section in pairs(exec:sections()) do
    if table.contains(section:types(), "executable") then
        print(TERM_COLOR_BLUE .. TERM_BOLD .. section:address():str0x() ..
              " " .. section:name() .. ":" ..
              TERM_NORMAL .. TERM_COLOR_DEFAULT)
        print_instructions(exec, section:disassemble())
    end
end

