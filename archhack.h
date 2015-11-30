/*
 *  archhack.h
 *  dl_kextsymboltool
 *
 *  Created by Apple on 23/12/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <mach-o/arch.h>

const
NXArchInfo *
DL_NXGetArchInfoFromName(
                         const char *name);