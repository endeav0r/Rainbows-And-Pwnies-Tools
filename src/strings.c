#include "strings.h"

char mnemonic_strings[][16] = {
    "3dnow",
    "aaa",
    "aad",
    "aam",
    "aas",
    "adc",
    "add",
    "addpd",
    "addps",
    "addsd",
    "addss",
    "addsubpd",
    "addsubps",
    "and",
    "andpd",
    "andps",
    "andnpd",
    "andnps",
    "arpl",
    "movsxd",
    "bound",
    "bsf",
    "bsr",
    "bswap",
    "bt",
    "btc",
    "btr",
    "bts",
    "call",
    "cbw",
    "cwde",
    "cdqe",
    "clc",
    "cld",
    "clflush",
    "clgi",
    "cli",
    "clts",
    "cmc",
    "cmovo",
    "cmovno",
    "cmovb",
    "cmovae",
    "cmovz",
    "cmovnz",
    "cmovbe",
    "cmova",
    "cmovs",
    "cmovns",
    "cmovp",
    "cmovnp",
    "cmovl",
    "cmovge",
    "cmovle",
    "cmovg",
    "cmp",
    "cmppd",
    "cmpps",
    "cmpsb",
    "cmpsw",
    "cmpsd",
    "cmpsq",
    "cmpss",
    "cmpxchg",
    "cmpxchg8b",
    "comisd",
    "comiss",
    "cpuid",
    "cvtdq2pd",
    "cvtdq2ps",
    "cvtpd2dq",
    "cvtpd2pi",
    "cvtpd2ps",
    "cvtpi2ps",
    "cvtpi2pd",
    "cvtps2dq",
    "cvtps2pi",
    "cvtps2pd",
    "cvtsd2si",
    "cvtsd2ss",
    "cvtsi2ss",
    "cvtss2si",
    "cvtss2sd",
    "cvttpd2pi",
    "cvttpd2dq",
    "cvttps2dq",
    "cvttps2pi",
    "cvttsd2si",
    "cvtsi2sd",
    "cvttss2si",
    "cwd",
    "cdq",
    "cqo",
    "daa",
    "das",
    "dec",
    "div",
    "divpd",
    "divps",
    "divsd",
    "divss",
    "emms",
    "enter",
    "f2xm1",
    "fabs",
    "fadd",
    "faddp",
    "fbld",
    "fbstp",
    "fchs",
    "fclex",
    "fcmovb",
    "fcmove",
    "fcmovbe",
    "fcmovu",
    "fcmovnb",
    "fcmovne",
    "fcmovnbe",
    "fcmovnu",
    "fucomi",
    "fcom",
    "fcom2",
    "fcomp3",
    "fcomi",
    "fucomip",
    "fcomip",
    "fcomp",
    "fcomp5",
    "fcompp",
    "fcos",
    "fdecstp",
    "fdiv",
    "fdivp",
    "fdivr",
    "fdivrp",
    "femms",
    "ffree",
    "ffreep",
    "ficom",
    "ficomp",
    "fild",
    "fncstp",
    "fninit",
    "fiadd",
    "fidivr",
    "fidiv",
    "fisub",
    "fisubr",
    "fist",
    "fistp",
    "fisttp",
    "fld",
    "fld1",
    "fldl2t",
    "fldl2e",
    "fldlpi",
    "fldlg2",
    "fldln2",
    "fldz",
    "fldcw",
    "fldenv",
    "fmul",
    "fmulp",
    "fimul",
    "fnop",
    "fpatan",
    "fprem",
    "fprem1",
    "fptan",
    "frndint",
    "frstor",
    "fnsave",
    "fscale",
    "fsin",
    "fsincos",
    "fsqrt",
    "fstp",
    "fstp1",
    "fstp8",
    "fstp9",
    "fst",
    "fnstcw",
    "fnstenv",
    "fnstsw",
    "fsub",
    "fsubp",
    "fsubr",
    "fsubrp",
    "ftst",
    "fucom",
    "fucomp",
    "fucompp",
    "fxam",
    "fxch",
    "fxch4",
    "fxch7",
    "fxrstor",
    "fxsave",
    "fpxtract",
    "fyl2x",
    "fyl2xp1",
    "haddpd",
    "haddps",
    "hlt",
    "hsubpd",
    "hsubps",
    "idiv",
    "in",
    "imul",
    "inc",
    "insb",
    "insw",
    "insd",
    "int1",
    "int3",
    "int",
    "into",
    "invd",
    "invlpg",
    "invlpga",
    "iretw",
    "iretd",
    "iretq",
    "jo",
    "jno",
    "jb",
    "jae",
    "jz",
    "jnz",
    "jbe",
    "ja",
    "js",
    "jns",
    "jp",
    "jnp",
    "jl",
    "jge",
    "jle",
    "jg",
    "jcxz",
    "jecxz",
    "jrcxz",
    "jmp",
    "lahf",
    "lar",
    "lddqu",
    "ldmxcsr",
    "lds",
    "lea",
    "les",
    "lfs",
    "lgs",
    "lidt",
    "lss",
    "leave",
    "lfence",
    "lgdt",
    "lldt",
    "lmsw",
    "lock",
    "lodsb",
    "lodsw",
    "lodsd",
    "lodsq",
    "loopnz",
    "loope",
    "loop",
    "lsl",
    "ltr",
    "maskmovq",
    "maxpd",
    "maxps",
    "maxsd",
    "maxss",
    "mfence",
    "minpd",
    "minps",
    "minsd",
    "minss",
    "monitor",
    "mov",
    "movapd",
    "movaps",
    "movd",
    "movddup",
    "movdqa",
    "movdqu",
    "movdq2q",
    "movhpd",
    "movhps",
    "movlhps",
    "movlpd",
    "movlps",
    "movhlps",
    "movmskpd",
    "movmskps",
    "movntdq",
    "movnti",
    "movntpd",
    "movntps",
    "movntq",
    "movq",
    "movqa",
    "movq2dq",
    "movsb",
    "movsw",
    "movsd",
    "movsq",
    "movsldup",
    "movshdup",
    "movss",
    "movsx",
    "movupd",
    "movups",
    "movzx",
    "mul",
    "mulpd",
    "mulps",
    "mulsd",
    "mulss",
    "mwait",
    "neg",
    "nop",
    "not",
    "or",
    "orpd",
    "orps",
    "out",
    "outsb",
    "outsw",
    "outsd",
    "outsq",
    "packsswb",
    "packssdw",
    "packuswb",
    "paddb",
    "paddw",
    "paddq",
    "paddsb",
    "paddsw",
    "paddusb",
    "paddusw",
    "pand",
    "pandn",
    "pause",
    "pavgb",
    "pavgw",
    "pcmpeqb",
    "pcmpeqw",
    "pcmpeqd",
    "pcmpgtb",
    "pcmpgtw",
    "pcmpgtd",
    "pextrw",
    "pinsrw",
    "pmaddwd",
    "pmaxsw",
    "pmaxub",
    "pminsw",
    "pminub",
    "pmovmskb",
    "pmulhuw",
    "pmulhw",
    "pmullw",
    "pmuludq",
    "pop",
    "popa",
    "popad",
    "popfw",
    "popfd",
    "popfq",
    "por",
    "prefetch",
    "prefetchnta",
    "prefetcht0",
    "prefetcht1",
    "prefetcht2",
    "psadbw",
    "pshufd",
    "pshufhw",
    "pshuflw",
    "pshufw",
    "pslldq",
    "psllw",
    "pslld",
    "psllq",
    "psraw",
    "psrad",
    "psrlw",
    "psrld",
    "psrlq",
    "psrldq",
    "psubb",
    "psubw",
    "psubd",
    "psubq",
    "psubsb",
    "psubsw",
    "psubusb",
    "psubusw",
    "punpckhbw",
    "punpckhwd",
    "punpckhdq",
    "punpckhqdq",
    "punpcklbw",
    "punpcklwd",
    "punpckldq",
    "punpcklqdq",
    "pi2fw",
    "pi2fd",
    "pf2iw",
    "pf2id",
    "pfnacc",
    "pfpnacc",
    "pfcmpge",
    "pfmin",
    "pfrcp",
    "pfrsqrt",
    "pfsub",
    "pfadd",
    "pfcmpgt",
    "pfmax",
    "pfrcpit1",
    "pfrspit1",
    "pfsubr",
    "pfacc",
    "pfcmpeq",
    "pfmul",
    "pfrcpit2",
    "pmulhrw",
    "pswapd",
    "pavgusb",
    "push",
    "pusha",
    "pushad",
    "pushfw",
    "pushfd",
    "pushfq",
    "pxor",
    "rcl",
    "rcr",
    "rol",
    "ror",
    "rcpps",
    "rcpss",
    "rdmsr",
    "rdpmc",
    "rdtsc",
    "rdtscp",
    "repne",
    "rep",
    "ret",
    "retf",
    "rsm",
    "rsqrtps",
    "rsqrtss",
    "sahf",
    "sal",
    "salc",
    "sar",
    "shl",
    "shr",
    "sbb",
    "scasb",
    "scasw",
    "scasd",
    "scasq",
    "seto",
    "setno",
    "setb",
    "setnb",
    "setz",
    "setnz",
    "setbe",
    "seta",
    "sets",
    "setns",
    "setp",
    "setnp",
    "setl",
    "setge",
    "setle",
    "setg",
    "sfence",
    "sgdt",
    "shld",
    "shrd",
    "shufpd",
    "shufps",
    "sidt",
    "sldt",
    "smsw",
    "sqrtps",
    "sqrtpd",
    "sqrtsd",
    "sqrtss",
    "stc",
    "std",
    "stgi",
    "sti",
    "skinit",
    "stmxcsr",
    "stosb",
    "stosw",
    "stosd",
    "stosq",
    "str",
    "sub",
    "subpd",
    "subps",
    "subsd",
    "subss",
    "swapgs",
    "syscall",
    "sysenter",
    "sysexit",
    "sysret",
    "test",
    "ucomisd",
    "ucomiss",
    "ud2",
    "unpckhpd",
    "unpckhps",
    "unpcklps",
    "unpcklpd",
    "verr",
    "verw",
    "vmcall",
    "vmclear",
    "vmxon",
    "vmptrld",
    "vmptrst",
    "vmresume",
    "vmxoff",
    "vmrun",
    "vmmcall",
    "vmload",
    "vmsave",
    "wait",
    "wbinvd",
    "wrmsr",
    "xadd",
    "xchg",
    "xlatb",
    "xor",
    "xorpd",
    "xorps",
    "db",
    "invalid",
    "d3vil",
    "na",
    "grp_reg",
    "grp_rm",
    "grp_vendor",
    "grp_x87",
    "grp_mode",
    "grp_osize",
    "grp_asize",
    "grp_mod",
    "none"
};


char types_strings [][16] = {
    "none",
    "al",
    "cl",
    "dl",
    "bl",
    "ah",
    "ch",
    "dh",
    "bh",
    "spl",
    "bpl",
    "sil",
    "dil",
    "r8b",
    "r9b",
    "r10b",
    "r11b",
    "r12b",
    "r13b",
    "r14b",
    "r15b",
    "ax",
    "cx",
    "dx",
    "bx",
    "sp",
    "bp",
    "si",
    "di",
    "r8w",
    "r9w",
    "r10w",
    "r11w",
    "r12w",
    "r13w",
    "r14w",
    "r15w",
    "eax",
    "ecx",
    "edx",
    "ebx",
    "esp",
    "ebp",
    "esi",
    "edi",
    "r8d",
    "r9d",
    "r10d",
    "r11d",
    "r12d",
    "r13d",
    "r14d",
    "r15d",
    "rax",
    "rcx",
    "rdx",
    "rbx",
    "rsp",
    "rbp",
    "rsi",
    "rdi",
    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13",
    "r14",
    "r15",
    "es",
    "cs",
    "ss",
    "ds",
    "fs",
    "gs",
    "cr0",
    "cr1",
    "cr2",
    "cr3",
    "cr4",
    "cr5",
    "cr6",
    "cr7",
    "cr8",
    "cr9",
    "cr10",
    "cr11",
    "cr12",
    "cr13",
    "cr14",
    "cr15",
    "dr0",
    "dr1",
    "dr2",
    "dr3",
    "dr4",
    "dr5",
    "dr6",
    "dr7",
    "dr8",
    "dr9",
    "dr10",
    "dr11",
    "dr12",
    "dr13",
    "dr14",
    "dr15",
    "mm0",
    "mm1",
    "mm2",
    "mm3",
    "mm4",
    "mm5",
    "mm6",
    "mm7",
    "st0",
    "st1",
    "st2",
    "st3",
    "st4",
    "st5",
    "st6",
    "st7",
    "xmm0",
    "xmm1",
    "xmm2",
    "xmm3",
    "xmm4",
    "xmm5",
    "xmm6",
    "xmm7",
    "xmm8",
    "xmm9",
    "xmm10",
    "xmm11",
    "xmm12",
    "xmm13",
    "xmm14",
    "xmm15",
    "rip",
    "reg",
    "mem",
    "ptr",
    "imm",
    "jimm",
    "const"
};


char shdr_type_strings [20][16] = {
    "null",
    "progbits",
    "symtab",
    "strtab",
    "rela",
    "hash",
    "dynamic",
    "note",
    "nobits",
    "rel",
    "shlib",
    "dynsym",
    "init_array",
    "fini_array",
    "preinit_array",
    "group",
    "symtab_shndx",
    "num"
};


char sym_type_strings [16][16] = {
    "notype",    // 0
    "object",    // 1
    "func",      // 2
    "section",   // 3
    "file",      // 4
    "common",    // 5
    "tls",       // 6
    "num",       // 7
    "undefined", // 8
    "undefined", // 9
    "loos",      // 10
    "undefined", // 11
    "hios",      // 12
    "loproc",    // 13
    "undefined", // 14
    "hiproc"     // 15
};


