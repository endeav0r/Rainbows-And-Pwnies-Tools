executable = exec_t.new(argv[1])

section = executable:section(".text")
instructions = section:disassemble()

for i, instruction in pairs(instructions) do
    print(instruction["description"])
    for o, operand in pairs(instruction["operands"]) do
        if operand["type"] == "mem" then
            print("  operand " .. o)
            for k, v in pairs(operand) do
                print("    " .. k .. " " .. tostring(v))
            end
        end
    end
end
