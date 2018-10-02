/*
Copyright 2018 <Pierre Constantineau>

3-Clause BSD License

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#ifndef KEYBOARD_CONFIG
#define KEYBOARD_CONFIG
#include "avr_mapping.h"

#define DEVICE_NAME_M                         "macro"                          /**< Name of device. Will be included in the advertising data. */

#define DEVICE_MODEL                        "macro"                          /**< Name of device. Will be included in the advertising data. */

#define MANUFACTURER_NAME                   "Ojan Thornycroft"                      /**< Manufacturer. Will be passed to Device Information Service. */


/* HARDWARE DEFINITION*/
/* key matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 4

#define MATRIX_ROW_PINS {2, 3, 4, 5, 28, 29 }
#define MATRIX_COL_PINS {16, 15, 7, 11 }
#define UNUSED_PINS {}

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION ROW2COL

#define DEBOUNCETIME 20

#define HORIZONTAL_KEYMAP( \
      k00, k01, k02, k03, k04, k05, \
      k06, k07, k08, k09, k10, k11, \
      k12, k13, k14, k15, k16, k17, \
      k18, k19, k20, k21, k22, k23 \
) \
{ \
    {k05, k11, k17, k23,}, \
    {k04, k10, k16, k22,}, \
    {k03, k09, k15, k21,}, \
    {k02, k08, k14, k20,}, \ 
    {k01, k07, k13, k19,}, \
    {k00, k06, k12, k18,} \
} 

#define VERTICAL_KEYMAP( \
        k00, k01, k02, k03, \
        k04, k05, k06, k07, \
        k08, k09, k10, k11, \
        k12, k13, k14, k15, \
        k16, k17, k18, k19, \
        k20, k21, k22, k23 \
) \
{ \
    {k00, k01, k02, k03,}, \
    {k04, k05, k06, k07,}, \
    {k08, k09, k10, k11,}, \
    {k12, k13, k14, k15,}, \
    {k16, k17, k18, k19,}, \
    {k20, k21, k22, k23,} \
} 

#endif /* KEYBOARD_CONFIG */
