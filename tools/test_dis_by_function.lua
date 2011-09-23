functions = dis_by_function("rop_tools")

for name, instructions in pairs(functions) do
    print(name)
    for ins_i, instruction in pairs(instructions) do
        print("  " .. tostring(ins_i))
        for name, value in pairs(instruction) do
            print("    " .. name .. "\t" .. tostring(value))
        end
    end
end
