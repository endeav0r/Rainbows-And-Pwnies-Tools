dofile("tools/rop_tools.lua")

--[[
[rop_index] table
    [rop_ins_i] table
        ["address"] integer
        ["description"] string
        ["mnemonic"] string
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

rop_list = make_rop_table("rop_tools", 1)


-- go through each rop pair
for k, rop_ins in pairs(rop_list) do
    print("rop")
    -- for each rop instruction
    for k, ins in pairs(rop_ins) do
        print("  " .. tostring(w2h(ins["address"])) .. " " .. ins["description"])
        for operand_i, operand in pairs(ins["operands"]) do
            if operand["type"] ~= "none" then
                print("    " .. tostring(operand_i))
                for k, data in pairs(operand) do
                    print("      " .. k .. "\t" .. data)
                end
            end
        end
    end
end
