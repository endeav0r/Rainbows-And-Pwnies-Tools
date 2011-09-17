dofile("tools/rop_tools.lua")

rop_list = make_rop_table("rop_tools", 1)

for rop_ins in pairs(rop_list) do
    print("rop")
    for ins in pairs(rop_ins) do
        print("\t" .. w2h(ins["address"]) .. " " .. ins["description"])
    end
end

