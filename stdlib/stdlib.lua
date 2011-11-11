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


dofile("stdlib/datadump.lua")
