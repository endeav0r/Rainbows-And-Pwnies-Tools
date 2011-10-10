function table.contains(t, v)
    for i, j in pairs(t) do
        if j == v then
            return true
        end
    end
    return false
end


executable = exec_t.new(argv[1])
print(executable:size() .. " byte " .. executable:type())
print("num sections: " .. executable:num_sections())

for i,section in pairs(executable:sections()) do
    types = ""
    for i, type in pairs(section:types()) do
        types = types .. type .. " "
    end
    print(i .. " " .. section:address():strx() .. " " .. 
          section:name() .. " [" .. types .. "]")
    if table.contains(section:types(), "executable") then
        instructions = section:disassemble()
        for i=1,3 do
            print("    " .. instructions[i]["address"]:strx() .. 
                  " " .. instructions[i]["description"])
        end
    end 
end

for i, address in pairs(executable:find_functions()) do
    print(address:str0x())
end
