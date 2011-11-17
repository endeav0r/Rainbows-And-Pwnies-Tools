function is_jump (mnemonic)
    if mnemonic == "jmp" or
       mnemonic == "jo" or
       mnemonic == "jno" or
       mnemonic == "jb" or
       mnemonic == "jae" or
       mnemonic == "jz" or
       mnemonic == "jnz" or
       mnemonic == "jbe" or
       mnemonic == "ja" or
       mnemonic == "js" or
       mnemonic == "jp" or
       mnemonic == "jnp" or
       mnemonic == "jl" or
       mnemonic == "jge" or
       mnemonic == "jle" or
       mnemonic == "jg" then
        return true
    end
    return false
end


function operand_abs (instruction, operand_i)
    local operand = instruction['operands'][operand_i]
    local absolute

    -- if this operand has an index or scale, don't describe it
    if operand['base'] ~= 'none' or operand['scale'] > 0 then
        return nil
    end
    
    absolute = operand['lval']

    if (operand['type'] == 'mem' and operand['base'] == 'rip')
       or (operand['type'] == 'jimm') then
        absolute = absolute + instruction['address']:int_t()
        absolute = absolute + int_t.new(absolute:size(), instruction['size'])
    end
    
    return absolute
end

