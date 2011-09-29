elf = elf_t.new("rop_tools")

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

section = dontcollect("rop_tools", ".text")
print("section 14 of rop_tools is " .. section:name())

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

