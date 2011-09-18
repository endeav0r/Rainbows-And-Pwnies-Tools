dofile("tools/rop_tools.lua")

--[[
["shnum"] integer
["sections"] table
    [section_i] integer
        ["name"] string
        ["address"] integer
        ["executable"] boolean
        ["size"] integer
        ["type"] string
        ["symbols"] table
            [symbol_i] integer
                ["name"] string
                ["address"] integer
                ["type"] string
]]

elf = elf_open("rop_tools")

print("shnum " .. tostring(elf["shnum"]))

-- go through each section
for k, section in pairs(elf["sections"]) do
    print("  name:    " .. section["name"])
    print("  address: " .. w2h(section["address"]))
    print("  size:    " .. tostring(section["size"]))
    print("  type:    " .. section["type"])
    -- print symbols
    if section["symbols"] ~= nil then
        for k, symbol in pairs(section["symbols"]) do
            print("    " .. tostring(k) .. " name:    " .. symbol["name"])
            print("    " .. tostring(k) .. " address: " .. w2h(symbol["address"]))
            print("    " .. tostring(k) .. " type:    " .. symbol["type"])
        end
    end
end
