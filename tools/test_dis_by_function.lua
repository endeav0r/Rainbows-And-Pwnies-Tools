print("1")

functions = dis_by_function("rop_tools")
functions = dis_by_function("rop_tools")

print("2")

for name, instructions in pairs(functions) do
    print(name)
    for ins_i, instruction in pairs(instructions) do
        print (tostring(ins_i) .. "\t" .. instruction["description"])
    end
end
