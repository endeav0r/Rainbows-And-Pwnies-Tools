function describe_address_symbol (exec, address)
    for si, s in pairs(exec:symbols()) do
        if s:address() == address then
            return s:name()
        end
    end
    return nil
end

function describe_address_section (exec, address)
    for si, s in pairs(exec:sections()) do
        if s:address() <= address and
           s:address() + uint_t.new(s:address():size(), s:size()) > address then
            return  s:name() .. '+' .. (address - s:address()):strx() 
        end
    end
    return nil
end

function describe_address (exec, address)
    local description = describe_address_symbol(exec, address)
    if description ~= nil then return description end
    return describe_address_section(exec, address)
end
