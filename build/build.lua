#!/usr/bin/lua

CC_LIBS = "-llua -lreadline"

function map (func, table)
    local new_table = {}
    for i,v in pairs(table) do new_table[i] = func(v) end
    return new_table
end

function table.join (a, b)
    local new_table = {}
    for k,v in pairs(a) do
        table.insert(new_table, v)
    end
    for k,v in pairs(b) do
        table.insert(new_table, v)
    end
    return new_table
end

rt_sources = {'src/analyze', 'src/auxiliary', 'src/main', 'src/strings', 'src/types', 'src/rop', 'src/bin/elf', 'src/bin/exec', 'src/bin/pe', 'src/lua/lua', 'src/lua/lua_dis', 'src/lua/lua_exec', 'src/lua/lua_elf', 'src/lua/lua_types', 'src/lib/list', 'src/lib/aatree'}

udis86_sources = {'src/udis86/decode', 'src/udis86/input', 'src/udis86/itab', 'src/udis86/syn-att', 'src/udis86/syn-intel', 'src/udis86/syn', 'src/udis86/udis86'}

sources = table.join(rt_sources, udis86_sources)

object_files = map(function(s) return s .. ".o" end, sources)

function execute (command)
    print(command)
    local status = os.execute(command)
    if status ~= 0 then 
        os.exit(status)
    end
end

