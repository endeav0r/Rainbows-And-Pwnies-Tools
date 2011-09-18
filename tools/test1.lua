dofile("tools/rop_tools.lua")

--[[
[rop_index]
    [address]
    [rop_ins_i]
        [address]
        [description]
        [mnemonic]
    [rop_ins_i]
        [address]
        [description]
        [mnemonic]
]]

rop_list = make_rop_table("rop_tools", 1)

-- go through each rop pair
for k, rop_ins in pairs(rop_list) do
    print(w2h(rop_ins["address"]))
    -- for each rop instruction
    for k, ins in pairs(rop_ins) do
        -- address is in here, filter it out
        if k ~= "address" then
            print("  " .. tostring(w2h(ins["address"])) .. " " .. ins["description"])
        end
        --print("\t" .. w2h(rop_list[ins]["address"]) .. " " .. rop_list[ins]["description"])
    end
end

