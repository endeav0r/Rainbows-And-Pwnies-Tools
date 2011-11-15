-- a global cache of all hooks
-- hooks is a singleton for each exec
global_hooks = {}

hooks = {}
hooks.mt = {}
setmetatable(hooks, hooks.mt)

hooks.hook = {}
hooks.hook.mt = {}
hooks.hook.mt.f = {}
hooks.hook.mt.__index = hooks_index

hooks.hook.functions = {}
hooks.hook.functions.mt = {}
hooks.hook.functions.mt.f = {}
hooks.hook.functions.mt.__index = hooks_index


function hooks_index (o, i)
    local mt = getmetatable(o)
    if mt.f[i] ~= nil then return mt.f[i] end
    return o[i]
end


function hooks.mt.__call (hooks, exec)
    if exec == nil then print('hooks got nil exec') end
    if global_hooks[exec] == nil then
        global_hooks[exec] = {}
        global_hooks[exec].exec = exec
        setmetatable(global_hooks[exec], hooks.hook.mt)
        global_hooks[exec].functions = {}
        setmetatable(global_hooks[exec].functions, hooks.hook.functions.mt)
    end
    return global_hooks[exec]
end


function hooks.hook.mt.f.init (hook)
    if #hook.functions == 0 then
        for fi, f in pairs(hook.exec:find_functions()) do
            f['name'] = describe_address(hook.exec, f['address'])
            hook.functions:insert(f)
        end
    end
end


function hooks.hook.functions.mt.f.search (functions, key, needle)
    for fi, f in pairs(functions) do
        if f[key] == needle then return f end
    end
    return nil
end

function hooks.hook.functions.mt.f.g_address (functions, address)
    return functions:search('address', address)
end

function hooks.hook.functions.mt.f.g_name (functions, name)
    return functions:search('name', name)
end

function hooks.hook.functions.mt.f.insert (functions, f)
    table.insert(functions, f)
end


function hooks_test (exec)
    local hook = hooks(exec)
    hook:init()
    
    hook = hooks(exec)

    for fi, f in pairs(functions) do
        print(hook.functions:g_address(f['address'])['name'])
    end
end
