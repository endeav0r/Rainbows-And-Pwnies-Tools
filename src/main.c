#include <stdio.h>
#include <unistd.h>

#include "elf.h"
#include "rop.h"

int print_rop_list (struct _elf_shdr * shdr, struct _rop_list * rops)
{
    struct _elf * elf = shdr->elf;
    struct _elf_sym  sym;
    struct _rop_ins  * rop_ins;
    int rop_offset, total_gadgets = 0;
            
    while (rops != NULL) {
        total_gadgets++;
        
        rop_ins = rop_list_ins(rops);
        rop_offset = 0;
        
        if (elf_sym_func_addr(elf, &sym, rops->offset + shdr_addr(shdr)))
            printf("%s + %x\n", sym_name(&sym),
                   (shdr_addr(shdr) + rops->offset) - sym_addr(&sym));
        else
            printf("%s + %x\n", shdr_name(shdr),
                   shdr_addr(shdr) + rops->offset);
    
        
        while (rop_ins != NULL) {
            printf("  %08x  %s\n",
                   shdr_addr(shdr) + rop_ins->offset,
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
    
    for (i = 0; i < elf_shnum(elf); i++) {
        elf_shdr(elf, &shdr, i);
        
        if (shdr_exec(&shdr)) {
            printf("section: %s\n", shdr_name(&shdr));
            
            if (ret_rop) {
                rops = rop_ret_rops(shdr_data(&shdr), shdr_size(&shdr), rop_depth);
                total_gadgets += print_rop_list(&shdr, rops);
                rop_list_destroy(rops);
            }
            
            if (jmp_rop) {
                rops = rop_jmp_reg_rops(shdr_data(&shdr), shdr_size(&shdr), rop_depth);
                total_gadgets += print_rop_list(&shdr, rops);
                rop_list_destroy(rops);
            }
            
            if (cond_jmp_rop) {
                rops = rop_cond_jmp_reg_rops(shdr_data(&shdr), shdr_size(&shdr), rop_depth);
                total_gadgets += print_rop_list(&shdr, rops);
                rop_list_destroy(rops);
            }
            
            if (call_rop) {
                rops = rop_call_reg_rops(shdr_data(&shdr), shdr_size(&shdr), rop_depth);
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
    int ret_rop = 0;
    int jmp_rop = 0;
    int cond_jmp_rop = 0;
    int call_rop = 0;
    int rop_depth = 1;
    int c;
    int total_gadgets = 0;
    
    while ((c = getopt(argc, argv, "cd:e:jkr")) != -1) {
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
        printf("%s [-rj] [-d depth] -e <elf>\n", argv[0]);
        printf("  -d <depth> depth, in instructions, to search backwards\n");
        printf("  -e <elf>   filename of elf to analyze\n");
        printf("  -j         search for jmp reg gadgets\n");
        printf("  -k         search for conditional jmp reg gadgets (for when your day is \n");
        printf("             really going that bad, and probably won't return anything)\n");
        printf("  -r         search for ret gadgets\n");
        exit(-1);
    }
    
    elf = elf_open(filename);
    
    total_gadgets = print_rops(elf, rop_depth, ret_rop, jmp_rop,
                               call_rop, cond_jmp_rop);
    
    elf_destroy(elf);
    
    printf("%d gadgets\n", total_gadgets);

    return 0;
}
