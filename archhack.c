/*
 *  archhack.c
 *  dl_kextsymboltool
 *
 *  Created by Apple on 23/12/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "archhack.h"

#include <limits.h>
#include <string.h>
#include <mach-o/fat.h>
#include <mach-o/arch.h>

#ifndef CPU_TYPE_ARM
#define CPU_TYPE_ARM            ((cpu_type_t) 12)
#define CPU_SUBTYPE_ARM_V4T		((cpu_subtype_t) 5)
#define CPU_SUBTYPE_ARM_V6		((cpu_subtype_t) 6)
#endif

#ifndef CPU_SUBTYPE_ARM_V5TEJ
#define CPU_SUBTYPE_ARM_V5TEJ           ((cpu_subtype_t) 7)
#endif
#ifndef CPU_SUBTYPE_ARM_V7
#define CPU_SUBTYPE_ARM_V7		((cpu_subtype_t) 9)
#endif

#ifndef CPU_TYPE_VEO
#define CPU_TYPE_VEO            ((cpu_type_t) 255)
#define CPU_SUBTYPE_VEO_1	((cpu_subtype_t) 1)
#define CPU_SUBTYPE_VEO_2	((cpu_subtype_t) 2)
#define CPU_SUBTYPE_VEO_ALL	CPU_SUBTYPE_VEO_2
#endif

/* The array of all currently know architecture flags (terminated with an entry
 * with all zeros).  Pointer to this returned with NXGetAllArchInfos().
 */
static const NXArchInfo ArchInfoTable[] = {
    /* architecture families */
    {"hppa",   CPU_TYPE_HPPA,	 CPU_SUBTYPE_HPPA_ALL,	   NX_BigEndian,
        "HP-PA"},
    {"i386",   CPU_TYPE_I386,    CPU_SUBTYPE_I386_ALL,	   NX_LittleEndian,
        "Intel 80x86"},
    {"x86_64",   CPU_TYPE_X86_64, CPU_SUBTYPE_X86_64_ALL,  NX_LittleEndian,
        "Intel x86_64"},
    {"i860",   CPU_TYPE_I860,    CPU_SUBTYPE_I860_ALL,     NX_BigEndian,
        "Intel 860"},
    {"m68k",   CPU_TYPE_MC680x0, CPU_SUBTYPE_MC680x0_ALL,  NX_BigEndian,
        "Motorola 68K"},
    {"m88k",   CPU_TYPE_MC88000, CPU_SUBTYPE_MC88000_ALL,  NX_BigEndian,
        "Motorola 88K"},
    {"ppc",    CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_ALL,  NX_BigEndian,
        "PowerPC"},
    {"ppc64",  CPU_TYPE_POWERPC64, CPU_SUBTYPE_POWERPC_ALL,  NX_BigEndian,
        "PowerPC 64-bit"},
    {"sparc",  CPU_TYPE_SPARC,   CPU_SUBTYPE_SPARC_ALL,	   NX_BigEndian,
        "SPARC"},
    {"arm",    CPU_TYPE_ARM,     CPU_SUBTYPE_ARM_V4T,	   NX_LittleEndian,
        "arm v4t"},
    {"armv4t", CPU_TYPE_ARM,     CPU_SUBTYPE_ARM_V4T,	   NX_LittleEndian,
        "arm v4t"},
    {"armv5", CPU_TYPE_ARM,     CPU_SUBTYPE_ARM_V5TEJ,	   NX_LittleEndian,
        "arm v5"},
    {"armv6",  CPU_TYPE_ARM,     CPU_SUBTYPE_ARM_V6,	   NX_LittleEndian,
        "arm v6"},
    {"armv7",  CPU_TYPE_ARM,     CPU_SUBTYPE_ARM_V7,	   NX_LittleEndian,
        "arm v7"},
    {"any",    CPU_TYPE_ANY,     CPU_SUBTYPE_MULTIPLE,     NX_UnknownByteOrder,
        "Architecture Independent"},
    {"veo",    CPU_TYPE_VEO,	 CPU_SUBTYPE_VEO_ALL,  	   NX_BigEndian,
        "veo"},
    /* specific architecture implementations */
    {"hppa7100LC", CPU_TYPE_HPPA, CPU_SUBTYPE_HPPA_7100LC, NX_BigEndian,
        "HP-PA 7100LC"},
    {"m68030", CPU_TYPE_MC680x0, CPU_SUBTYPE_MC68030_ONLY, NX_BigEndian,
        "Motorola 68030"},
    {"m68040", CPU_TYPE_MC680x0, CPU_SUBTYPE_MC68040,	   NX_BigEndian,
        "Motorola 68040"},
    {"i486",   CPU_TYPE_I386,    CPU_SUBTYPE_486,	   NX_LittleEndian,
        "Intel 80486"},
    {"i486SX", CPU_TYPE_I386,    CPU_SUBTYPE_486SX,	   NX_LittleEndian,
        "Intel 80486SX"},
    {"pentium",CPU_TYPE_I386,    CPU_SUBTYPE_PENT,	   NX_LittleEndian,
        "Intel Pentium"}, /* same as 586 */
    {"i586",   CPU_TYPE_I386,    CPU_SUBTYPE_586,	   NX_LittleEndian,
        "Intel 80586"},
    {"pentpro", CPU_TYPE_I386, CPU_SUBTYPE_PENTPRO,	   NX_LittleEndian,
        "Intel Pentium Pro"}, /* same as 686 */
    {"i686",    CPU_TYPE_I386, CPU_SUBTYPE_PENTPRO,	   NX_LittleEndian,
        "Intel Pentium Pro"},
    {"pentIIm3", CPU_TYPE_I386, CPU_SUBTYPE_PENTII_M3, NX_LittleEndian,
        "Intel Pentium II Model 3" },
    {"pentIIm5", CPU_TYPE_I386, CPU_SUBTYPE_PENTII_M5, NX_LittleEndian,
        "Intel Pentium II Model 5" },
    {"pentium4", CPU_TYPE_I386, CPU_SUBTYPE_PENTIUM_4, NX_LittleEndian,
        "Intel Pentium 4" },
    {"ppc601", CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_601,  NX_BigEndian,
        "PowerPC 601" },
    {"ppc603", CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_603,  NX_BigEndian,
        "PowerPC 603" },
    {"ppc603e",CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_603e, NX_BigEndian,
        "PowerPC 603e" },
    {"ppc603ev",CPU_TYPE_POWERPC,CPU_SUBTYPE_POWERPC_603ev,NX_BigEndian,
        "PowerPC 603ev" },
    {"ppc604", CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_604,  NX_BigEndian,
        "PowerPC 604" },
    {"ppc604e",CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_604e, NX_BigEndian,
        "PowerPC 604e" },
    {"ppc750", CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_750,  NX_BigEndian,
        "PowerPC 750" },
    {"ppc7400",CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_7400,  NX_BigEndian,
        "PowerPC 7400" },
    {"ppc7450",CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_7450,  NX_BigEndian,
        "PowerPC 7450" },
    {"ppc970", CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_970,  NX_BigEndian,
        "PowerPC 970" },
    {"ppc970-64",  CPU_TYPE_POWERPC64, CPU_SUBTYPE_POWERPC_970,  NX_BigEndian,
        "PowerPC 970 64-bit"},
    {"little", CPU_TYPE_ANY,     CPU_SUBTYPE_LITTLE_ENDIAN, NX_LittleEndian,
        "Little Endian"},
    {"big",    CPU_TYPE_ANY,     CPU_SUBTYPE_BIG_ENDIAN,   NX_BigEndian,
        "Big Endian"},
    {"veo1",CPU_TYPE_VEO,	 CPU_SUBTYPE_VEO_1,	   NX_BigEndian,
        "veo 1" },
    {"veo2",CPU_TYPE_VEO,	 CPU_SUBTYPE_VEO_2,	   NX_BigEndian,
        "veo 2" },
    {NULL,     0,		  0,			   0,
        NULL}
};

/*
 * NXGetArchInfoFromName() is passed an architecture name (like "m68k")
 * and returns the matching NXArchInfo struct, or NULL if none is found.
 */
const
NXArchInfo *
DL_NXGetArchInfoFromName(
                      const char *name)
{
    const NXArchInfo *ai;
    
	for(ai = ArchInfoTable; ai->name != NULL; ai++)
	    if(strcmp(ai->name, name) == 0)
            return(ai);
    
	return(NULL);
}
