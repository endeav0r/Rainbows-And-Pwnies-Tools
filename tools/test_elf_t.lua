elf = elf_t.new(argv[1])

print(elf:filename() .. " is a " .. elf:class() .. " bit elf with " ..
      tostring(elf:shnum()) .. " sections")

print(elf:shnum())

for i = 0,elf:shnum():int()-1 do
    section = elf:section(i)
    print(section:address():strx() .. " " .. section:name())
end

function dontcollect (filename, section_index)
    elf3 = elf_t.new(filename)
    elf2 = elf3
    section = elf2:section(section_index)
    return section
end

section = dontcollect(argv[1], ".text")
print("section 14 of " .. argv[1] .. " is " .. section:name() .. " with " .. 
      tostring(section:size()) .. " bytes")

print("disassembling " .. section:name())
disassembly = section:disassemble()

for i = 1,10 do
    print(disassembly[i]["description"])
end

print(#disassembly .. " instructions in " .. section:name())


rops = section:rop_table(1)
for i = 1,3 do
    print("rop")
    for ins_i, instruction in pairs(rops[i]) do
        print(instruction["address"]:strx() .. " " ..
              instruction["description"])
    end
end

--[[
symtab = elf:section(".symtab")
for i = 0,symtab:num()-1 do
    symbol = symtab:symbol(i)
    if #symbol:name() > 0 then
        print(symbol:name())
    end
end


relplt = elf:section(".rela.plt")
print(relplt:name())
for i = 0,relplt:num()-1 do
    relocation = relplt:relocation(i)
    print(relocation:offset():strx() .. " " ..
          relocation:type() .. " " ..
          relocation:name())
end
]]
