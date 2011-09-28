elf = elf_t.new("rop_tools")

print(elf:filename() .. " is a " .. elf:class() .. " bit elf with " ..
      tostring(elf:shnum()) .. " sections")

print(elf:shnum())

for i = 0,elf:shnum():int()-1 do
    section = elf:section(i)
    print(section:name())
end

function dontcollect (filename, section_index)
    elf = elf_t.new(filename)
    section = elf:section(section_index)
    return section
end

section = dontcollect("rop_tools", 14)
print("section 14 of rop_tools is " .. section:name())
