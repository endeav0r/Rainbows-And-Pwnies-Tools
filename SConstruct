env = Environment()

env.AppendUnique(CFLAGS=['-O2', '-Wall', '-g'])

src_c     = ['analyze', 'aux', 'main', 'strings', 'types', 'rop']
src_bin_c = ['elf', 'exec', 'pe']
src_lua_c = ['lua', 'lua_dis', 'lua_exec', 'lua_elf', 'lua_types']
         
sources_c =  map(lambda x: 'src/' + x + '.c', src_c)
sources_c += map(lambda x: 'src/lua/' + x + '.c', src_lua_c)
sources_c += map(lambda x: 'src/bin/' + x + '.c', src_bin_c) 

env.Program(target="rop_tools",
            source=sources_c,
            LIBS=['udis86', 'lua5.1'])
        
