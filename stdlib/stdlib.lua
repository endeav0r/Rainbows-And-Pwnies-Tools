TERM_COLOR_BLACK   = "\027[30m"
TERM_COLOR_RED     = "\027[31m"
TERM_COLOR_GREEN   = "\027[32m"
TERM_COLOR_YELLOW  = "\027[33m"
TERM_COLOR_BLUE    = "\027[34m"
TERM_COLOR_MAGENTA = "\027[35m"
TERM_COLOR_CYAN    = "\027[36m"
TERM_COLOR_WHITE   = "\027[37m"
TERM_COLOR_DEFAULT = "\027[39m"
TERM_BOLD          = "\027[1m"
TERM_NORMAL        = "\027[22m"

function table.contains(t, v)
    for i, j in pairs(t) do
        if j == v then
            return true
        end
    end
    return false
end


-- assumes tables has been sorted with cmp function
function table.find(t, value, cmp)
    if #t == 0 then
        return false
    end

    local min = 1
    local max = #t
    local mid
    while min < max do
        mid = (min + max) / 2
        if cmp(t[mid], value) < 0 then
            max = mid - 1
        elseif cmp(t[mid], value) > 0 then
            min = min + 1
        else
            return mid
        end
    end
    return false
end


function sub_table (super_table, func)
    local sub = {}
    for ti, t in pairs(super_table) do
        if func(t) then sub[ti] = t end
    end
    return sub
end


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


dofile("stdlib/disassembly.lua")
dofile("stdlib/description.lua")

dofile("stdlib/datadump.lua")
