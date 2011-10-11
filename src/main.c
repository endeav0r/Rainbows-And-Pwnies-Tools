#include <stdio.h>
#include <unistd.h>

#include "bin/exec.h"
#include "lua/lua.h"
#include "rop.h"
#include "types.h"


int print_rop_list (struct _exec_section * section, struct _rop_list * rops)
{
    struct _rop_ins  * rop_ins;
    int rop_offset, total_gadgets = 0;
    char bytes_string[8 * 3 + 1];
    int byte_i;
    uint_t tmp_addr;
            
    while (rops != NULL) {
        total_gadgets++;
        
        rop_ins = rop_list_ins(rops);
        rop_offset = 0;
        
        uint_t_set(&tmp_addr, exec_section_address(section));
        uint_t_add_int(&tmp_addr, rops->offset);
        
        while (rop_ins != NULL) {
            bytes_string[0] = '\0';
            for (byte_i = 0;
                 (byte_i < 8) && (byte_i < rop_ins->bytes_size);
                 byte_i++)
                sprintf(&(bytes_string[byte_i * 3]),
                        "%02x ", rop_ins->bytes[byte_i]);
            while (byte_i++ < 8)
                strcat(bytes_string, "-- ");
            
            uint_t_set(&tmp_addr, exec_section_address(section));
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


int print_rops (struct _exec * exec, int rop_depth, int ret_rop, int jmp_rop,
                int call_rop, int cond_jmp_rop)
{
    struct _exec_section section;
    struct _rop_list * rops;
    int i;
    int total_gadgets = 0;
    
    for (i = 0; i < exec_num_sections(exec); i++) {
        exec_section(exec, &section, i);
        
        if (exec_section_types(&section) & EXEC_SECTION_TYPE_EXECUTABLE) {
            printf("section: %s\n", exec_section_name(&section));
            
            if (ret_rop) {
                rops = rop_ret_rops(exec_section_data(&section),
                                    exec_section_size(&section),
                                    rop_depth,
                                    exec_mode(exec));
                total_gadgets += print_rop_list(&section, rops);
                rop_list_destroy(rops);
            }
            
            if (jmp_rop) {
                rops = rop_jmp_reg_rops(exec_section_data(&section),
                                        exec_section_size(&section),
                                        rop_depth,
                                        exec_mode(exec));
                total_gadgets += print_rop_list(&section, rops);
                rop_list_destroy(rops);
            }
            
            if (cond_jmp_rop) {
                rops = rop_cond_jmp_reg_rops(exec_section_data(&section),
                                             exec_section_size(&section),
                                             rop_depth,
                                             exec_mode(exec));
                total_gadgets += print_rop_list(&section, rops);
                rop_list_destroy(rops);
            }
            
            if (call_rop) {
                rops = rop_call_reg_rops(exec_section_data(&section),
                                         exec_section_size(&section),
                                         rop_depth,
                                         exec_mode(exec));
                total_gadgets += print_rop_list(&section, rops);
                rop_list_destroy(rops);
            }
        }
    }
    
    return total_gadgets;
}


int main (int argc, char * argv[])
{
    struct _exec * exec;
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
        printf("%s [-cjr] [-d depth] (-e <executable> | -l <lua_file> [args])\n", argv[0]);
        printf("  -c              search for call reg gadgets\n");
        printf("  -d <depth>      depth, in instructions, to search backwards\n");
        printf("  -e <executable> filename of executable to analyze\n");
        printf("  -j              search for jmp reg gadgets\n");
        printf("  -k              search for conditional jmp reg gadgets (for when your day is \n");
        printf("                  really going that bad, and probably won't return anything)\n");
        printf("  -l <lua>        runs lua script\n");
        printf("  -r              search for ret gadgets\n");
        exit(-1);
    }
    
    
    if (lua_run)
        lua_run_file(filename, &(argv[optind]), argc - optind);
    
    else {
        exec = exec_open(filename);
        if (exec != NULL) {
            total_gadgets = print_rops(exec, rop_depth, ret_rop, jmp_rop,
                                       call_rop, cond_jmp_rop);
            exec_destroy(exec);
            printf("%d gadgets\n", total_gadgets);
        }
    }

    return 0;
}
