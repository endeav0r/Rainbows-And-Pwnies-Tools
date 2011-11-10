elf = elf_t.new(argv[1])

function operand_abs (operand, address, size)
    local absolute
    if (operand["base"] ~= "none" and operand["base"] ~= "rip") or 
        operand["index"] ~= "none" then
        return false
    elseif operand["type"] == "jimm" or 
           (operand["type"] == "mem" and operand["base"] == "rip") then
        absolute = address + uint_t.new(32, size)
        if operand["lval"]:int() < 0 then
            absolute = absolute - (operand["lval"] * int_t.new(8, -1)):uint_t()
        else
            absolute = absolute + operand["lval"]:uint_t()
        end
    elseif operand["type"] == "mem" then
        absolute = operand["lval"]:uint_t()
    else
        return false
    end
    return absolute
end


function print_strings (instructions, data_section)
    for ii, ins in pairs(instructions) do
        absolute_address = operand_abs(ins['operands'][2], ins['address'], ins['size'])
        if not absolute_address then absolute_address = ins['operands'][2]['lval']:uint_t() end
        if absolute_address and
            ins['mnemonic'] == 'mov' and
            absolute_address >= data_section:address() and
            absolute_address < data_section:address() + data_section:size():uint_t() then
            local s = ''
            local c = data_section:mem_at_address(absolute_address, 1, 8)[1]
            while c:int() ~= 0 do
                s = s .. string.char(c:int())
                absolute_address = absolute_address + uint_t.new(8, 1)
                c = data_section:mem_at_address(absolute_address, 1, 8)[1]
            end
            print(ins['address']:str0x() .. " -> " .. absolute_address:str0x() .. ": " .. s)
        end
    end
end


instructions = elf:section(".symtab"):symbol('main'):disassemble()

--print_strings(instructions, elf:section(".data"))
--print_strings(instructions, elf:section(".bss"))
print_strings(instructions, elf:section(".rodata"))
