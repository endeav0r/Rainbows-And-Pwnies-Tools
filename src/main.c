#include <stdio.h>
#include <unistd.h>

#include "elf.h"
#include "rop.h"

int print_ret_rops (struct _elf * elf, int rop_depth)
{
    struct _elf_shdr shdr;
    struct _elf_sym  sym;
    struct _rop_list * rops;
    struct _rop_list * rops_first;
    struct _rop_ins  * rop_ins;
    int i, rop_offset, total_gadgets = 0;
    
    for (i = 0; i < elf_shnum(elf); i++) {
        elf_shdr(elf, &shdr, i);
        
        if (shdr_exec(&shdr)) {
            rops_first = rop_ret_rops(shdr_data(&shdr),
                                      shdr_size(&shdr),
                                      rop_depth);
            rops = rops_first;
            printf("section: %s\n", shdr_name(&shdr));
            
            while (rops != NULL) {
                total_gadgets++;
                
                rop_ins = rop_list_ins(rops);
                rop_offset = 0;
                
                if (elf_sym_func_addr(elf, &sym, rops->offset + shdr_addr(&shdr)))
                    printf("%s + %x\n", sym_name(&sym),
                           (shdr_addr(&shdr) + rops->offset) - sym_addr(&sym));
                else
                    printf("%s + %x\n", shdr_name(&shdr),
                           shdr_addr(&shdr) + rops->offset);
            
                
                while (rop_ins != NULL) {
                    printf("  %08x  %s\n",
                           shdr_addr(&shdr) + rop_ins->offset,
                           rop_ins->description);
                    rop_ins = rop_ins->next;
                }
                printf("\n");
                rops = rops->next;
            }
            
            rop_list_destroy(rops_first);
        }
    }
    
    return total_gadgets;
}


int main (int argc, char * argv[])
{
    struct _elf * elf;
    char * filename = NULL;
    int ret_rop = 0;
    int rop_depth = 1;
    int c;
    int total_gadgets = 0;
    
    while ((c = getopt(argc, argv, "e:rd:")) != -1) {
        switch (c) {
        case 'e' :
            filename = optarg;
            break;
        case 'r' :
            ret_rop = 1;
            break;
        case 'd' :
            rop_depth = atoi(optarg);
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
        printf("%s [-r] [-d depth] -e <elf>\n", argv[0]);
        printf("  -d <depth> depth, in instructions, to search backwards\n");
        printf("  -e <elf>   filename of elf to analyze\n");
        printf("  -r         search for ret gadgets\n");
        exit(-1);
    }
    
    elf = elf_open(filename);
    
    if (ret_rop)
        total_gadgets = print_ret_rops(elf, rop_depth);
    
    elf_destroy(elf);
    
    printf("%d gadgets\n", total_gadgets);

    return 0;
}
