env = Environment()

env.AppendUnique(CFLAGS=['-O2', '-Wall', '-g'])

src_c     = ['analyze', 'auxiliary', 'main', 'strings', 'types', 'rop']
src_bin_c = ['elf', 'exec', 'pe']
src_lua_c = ['lua', 'lua_dis', 'lua_exec', 'lua_elf', 'lua_types']
src_rta_c = ['mem', 'exp', 'rta_x86', 'vars']
         
sources_c =  map(lambda x: 'src/' + x + '.c', src_c)
sources_c += map(lambda x: 'src/lua/' + x + '.c', src_lua_c)
sources_c += map(lambda x: 'src/bin/' + x + '.c', src_bin_c) 
sources_c += map(lambda x: 'src/rta/' + x + '.c', src_rta_c) 

env.Program(target="rop_tools",
            source=sources_c,
            LIBS=['udis86', 'lua'])
        
