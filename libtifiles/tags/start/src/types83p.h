/*  libtifiles - TI File Format library
 *  Copyright (C) 2002  Romain Lievin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __TIFILES_DEFS83P__
#define __TIFILES_DEFS83P__

#include <stdint.h>

#define TI83p_MAXTYPES 48

#define TI83p_REAL   0x00
#define TI83p_LIST   0x01
#define TI83p_MATRX  0x02
#define TI83p_EQU    0x03
#define TI83p_STRNG  0x04
#define TI83p_PRGM   0x05
#define TI83p_ASM    0x06
#define TI83p_PIC    0x07
#define TI83p_GDB    0x08
#define TI83p_CPLX   0x0C
#define TI83p_WDW    0x0F
#define TI83p_ZSTO   0x10
#define TI83p_TAB    0x11
#define TI83p_BKUP   0x13
#define TI83p_APPVAR 0x15
#define TI83p_GROUP  0x17
#define TI83p_DIR    0x19
#define TI83p_AMS    0x23
#define TI83p_APPL   0x24
#define TI83p_IDLIST 0x26

// libtifiles: for internal use only, not exported !

extern const char* TI83p_CONST[TI83p_MAXTYPES][3];

const char *ti83p_byte2type(uint8_t data);
uint8_t     ti83p_type2byte(const char *s);
const char *ti83p_byte2fext(uint8_t data);
uint8_t     ti83p_fext2byte(const char *s);

const char *ti83p_byte2desc(uint8_t data);

#endif

