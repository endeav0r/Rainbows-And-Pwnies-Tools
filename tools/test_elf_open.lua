dofile("tools/rop_tools.lua")

--[[
["shnum"] integer
["sections"] table
    ["name" or index if name is NULL] string
        ["address"] integer
        ["executable"] boolean
        ["size"] integer
        ["type"] string
        ["symbols"] table
            ["name" or index if name is NULL] integer
                ["address"] integer
                ["type"] string
]]

elf = elf_read("rop_tools")

print("shnum " .. tostring(elf["shnum"]))

-- go through each section
for name, section in pairs(elf["sections"]) do
    print(name)
    print("  address: " .. w2h(section["address"]))
    print("  size:    " .. tostring(section["size"]))
    print("  type:    " .. section["type"])
    
    if section["type"] == "rel" then
        for name, rel in pairs(section["relocations"]) do
            print("    " .. name .. " => " .. w2h(rel["offset"]))
        end
    end
    
    if section["symbols"] ~= nil then
        for name, symbol in pairs(section["symbols"]) do
            print("    " .. name)
            print("      address: " .. w2h(symbol["address"]))
            print("      type:    " .. symbol["type"])
        end
    end
end
