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
    
    for fi, f in pairs(hook.functions) do
        print(TERM_COLOR_GREEN .. f['address']:strx() .. TERM_COLOR_DEFAULT .. ' ' ..
              f['size'] .. ' ' ..
              TERM_COLOR_CYAN .. f['name'] .. TERM_COLOR_DEFAULT)
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

        -- calls and jumps are special cases
        if ins['mnemonic'] == 'call' or is_jump(ins['mnemonic']) then
            print(ins['mnemonic'])
        end

        -- print the instruction
        print(TERM_COLOR_GREEN .. ins['address']:strx() .. TERM_COLOR_DEFAULT .. ' ' ..
              TERM_COLOR_CYAN  .. ins['description']    .. TERM_COLOR_DEFAULT)

    end

end
