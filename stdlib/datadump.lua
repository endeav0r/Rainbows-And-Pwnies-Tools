function datadump_largest_string (strings)
    local largest = 0
    for si, s in pairs(strings) do
        if #tostring(s) > largest then
            largest = #tostring(s)
        end
    end
    return largest
end


-------------------------------------------------
-- dump_sections                               --
-------------------------------------------------
function dump_sections (exec)
    local sections = exec:sections()
    local sizes = {}
    local addresses = {}
    
    for si, s in pairs(sections) do 
        table.insert(sizes, s:size())
        table.insert(addresses, s:address():strx())
    end
    local largest_size    = datadump_largest_string(sizes)
    local largest_address = datadump_largest_string(addresses)

    for si, section in pairs(sections) do
        
        local size = tostring(section:size())
        while #size < largest_size do
            size = size .. ' '
        end
        local address = section:address():strx()
        while #address < largest_address do
            address = '0' .. address
        end
        print(TERM_COLOR_GREEN .. address .. TERM_COLOR_DEFAULT .. ' ' ..
              size .. ' ' ..
              TERM_COLOR_CYAN .. section:name() .. TERM_COLOR_DEFAULT)
    end
end


-------------------------------------------------
-- dump_functions                              --
-------------------------------------------------
function dump_functions (exec)
    local hook = hooks(exec)
    hook:init()
    
    for fi, f in pairs(hook.functions:get()) do
        print(TERM_COLOR_GREEN .. f['address']:strx() .. TERM_COLOR_DEFAULT .. ' ' ..
              f['size'] .. ' ' ..
              TERM_COLOR_CYAN .. f['name'] .. TERM_COLOR_DEFAULT)
    end
end


-------------------------------------------------
-- dump_function                               --
-------------------------------------------------
function dump_function (exec, function_name)
    local hook = hooks(exec)
    hook:init()

    local func = hook.functions:g_name(function_name)
    if func == nil then
        print('could not find function')
    else
        instructions = exec:disassemble(func['address'], func['size'])
        dump_instructions(exec, instructions)
    end
end


-------------------------------------------------
-- dump_instructions                           --
-------------------------------------------------
function dump_instructions (exec, instructions)
    table.sort(instructions, function (a, b) return a['address'] < b['address'] end)

    for ii, ins in pairs(instructions) do
        -- symbol description for this address
        local description = describe_address_symbol(exec, ins['address'])
        if description ~= nil then
            print(TERM_BOLD .. TERM_COLOR_RED .. description .. TERM_COLOR_DEFAULT .. TERM_NORMAL)
        end

        -- hook label for this address
        local labels = hooks(exec).labels:g_address(ins['address'])
        if labels ~= nil then
            for li, l in pairs(labels) do
                print(TERM_COLOR_GREEN .. ins['address']:strx() .. TERM_COLOR_DEFAULT .. ' ' ..
                      TERM_COLOR_WHITE .. '// ' .. l .. TERM_COLOR_DEFAULT)
            end
        end

        -- calls and jumps are special cases
        local description = ins['description']
        local description_color = TERM_COLOR_CYAN
        if ins['mnemonic'] == 'call' or is_jump(ins['mnemonic']) then
            description_color = TERM_COLOR_YELLOW
            if operand_abs(ins, 1) ~= nil then
                local description_text = hooks(e).functions:g_address(operand_abs(ins, 1):uint_t())
                if description_text ~= nil then
                    description_text = description_text['name']
                else
                    description_text = describe_address(exec, operand_abs(ins, 1):uint_t())
                end
                description = ins['mnemonic'] .. ' ' .. description_text
            end
        end

        -- print the instruction
        print(TERM_COLOR_GREEN .. ins['address']:strx() .. TERM_COLOR_DEFAULT .. ' ' ..
              description_color  .. description    .. TERM_COLOR_DEFAULT)

    end

end
