JSON = dofile(STDLIB_PATH .. 'JSON.lua')

-- hooks is a singleton for each exec
global_hooks = {}

hooks = {}
hooks.mt = {}
setmetatable(hooks, hooks.mt)

hooks.hook = {}
hooks.hook.mt = {}
hooks.hook.mt.__index = hooks.hook

hooks.hook.functions = {}
hooks.hook.functions.mt = {}
hooks.hook.functions.mt.__index = hooks.hook.functions

hooks.hook.labels = {}
hooks.hook.labels.mt = {}
hooks.hook.labels.mt.__index = hooks.hook.labels


function hooks.mt.__call (hooks, exec)
    if exec == nil then print('hooks got nil exec') end
    if global_hooks[exec] == nil then
        global_hooks[exec] = hooks.hook:new(exec)
    end
    return global_hooks[exec]
end

---------------------------------------------------------------------
-- hooks.hook                                                      --
---------------------------------------------------------------------
function hooks.hook:new (e)
    hook = {exec      = e,
            functions = hooks.hook.functions:new(),
            labels    = hooks.hook.labels:new()}
    return setmetatable(hook, hooks.hook.mt)
end

function hooks.hook:init ()
    self.functions:init(self.exec)
    self.labels:init(self.exec)
end

function hooks.hook:dump (filename)
    json = {functions=self.functions:dump(), labels=self.labels:dump()}
    fh = io.open(filename, 'w')
    fh:write(JSON:encode(json))
    fh:close()
end

function hooks.hook:undump (filename)
    fh = io.open(filename, 'r')
    json = JSON:decode(fh:read())
    fh:close()
    self.functions:undump(json['functions'])
    self.labels:undump(json['labels'])
end



---------------------------------------------------------------------
-- hooks.hook.functions                                            --
---------------------------------------------------------------------
function hooks.hook.functions:new ()
    return setmetatable({functions={}}, hooks.hook.functions.mt)
end

function hooks.hook.functions:init (exec)
    if #self.functions == 0 then
        -- get functions based on call instructions
        for fi, f in pairs(exec:find_functions()) do
            f['name'] = describe_address(exec, f['address'])
            self:insert(f)
        end
        -- add functions from symbols
        for si, s in pairs(exec:symbols()) do
            if s:type() == 'function' and s:address():int() > 0 then
                local f = self:g_address(s:address())
                if f == nil then
                    self:insert({name=s:name(),
                                 address=s:address(),
                                 size=s:size()})
                elseif f['name'] == '' then
                    self:g_address(s:address())['name'] = s:name()
                end
            end
        end
        -- sort
        table.sort(self.functions, function (a, b) return a['address'] < b['address'] end)
        -- readjust sizes
        for fi=1,#self.functions-1 do
            self.functions[fi]['size'] = (self.functions[fi+1]['address'] - self.functions[fi]['address']):int()
        end
    end
end

-- dumps to a format capable of being json encoded
function hooks.hook.functions:dump ()
    dump = {}
    for fi, f in pairs(self.functions) do
        table.insert(dump, {name=f['name'], address=f['address']:str0x(), size=f['size']})
    end
    return dump
end

function hooks.hook.functions:undump (json)
    for fi, f in pairs(json) do
        f['address'] = uint_t.new(8):from_string(f['address'])
        self:d_address(f['address'])
        self:insert(f)
    end
end

function hooks.hook.functions:search (key, needle)
    for fi, f in pairs(self.functions) do if f[key] == needle then return f end end
    return nil
end

function hooks.hook.functions:g_address (address)
    return self:search('address', address)
end

function hooks.hook.functions:g_name (name)
    return self:search('name', name)
end

function hooks.hook.functions:d_address (address)
    for fi, f in pairs(self.functions) do
        if f['address'] == address then
            self.functions[fi] = nil
            break
        end
    end
end

function hooks.hook.functions:insert (f)
    table.insert(self.functions, f)
end

function hooks.hook.functions:get ()
    return self.functions
end



---------------------------------------------------------------------
-- hooks.hook.labels                                               --
---------------------------------------------------------------------
function hooks.hook.labels:new ()
    return setmetatable({labels={}}, hooks.hook.labels.mt)
end

function hooks.hook.labels:init (exec)
    if #self.labels == 0 then
        for section_i, section in pairs(exec:sections()) do
            if table.contains(section:types(), "executable") then
                for ii, ins in pairs(section:disassemble()) do
                    if is_jump(ins['mnemonic']) then
                        local destination = operand_abs(ins, 1)
                        if destination ~= nil then
                            self:insert(destination:uint_t(), 'destination for jmp at ' .. ins['address']:strx())
                        end
                    end
                end
            end
        end
    end
end

-- dumps to a format capable of being json encoded
function hooks.hook.labels:dump ()
    dump = {}
    for li, l in pairs(self.labels) do
        dump[li:str0x()] = l
    end
    return dump
end

function hooks.hook.labels:undump (json)
    for li, l in pairs(json) do
        local address = uint_t.new(8):from_string(li)
        if self.labels[address] == nil then
            self.labels[address] = {l}
        else
            table.insert(self.labels[address], l)
        end
    end
end

function hooks.hook.labels:g_address (address)
    -- for some reason currently unknown, self.labels[address] always returns nil
    for li, l in pairs(self.labels) do
        if li == address then return l end
    end
    return nil
end

function hooks.hook.labels:d_address (address)
    self.labels[address] = nil
end

function hooks.hook.labels:insert (address, label)
    if self.labels[address] == nil then self.labels[address] = {} end
    table.insert(self.labels[address], label)
end


function hooks_test (exec)
    local hook = hooks(exec)
    hook:init()
    
    hook = hooks(exec)

    for fi, f in pairs(hook.functions) do
        print(hook.functions:g_address(f['address'])['name'])
    end
end
