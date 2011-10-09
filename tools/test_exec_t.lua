executable = exec_t.new(argv[1])
print(executable:size() .. " byte " .. executable:type())
print("num sections: " .. executable:num_sections())

for i=0,executable:num_sections()-1 do
    print(executable:section(i):name())
end
