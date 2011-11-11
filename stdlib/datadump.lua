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

function datadump_largest_string (strings)
    local largest = 0
    for si, s in pairs(strings) do
        if #tostring(s) > largest then
            largest = #tostring(s)
        end
    end
    return largest
end


-------------------------------------------------
-- dump_sections                               --
-------------------------------------------------
function dump_sections (exec)
    local sections = exec:sections()
    local sizes = {}
    local addresses = {}
    
    for si, s in pairs(sections) do 
        table.insert(sizes, s:size())
        table.insert(addresses, s:address():strx())
    end
    local largest_size    = datadump_largest_string(sizes)
    local largest_address = datadump_largest_string(addresses)

    for si, section in pairs(sections) do
        
        local size = tostring(section:size())
        while #size < largest_size do
            size = size .. ' '
        end
        local address = section:address():strx()
        while #address < largest_address do
            address = '0' .. address
        end
        print(TERM_COLOR_GREEN .. address .. TERM_COLOR_DEFAULT .. ' ' ..
              size .. ' ' ..
              TERM_COLOR_CYAN .. section:name() .. TERM_COLOR_DEFAULT)
    end
end


-------------------------------------------------
-- dump_functions                              --
-------------------------------------------------
function dump_functions (exec)
    functions = exec:find_functions()
    
    symbols = exec:symbols()
    
    for fi, f in pairs(functions) do
        local function_name = "func_" .. f:strx()
        for si, s in pairs(symbols) do
            if s:address() == f then
                function_name = s:name()
                break
            end
        end
        print(TERM_COLOR_GREEN .. f:strx() .. TERM_COLOR_DEFAULT .. ' ' ..
              TERM_COLOR_CYAN .. function_name .. TERM_COLOR_DEFAULT)
    end
end
