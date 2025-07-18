/*
 *  Network Calculations Library
 *  Copyright (C) 2025 David M. Syzdek <david@syzdek.net>.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *
 *     3. Neither the name of the copyright holder nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define __LIB_LIBNETCALC_LIBNETCALC_C 1
#include "libnetcalc.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

static int
netcalc_parse_eui(
         netcalc_net_t *               n,
         char *                        address,
         int                           family );


static int
netcalc_parse_inet(
         netcalc_net_t *               n,
         char *                        address );


static int
netcalc_parse_inet6(
         netcalc_net_t *               n,
         char *                        address );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

const netcalc_addr_t _netcalc_netmasks[] =
{  {  .addr_dat8  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /0

   {  .addr_dat8  = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /1
   {  .addr_dat8  = { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /2
   {  .addr_dat8  = { 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /3
   {  .addr_dat8  = { 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /4
   {  .addr_dat8  = { 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /5
   {  .addr_dat8  = { 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /6
   {  .addr_dat8  = { 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /7
   {  .addr_dat8  = { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /8

   {  .addr_dat8  = { 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /9
   {  .addr_dat8  = { 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /10
   {  .addr_dat8  = { 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /11
   {  .addr_dat8  = { 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /12
   {  .addr_dat8  = { 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /13
   {  .addr_dat8  = { 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /14
   {  .addr_dat8  = { 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /15
   {  .addr_dat8  = { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /16

   {  .addr_dat8  = { 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /17
   {  .addr_dat8  = { 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /18
   {  .addr_dat8  = { 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /19
   {  .addr_dat8  = { 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /20
   {  .addr_dat8  = { 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /21
   {  .addr_dat8  = { 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /22
   {  .addr_dat8  = { 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /23
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /24

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /25
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /26
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /27
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /28
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /29
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /30
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /31
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /32

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /33
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /34
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /35
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /36
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /37
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /38
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /39
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /40

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /41
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /42
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /43
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /44
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /45
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /46
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /47
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /48

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /49
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /50
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /51
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /52
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /53
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /54
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /55
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /56

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /57
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /58
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /59
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /60
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /61
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /62
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /63
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /64

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /65
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /66
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /67
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /68
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /69
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /70
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /71
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /72

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /73
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /74
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /75
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /76
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /77
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /78
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /79
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /80

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /81
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /82
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /83
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /84
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /85
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /86
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /87
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /88

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /89
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /90
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /91
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /92
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /93
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /94
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /95
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /96

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00 } },  // cidr: /97
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00 } },  // cidr: /98
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00 } },  // cidr: /99
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00 } },  // cidr: /100
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00 } },  // cidr: /101
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00 } },  // cidr: /102
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00 } },  // cidr: /103
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 } },  // cidr: /104

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00 } },  // cidr: /105
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00 } },  // cidr: /106
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00 } },  // cidr: /107
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00 } },  // cidr: /108
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00 } },  // cidr: /109
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00 } },  // cidr: /110
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00 } },  // cidr: /111
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00 } },  // cidr: /112

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00 } },  // cidr: /113
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00 } },  // cidr: /114
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00 } },  // cidr: /115
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00 } },  // cidr: /116
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00 } },  // cidr: /117
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00 } },  // cidr: /118
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00 } },  // cidr: /119
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00 } },  // cidr: /120

   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80 } },  // cidr: /121
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0 } },  // cidr: /122
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0 } },  // cidr: /123
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0 } },  // cidr: /124
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8 } },  // cidr: /125
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc } },  // cidr: /126
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe } },  // cidr: /127
   {  .addr_dat8  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } },  // cidr: /128
};


// IPv4 localhost: 127.0.0.0/8
const netcalc_net_t _netcalc_lo_in =
{
   .net_flags        = NETCALC_AF_INET,
   .net_port         = 0,
   .net_cidr         = 104,
   .net_addr         = { .addr_dat8 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x01 } },
   .net_scope_name   = NULL
};


// IPv6 localhost: ::1/128
const netcalc_net_t _netcalc_lo_in6 =
{
   .net_flags        = NETCALC_AF_INET6,
   .net_port         = 0,
   .net_cidr         = 128,
   .net_addr         = { .addr_dat8 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 } },
   .net_scope_name   = NULL
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

netcalc_net_t *
netcalc_alloc(
         void )
{
   netcalc_net_t *   net;
   if ((net = malloc(sizeof(netcalc_net_t))) == NULL)
      return(NULL);
   memset(net, 0, sizeof(netcalc_net_t));
   return(net);
}


int
netcalc_dup(
         netcalc_net_t **              netp,
         const netcalc_net_t *         src )
{
   netcalc_net_t *      net;

   assert( netp != NULL );
   assert( src  != NULL );

   // allocate memory and copy data
   if ((net = malloc(sizeof(netcalc_net_t))) == NULL)
      return(NETCALC_ENOMEM);
   memcpy(net, src, sizeof(netcalc_net_t));
   net->net_scope_name = NULL;

   // allocate memory and copy net_scope
   if ( ((src->net_scope_name)) && ((src->net_scope_name[0])) )
   {
      if ((net->net_scope_name = strdup(src->net_scope_name)) == NULL)
      {
         netcalc_free(net);
         return(NETCALC_ENOMEM);
      };
   };

   *netp = net;

   return(0);
}


void
netcalc_free(
         netcalc_net_t *               net )
{
   if (!(net))
      return;
   if ((net->net_scope_name))
      free(net->net_scope_name);
   memset(net, 0, sizeof(netcalc_net_t));
   free(net);
   return;
}


int
netcalc_get_field(
         netcalc_net_t  *              net,
         int                           option,
         void *                        outvalue )
{
   int      family;
   int      val_i;
   char *   str;
   void *   ptr;
   size_t   off;

   assert( net      != NULL );
   assert( outvalue != NULL );

   family = (int)(net->net_flags & NETCALC_AF);

   switch(family)
   {  case NETCALC_AF_EUI48:  off = 10; break;
      case NETCALC_AF_EUI64:  off =  8; break;
      case NETCALC_AF_INET:   off = 12; break;
      default:                off =  0; break;
   };

   switch(option)
   {
      case NETCALC_FLD_ADDR:
         if ((ptr = malloc(sizeof(net->net_addr.netcalc_addr.netcalc_addr8))) == NULL)
            return(NETCALC_ENOMEM);
         memcpy(ptr, &net->net_addr.netcalc_addr.netcalc_addr8[off], (sizeof(netcalc_addr_t)-off) );
         *((uint8_t **)outvalue) = (uint8_t *)ptr;
         return(NETCALC_SUCCESS);

      case NETCALC_FLD_ADDRLEN:
         *((int *)outvalue) = (int)(sizeof(netcalc_addr_t)-off);
         return(NETCALC_SUCCESS);

      case NETCALC_FLD_CIDR:
         val_i = (int)net->net_cidr;
         switch(family)
         {  case  NETCALC_AF_INET:  val_i = val_i - 96; break;
            default: break;
         };
         *((int *)outvalue) = val_i;
         return(NETCALC_SUCCESS);

      case NETCALC_FLD_FAMILY:
         *((int *)outvalue) = family;
         return(NETCALC_SUCCESS);

      case NETCALC_FLD_FLAGS:
         *((int *)outvalue) = (int)net->net_flags;
         return(NETCALC_SUCCESS);

      case NETCALC_FLD_PORT:
         *((int *)outvalue) = (int)net->net_port;
         return(NETCALC_SUCCESS);

      case NETCALC_FLD_SCOPE_NAME:
         if (!(net->net_scope_name))
         {
            *((char **)outvalue) = NULL;
            return(NETCALC_SUCCESS);
         };
         if ((str = strdup(net->net_scope_name)) == NULL)
            return(NETCALC_ENOMEM);
         *((char **)outvalue) = str;
         return(NETCALC_SUCCESS);

      default:
         break;
   };

   return(NETCALC_EFIELD);
}


int
netcalc_initialize(
         netcalc_net_t **              netp,
         const char *                  address,
         int                           flags )
{
   size_t               addrlen;
   char *               str;
   int                  rc;
   char                 sbuff[NETCALC_ADDRESS_LENGTH];
   char                 scope_name[NETCALC_SCOPE_NAME_LENGTH];
   netcalc_net_t        nbuff;

   assert(address != NULL);

   // check flags
   if ((flags & NETCALC_FLG))
      return(NETCALC_EINVAL);
   flags |= ((flags & NETCALC_AF)) ? 0 : NETCALC_AF;

   // prepare netcalc_net_t buffer
   memset(&nbuff,       0, sizeof(netcalc_net_t));
   memset(&scope_name,  0, sizeof(scope_name));
   nbuff.net_scope_name = scope_name;
   nbuff.net_flags      = ((flags & NETCALC_AF)) ? flags : (flags | NETCALC_AF);
   nbuff.net_cidr       = 128;

   // prepare string buffer
   addrlen = strlen(address);
   if (NETCALC_ADDRESS_LENGTH <= (addrlen+1))
      return(NETCALC_EBUFFLEN);
   memcpy(sbuff, address, addrlen);
   sbuff[addrlen] = '\0';
   str            = sbuff;

   // initial address checks
   //    EUI48:   xx-xx-xx-xx-xx-xx
   //             xx:xx:xx:xx:xx:xx
   //             xxxx.xxxx.xxxx
   //             xxxxxxxxxxxx
   //    EUI48:   xx-xx-xx-xx-xx-xx-xx-xx
   //             xx:xx:xx:xx:xx:xx:xx:xx
   //             xxxx.xxxx.xxxx.xxxx
   //             xxxxxxxxxxxxxxxx
   //    INET:    ddd.ddd.ddd.ddd
   //             ddd.ddd.ddd.ddd/cc
   //             ddd.ddd.ddd.ddd:ppppp
   //    INET6:   ::
   //             ::1
   //             [xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx%iface]:ppppp
   //             [xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx]:ppppp
   //             [xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:ddd.ddd.ddd.ddd]:ppppp
   //             xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx%iface
   //             xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx/ccc%iface
   //             xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:ddd.ddd.ddd.ddd/ccc%iface
   //             xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx/ccc
   //             xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:ddd.ddd.ddd.ddd/ccc
   //             xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:ddd.ddd.ddd.ddd
   // where
   //    * 'ddd' can be a decimal value betweem 0 and 255.
   //    * 'ppppp' can be a decimal value between 0 and 65535
   //    * IPv6 zero compression is supported, but not displayed.
   //
   if ( (addrlen != 12) && (addrlen != 14) && (addrlen != 17) )
      nbuff.net_flags &= ~NETCALC_AF_EUI48;
   if ( (addrlen != 16) && (addrlen != 23) && (addrlen != 19) )
      nbuff.net_flags &= ~NETCALC_AF_EUI64;
   if ( (addrlen < 7) || (addrlen > 21) )
      nbuff.net_flags &= ~NETCALC_AF_INET;
   if (addrlen < 2)
      nbuff.net_flags &= ~NETCALC_AF_INET6;
   if (!(nbuff.net_flags & NETCALC_AF))
      return(NETCALC_EBADADDR);

   rc = NETCALC_SUCCESS;
   if ((nbuff.net_flags & NETCALC_AF_EUI48))
   {
      nbuff.net_flags   = ((rc = netcalc_parse_eui(&nbuff, str, NETCALC_AF_EUI48)) == NETCALC_SUCCESS)
                        ? (nbuff.net_flags & ~NETCALC_AF) | NETCALC_AF_EUI48
                        : (nbuff.net_flags & ~NETCALC_AF_EUI48);
   };
   if ((nbuff.net_flags & NETCALC_AF_EUI64))
   {
      nbuff.net_flags   = ((rc = netcalc_parse_eui(&nbuff, str, NETCALC_AF_EUI64)) == NETCALC_SUCCESS)
                        ? (nbuff.net_flags & ~NETCALC_AF) | NETCALC_AF_EUI64
                        : (nbuff.net_flags & ~NETCALC_AF_EUI64);
   };
   if ((nbuff.net_flags & NETCALC_AF_INET))
   {
      nbuff.net_flags   = ((rc = netcalc_parse_inet(&nbuff, str)) == NETCALC_SUCCESS)
                        ? (nbuff.net_flags & ~NETCALC_AF) | NETCALC_AF_INET
                        : (nbuff.net_flags & ~NETCALC_AF_INET);
   };
   if ((nbuff.net_flags & NETCALC_AF_INET6))
   {
      nbuff.net_flags   = ((rc = netcalc_parse_inet6(&nbuff, str)) == NETCALC_SUCCESS)
                        ? (nbuff.net_flags & ~NETCALC_AF) | NETCALC_AF_INET6
                        : (nbuff.net_flags & ~NETCALC_AF_INET6 );
   };
   if ((rc))
      return(rc);
   switch(nbuff.net_flags & NETCALC_AF)
   {
      case NETCALC_AF_EUI48:  break;
      case NETCALC_AF_EUI64:  break;
      case NETCALC_AF_INET:   break;
      case NETCALC_AF_INET6:  break;
      default:                return(NETCALC_EBADADDR);
   };

   return((netp == NULL) ? 0 : netcalc_dup(netp, &nbuff));
}


int
netcalc_parse_eui(
         netcalc_net_t *               n,
         char *                        address,
         int                           family )
{
   size_t            pos;
   size_t            digit;
   size_t            byte;
   size_t            byte_max;
   size_t            off;
   unsigned          hex;
   uint8_t *         addr8;
   char              delim;
   char              str[NETCALC_ADDRESS_LENGTH];
   netcalc_addr_t    net_addr;

   assert(n       != NULL);
   assert(address != NULL);

   switch(family)
   {
      case NETCALC_AF_EUI48: off = 10; byte_max = 5; break;
      case NETCALC_AF_EUI64: off =  8; byte_max = 7; break;
      default: return(NETCALC_EBADADDR);
   }

   delim = 0;
   addr8 = net_addr.netcalc_addr.netcalc_addr8;
   strncpy(str, address, sizeof(str));

   for(pos = 0, digit = 0, byte = 0, hex = 0; ((str[pos])); pos++)
   {
      switch(str[pos])
      {
         case '0': hex = (hex << 4) +  0; digit++; break;
         case '1': hex = (hex << 4) +  1; digit++; break;
         case '2': hex = (hex << 4) +  2; digit++; break;
         case '3': hex = (hex << 4) +  3; digit++; break;
         case '4': hex = (hex << 4) +  4; digit++; break;
         case '5': hex = (hex << 4) +  5; digit++; break;
         case '6': hex = (hex << 4) +  6; digit++; break;
         case '7': hex = (hex << 4) +  7; digit++; break;
         case '8': hex = (hex << 4) +  8; digit++; break;
         case '9': hex = (hex << 4) +  9; digit++; break;

         case 'a': hex = (hex << 4) + 10; digit++; break;
         case 'b': hex = (hex << 4) + 11; digit++; break;
         case 'c': hex = (hex << 4) + 12; digit++; break;
         case 'd': hex = (hex << 4) + 13; digit++; break;
         case 'e': hex = (hex << 4) + 14; digit++; break;
         case 'f': hex = (hex << 4) + 15; digit++; break;

         case 'A': hex = (hex << 4) + 10; digit++; break;
         case 'B': hex = (hex << 4) + 11; digit++; break;
         case 'C': hex = (hex << 4) + 12; digit++; break;
         case 'D': hex = (hex << 4) + 13; digit++; break;
         case 'E': hex = (hex << 4) + 14; digit++; break;
         case 'F': hex = (hex << 4) + 15; digit++; break;

         case '.':
            if (delim == 0)
               delim = '.';
            if ( (digit != 4) || (delim != '.') )
               return(NETCALC_EBADADDR);
            addr8[off+0+byte]  = ((hex >> 8) & 0xff);
            addr8[off+1+byte]  = ((hex >> 0) & 0xff);
            hex                = 0;
            digit              = 0;
            byte              += 2;
            break;

         case '-':
         case ':':
            if (delim == 0)
               delim = str[pos];
            if ( (digit != 2) || (delim != str[pos]) )
               return(NETCALC_EBADADDR);
            addr8[off+byte]   = hex;
            hex               = 0;
            digit             = 0;
            byte++;
            break;

         default:
            return(NETCALC_EBADADDR);
      };

      if ( (digit > 2) && ((delim == ':') || (delim == '-')) )
         return(NETCALC_EBADADDR);
      if (digit > 4)
      {
         if (delim == 0)
            delim = 1;
         if (delim != 1)
            return(NETCALC_EBADADDR);
         addr8[off+0+byte]  = ((hex >>  12) & 0xff);
         addr8[off+1+byte]  = ((hex >>  4) & 0xff);
         hex                = hex & 0x0f;
         digit              = 1;
         byte              += 2;
      };
      if (byte > byte_max)
         return(NETCALC_EBADADDR);
   };

   switch(delim)
   {
      case ':':
      case '-':
         if (digit != 2)
            return(NETCALC_EBADADDR);
         addr8[off+byte] = hex;
         break;

      case 1:
      case '.':
         if (digit != 4)
            return(NETCALC_EBADADDR);
         addr8[off+0+byte] = ((hex >> 8) & 0xff);
         addr8[off+1+byte] = ((hex >> 0) & 0xff);
         break;

      default:
         return(NETCALC_EBADADDR);
   };

   memcpy(&n->net_addr, &net_addr,  sizeof(net_addr));
   n->net_cidr = 0;
   n->net_port = 0;

   return(0);
}


int
netcalc_parse_inet(
         netcalc_net_t *               n,
         char *                        address )
{
   size_t            pos;
   size_t            digit;
   size_t            byte;
   int               cidr;
   int               port;
   unsigned          dec;
   uint8_t *         addr8;
   char *            ptr;
   netcalc_addr_t    net_addr;
   char              str[NETCALC_ADDRESS_LENGTH];

   assert(n       != NULL);
   assert(address != NULL);

   addr8 = net_addr.netcalc_addr.netcalc_addr8;
   cidr  = -1;
   port  = -1;
   strncpy(str, address, sizeof(str));

   // check for port
   if ((ptr = strchr(str, ':')) != NULL)
   {
      ptr[0] = '\0';
      if (ptr[1] == '\0')
         return(NETCALC_EBADADDR);
      port = (int)strtoul(&ptr[1], &ptr, 10);
      if ( (ptr[0] != '\0') || (port > 0xffff) )
         return(NETCALC_EBADADDR);
   };

   // check for CIDR
   if ((ptr = strchr(str, '/')) != NULL)
   {
      ptr[0] = '\0';
      if (ptr[1] == '\0')
         return(NETCALC_EBADADDR);
      cidr = (int)strtoul(&ptr[1], &ptr, 10);
      if ( (ptr[0] != '\0') || (cidr > 32) )
         return(NETCALC_EBADADDR);
      cidr += 96;
   };

   for(pos = 0, digit = 0, byte = 0, dec = 0; ((str[pos])); pos++)
   {
      switch(str[pos])
      {
         case '0': dec = (dec * 10) + 0; digit++; break;
         case '1': dec = (dec * 10) + 1; digit++; break;
         case '2': dec = (dec * 10) + 2; digit++; break;
         case '3': dec = (dec * 10) + 3; digit++; break;
         case '4': dec = (dec * 10) + 4; digit++; break;
         case '5': dec = (dec * 10) + 5; digit++; break;
         case '6': dec = (dec * 10) + 6; digit++; break;
         case '7': dec = (dec * 10) + 7; digit++; break;
         case '8': dec = (dec * 10) + 8; digit++; break;
         case '9': dec = (dec * 10) + 9; digit++; break;

         case '.':
         if (digit == 0)
            return(NETCALC_EBADADDR);
         addr8[15-byte] = dec;
         dec            = 0;
         digit          = 0;
         byte++;
         break;

         default:
         return(NETCALC_EBADADDR);
      };

      if ( (digit > 3) || (dec > 255) )
         return(NETCALC_EBADADDR);
   };

   if ( (byte != 3) || (digit == 0) )
      return(NETCALC_EBADADDR);

   addr8[12+byte] = dec;

   memcpy(&n->net_addr.netcalc_addr.netcalc_addr8[15], &net_addr.netcalc_addr.netcalc_addr8[15], sizeof(net_addr)-16);
   n->net_cidr = (uint8_t)((cidr != -1) ? cidr : n->net_cidr);
   n->net_port = (uint16_t)((port != -1) ? port : n->net_port);

   return(0);
}


int
netcalc_parse_inet6(
         netcalc_net_t *               n,
         char *                        address )
{
   size_t            pos;
   size_t            fwd;
   size_t            digit;
   size_t            wyde;
   size_t            wyde_fwd;
   unsigned          hex;
   unsigned          dec;
   char *            tmp;
   char *            str;
   char *            ptr;
   uint8_t *         addr8;
   int               zero;
   int               port;
   int               cidr;
   int               bracketed;
   netcalc_addr_t    net_addr;
   char              scope_name[NETCALC_SCOPE_NAME_LENGTH];
   char              sbuff[NETCALC_ADDRESS_LENGTH];

   assert(n       != NULL);
   assert(address != NULL);

   if (!(n->net_flags & NETCALC_AF_INET6))
      return(NETCALC_EBADADDR);

   zero        = 0;
   addr8       = net_addr.netcalc_addr.netcalc_addr8;
   str         = sbuff;
   port        = -1;
   cidr        = -1;
   bracketed   = 0;
   strncpy(sbuff, address, sizeof(sbuff));

   // check for bracketed address
   if (str[0] == '[')
   {
      bracketed   = 1;

      // adjust start of string
      str       = &str[1];

      if ((ptr = strchr(str, ']')) == NULL)
         return(NETCALC_EBADADDR);
      ptr[0]   = '\0';
      tmp      = &ptr[1];

      // check for port after bracketed address
      if ((ptr = strchr(tmp, ':')) != NULL)
      {
         ptr[0] = '\0';
         if (ptr[1] == '\0')
            return(NETCALC_EBADADDR);
         port = (int)strtoul(&ptr[1], &ptr, 10);
         if ( (ptr[0] != '\0') || (port > 0xffff) )
            return(NETCALC_EBADADDR);
      };

      // check for CIDR after bracketed address
      if ((ptr = strchr(tmp, '/')) != NULL)
      {
         ptr[0] = '\0';
         if (ptr[1] == '\0')
            return(NETCALC_EBADADDR);
         cidr = (int)strtoul(&ptr[1], &ptr, 10);
         if ( (ptr[0] != '\0') || (cidr > 128) )
            return(NETCALC_EBADADDR);
      };

      if (tmp[0] != '\0')
         return(NETCALC_EBADADDR);
   };

   // check for named scope
   if ((ptr = strchr(str, '%')) != NULL)
   {
      ptr[0] = '\0';
      if (ptr[1] == '\0')
         return(NETCALC_EBADADDR);
      for(pos = 1; ((ptr[pos])); pos++)
      {
         if (pos >= NETCALC_SCOPE_NAME_LENGTH)
            return(NETCALC_EBUFFLEN);
         if       ( (ptr[pos] >= '0') && (ptr[pos] <= '9') ) scope_name[pos-1] = ptr[pos];
         else if  ( (ptr[pos] >= 'a') && (ptr[pos] <= 'z') ) scope_name[pos-1] = ptr[pos];
         else if  ( (ptr[pos] >= 'A') && (ptr[pos] <= 'Z') ) scope_name[pos-1] = ptr[pos];
         else
         {
            switch(ptr[pos])
            {
               case '.':
               case '-':
               case '_':
               case ':':
                  scope_name[pos-1] = ptr[pos];
                  break;

               default:
                  return(NETCALC_EBADADDR);
            }
         }
      };
      scope_name[pos-1] = '\0';
   };

   // check for CIDR
   if ((ptr = strchr(str, '/')) != NULL)
   {
      if ( (!(bracketed)) && (scope_name[0] != '\0') )
         return(NETCALC_EBADADDR);
      ptr[0] = '\0';
      if (ptr[1] == '\0')
         return(NETCALC_EBADADDR);
      cidr = (int)strtoul(&ptr[1], &ptr, 10);
      if ( (ptr[0] != '\0') || (cidr > 128) )
         return(NETCALC_EBADADDR);
   };

   for(pos = 0, digit = 0, wyde = 0, dec = 0, hex = 0; ((str[pos])); pos++)
   {
      switch(str[pos])
      {
         case '0': hex = (hex << 4) +  0; digit++; break;
         case '1': hex = (hex << 4) +  1; digit++; break;
         case '2': hex = (hex << 4) +  2; digit++; break;
         case '3': hex = (hex << 4) +  3; digit++; break;
         case '4': hex = (hex << 4) +  4; digit++; break;
         case '5': hex = (hex << 4) +  5; digit++; break;
         case '6': hex = (hex << 4) +  6; digit++; break;
         case '7': hex = (hex << 4) +  7; digit++; break;
         case '8': hex = (hex << 4) +  8; digit++; break;
         case '9': hex = (hex << 4) +  9; digit++; break;

         case 'a': hex = (hex << 4) + 10; digit++; break;
         case 'b': hex = (hex << 4) + 11; digit++; break;
         case 'c': hex = (hex << 4) + 12; digit++; break;
         case 'd': hex = (hex << 4) + 13; digit++; break;
         case 'e': hex = (hex << 4) + 14; digit++; break;
         case 'f': hex = (hex << 4) + 15; digit++; break;

         case 'A': hex = (hex << 4) + 10; digit++; break;
         case 'B': hex = (hex << 4) + 11; digit++; break;
         case 'C': hex = (hex << 4) + 12; digit++; break;
         case 'D': hex = (hex << 4) + 13; digit++; break;
         case 'E': hex = (hex << 4) + 14; digit++; break;
         case 'F': hex = (hex << 4) + 15; digit++; break;

         case ':':
         addr8[(wyde*2)+0] = (hex >> 16) & 0xff;
         addr8[(wyde*2)+1] = (hex >>  0) & 0xff;
         if (str[pos+1] == ':')
         {
            if (str[pos+2] == ':')
               return(NETCALC_EBADADDR);
            if ((zero))
               return(NETCALC_EBADADDR);
            zero++;
            pos += 2;
            for(fwd = pos, wyde_fwd = 1; ((str[fwd])); fwd++)
            {
               if (str[fwd] == ':')
               {
                  wyde_fwd++;
                  digit = 0;
               }
               else if (str[fwd] == '.')
                  dec++;
               else if  (  ( (str[fwd] >= '0') && (str[fwd] <= '9') ) ||
                           ( (str[fwd] >= 'a') && (str[fwd] <= 'f') ) ||
                           ( (str[fwd] >= 'A') && (str[fwd] <= 'F') )
                        )
                  continue;
               else
                  return(NETCALC_EBADADDR);
            };
            if ( (dec != 0) && (dec != 3) )
               return(NETCALC_EBADADDR);
            if ((wyde_fwd + wyde) > 7)
               return(NETCALC_EBADADDR);
            wyde     += 7 - (wyde_fwd + wyde);
            if (dec == 3)
               wyde--;
         };
         hex   = 0;
         digit = 0;
         wyde++;
         break;

         default:
         return(NETCALC_EBADADDR);
      };

      // check for IPv4 mapped address
      if (wyde == 6)
      {
         if (!(netcalc_parse_inet(n, &str[pos])))
         {
            n->net_flags |= NETCALC_FLG_V4MAPPED;
            return(0);
         };
      };

      if ( (wyde > 7) || (digit > 4) || (hex > 0xffff) )
         return(NETCALC_EBADADDR);
   };

   if (wyde != 7)
      return(NETCALC_EBADADDR);

   addr8[(wyde*2)+0] = (hex >> 16) & 0xff;
   addr8[(wyde*2)+1] = (hex >>  0) & 0xff;

   memcpy(&n->net_addr,       &net_addr,  sizeof(net_addr));
   memcpy(n->net_scope_name,  scope_name, sizeof(scope_name));
   n->net_cidr =  (uint8_t)((cidr != -1) ? cidr : n->net_cidr);
   n->net_port = (uint16_t)((port != -1) ? port : n->net_port);

   return(0);
}


const char *
netcalc_strerror(
         int                           errnum )
{
   switch(errnum)
   {
      case NETCALC_SUCCESS:      return("success");

      case NETCALC_EBADADDR:     return("bad address string");
      case NETCALC_EBUFFLEN:     return("buffer length exceeeded");
      case NETCALC_EFIELD:       return("unknown or unsupported field");
      case NETCALC_EINVAL:       return("invalid argument");
      case NETCALC_ENOMEM:       return("out of virtual memory");

      case NETCALC_EUNKNOWN:     break;
      default:                   break;
   };
   return("unknown error");
}


/* end of source */
