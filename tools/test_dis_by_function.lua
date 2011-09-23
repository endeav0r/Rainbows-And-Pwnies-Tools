--[[
["name"]
    [ins_i]
        ["address"]
        ["description"]
        ["mnemonic"]
        ["operands"] table
            [operand_i] integer
                ["type"] string
                ["base"] string
                ["index"] string
                ["offset"] string
                ["scale"] string
                ["size"] integer
                ["lval"] integer
]]

dofile("tools/rop_tools.lua")

functions = dis_by_function("rop_tools")

for name, instructions in pairs(functions) do
    print(name)
    for ins_i, instruction in pairs(instructions) do
        print("  " .. w2h(instruction["address"]))
        for name, value in pairs(instruction) do
            if name ~= "address" then
                print("    " .. name .. "\t" .. tostring(value))
            end
        end
    end
end
