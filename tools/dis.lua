dofile("tools/rop_tools.lua")

for i, v in pairs(argv) do
    print (tostring(i) .. " " .. v)
end

functions = dis_by_function(argv[1])

for name, func in pairs(functions) do
    print(w2h(func["address"]) .. " " .. name)
    for ins_i, instruction in pairs(func["instructions"]) do
        print("  " .. w2h(instruction["address"]) .. 
              "  " .. instruction["description"])
    end
end
