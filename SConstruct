env = Environment()

env.AppendUnique(CFLAGS=['-O2', '-Wall', '-g'])

def fullpath (sources, path) :
    return map(lambda x: path + x + '.c', sources)

src_c     = ['analyze', 'auxiliary', 'main', 'strings', 'types', 'rop']
src_bin_c = ['elf', 'exec', 'pe']
src_lua_c = ['lua', 'lua_dis', 'lua_exec', 'lua_elf', 'lua_types']
src_rta_c = ['mem', 'exp', 'rta_x86', 'vars', 'ops']
src_lib_c = ['list', 'aatree']

src_c     = fullpath(src_c,     'src/')
src_bin_c = fullpath(src_bin_c, 'src/bin/')
src_lua_c = fullpath(src_lua_c, 'src/lua/')
src_lib_c = fullpath(src_lib_c, 'src/lib/')
src_rta_c = fullpath(src_rta_c, 'src/rta/')

env.Program(target="rt",
            source=src_c + src_bin_c + src_lua_c + src_lib_c,
            LIBS=['udis86', 'lua', 'readline'])

