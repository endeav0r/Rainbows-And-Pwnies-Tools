#include <stdio.h>

#include "../src/exec.h"

int main (int argc, char * argv[])
{
    
    struct _exec         * exec;
    struct _exec_section section;
    int i;
    
    if (argc != 2) {
        printf("usage: %s <executable>\n", argv[0]);
        return -1;
    }
    
    exec = exec_open(argv[1]);
    
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF : printf("type: ELF\n"); break;
    case EXEC_TYPE_PE  : printf("type: PE\n"); break;
    }
    
    printf("num_sections: %d\n", exec_num_sections(exec));

    for (i = 0; i < exec_num_sections(exec); i++) {
        exec_section(exec, &section, i);
        printf("section %2d ", i);
        if (exec_section_types(&section) & EXEC_SECTION_TYPE_TEXT)
            printf("T");
        else
            printf(" ");
        if (exec_section_types(&section) & EXEC_SECTION_TYPE_SYMBOL)
            printf("S");
        else
            printf(" ");
        if (exec_section_types(&section) & EXEC_SECTION_TYPE_RELOCATION)
            printf("R");
        else
            printf(" ");
        printf(" ");
        printf("%s %d\n", exec_section_name(&section),
               exec_section_size(&section));
    }

    exec_destroy(exec);
    
    return 0;
}
