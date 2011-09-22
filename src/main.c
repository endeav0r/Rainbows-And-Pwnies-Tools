#include <stdio.h>
#include <unistd.h>

#include "elf.h"
#include "lua.h"
#include "rop.h"
#include "types.h"

int print_rop_list (struct _elf_shdr * shdr, struct _rop_list * rops)
{
    struct _elf * elf = shdr->elf;
    struct _elf_sym  sym;
    struct _rop_ins  * rop_ins;
    int rop_offset, total_gadgets = 0;
    char bytes_string[8 * 3 + 1];
    int byte_i;
    uint_t tmp_addr;
            
    while (rops != NULL) {
        total_gadgets++;
        
        rop_ins = rop_list_ins(rops);
        rop_offset = 0;
        
        uint_t_set(&tmp_addr, shdr_addr(shdr));
        uint_t_add_int(&tmp_addr, rops->offset);
        if (elf_sym_func_addr(elf, &sym, &tmp_addr))
            printf("%s + 0x%x\n", sym_name(&sym),
                   (uint_t_get(shdr_addr(shdr)) + rops->offset)
                   - uint_t_get(sym_value(&sym)));
        else
            printf("%s + 0x%x\n", shdr_name(shdr),
                   uint_t_get(shdr_addr(shdr)) + rops->offset);
    
        
        while (rop_ins != NULL) {
            bytes_string[0] = '\0';
            for (byte_i = 0;
                 (byte_i < 8) && (byte_i < rop_ins->bytes_size);
                 byte_i++)
                sprintf(&(bytes_string[byte_i * 3]),
                        "%02x ", rop_ins->bytes[byte_i]);
            while (byte_i++ < 8)
                strcat(bytes_string, "-- ");
            
            uint_t_set(&tmp_addr, shdr_addr(shdr));
            uint_t_add_int(&tmp_addr, rop_ins->offset);
            printf("  0x%s:  %s  %s\n",
                   uint_t_strx(&tmp_addr),
                   bytes_string,
                   rop_ins->description);
            rop_ins = rop_ins->next;
        }
        printf("\n");
        rops = rops->next;
    }
    
    return total_gadgets;
}


int print_rops (struct _elf * elf, int rop_depth, int ret_rop, int jmp_rop,
                int call_rop, int cond_jmp_rop)
{
    struct _elf_shdr shdr;
    struct _rop_list * rops;
    int i;
    int total_gadgets = 0;
    int mode;

    switch (elf_class(elf)) {
    case ELFCLASS32 :
        mode = 32;
        break;
    case ELFCLASS64 :
        mode = 64;
        break;
    }
    
    for (i = 0; i < int_t_get(elf_shnum(elf)); i++) {
        elf_shdr(elf, &shdr, i);
        
        if (shdr_exec(&shdr)) {
            printf("section: %s\n", shdr_name(&shdr));
            
            if (ret_rop) {
                rops = rop_ret_rops(shdr_data(&shdr),
                                    int_t_get(shdr_size(&shdr)),
                                    rop_depth,
                                    mode);
                total_gadgets += print_rop_list(&shdr, rops);
                rop_list_destroy(rops);
            }
            
            if (jmp_rop) {
                rops = rop_jmp_reg_rops(shdr_data(&shdr),
                                        int_t_get(shdr_size(&shdr)),
                                        rop_depth,
                                        mode);
                total_gadgets += print_rop_list(&shdr, rops);
                rop_list_destroy(rops);
            }
            
            if (cond_jmp_rop) {
                rops = rop_cond_jmp_reg_rops(shdr_data(&shdr),
                                             int_t_get(shdr_size(&shdr)),
                                             rop_depth,
                                             mode);
                total_gadgets += print_rop_list(&shdr, rops);
                rop_list_destroy(rops);
            }
            
            if (call_rop) {
                rops = rop_call_reg_rops(shdr_data(&shdr),
                                         int_t_get(shdr_size(&shdr)),
                                         rop_depth,
                                         mode);
                total_gadgets += print_rop_list(&shdr, rops);
                rop_list_destroy(rops);
            }
        }
    }
    
    return total_gadgets;
}


int main (int argc, char * argv[])
{
    struct _elf * elf;
    char * filename = NULL;
    int lua_run = 0;
    int ret_rop = 0;
    int jmp_rop = 0;
    int cond_jmp_rop = 0;
    int call_rop = 0;
    int rop_depth = 1;
    int c;
    int total_gadgets = 0;
    
    while ((c = getopt(argc, argv, "cd:e:jkl:r")) != -1) {
        switch (c) {
        case 'c' :
            call_rop = 1;
            break;
        case 'd' :
            rop_depth = atoi(optarg);
            break;
        case 'e' :
            filename = optarg;
            break;
        case 'j' :
            jmp_rop = 1;
            break;
        case 'k' :
            cond_jmp_rop = 1;
            break;
        case 'l' :
            lua_run = 1;
            filename = optarg;
            break;
        case 'r' :
            ret_rop = 1;
            break;
        case ':' :
            fprintf(stderr, "missing required optarg for %c\n", optopt);
            exit(-1);
        case '?' :
            fprintf(stderr, "unknown optarg %c\n", optopt);
            exit(-1);
        }
    }
    
    if (filename == NULL) {
        printf("rop_tools\n");
        printf("brought to you by rainbowsandpwnies\n");
        printf("\n");
        printf("%s [-cjr] [-d depth] (-e <elf> | -l <lua_file)\n", argv[0]);
        printf("  -c         search for call reg gadgets\n");
        printf("  -d <depth> depth, in instructions, to search backwards\n");
        printf("  -e <elf>   filename of elf to analyze\n");
        printf("  -j         search for jmp reg gadgets\n");
        printf("  -k         search for conditional jmp reg gadgets (for when your day is \n");
        printf("             really going that bad, and probably won't return anything)\n");
        printf("  -l <lua>   runs lua script\n");
        printf("  -r         search for ret gadgets\n");
        exit(-1);
    }
    
    
    if (lua_run)
        lua_run_file(filename);
    else {
        elf = elf_open(filename);
        if (elf != NULL) {
            total_gadgets = print_rops(elf, rop_depth, ret_rop, jmp_rop,
                                       call_rop, cond_jmp_rop);
            elf_destroy(elf);
            printf("%d gadgets\n", total_gadgets);
        }
    }

    return 0;
}
