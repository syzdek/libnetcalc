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


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros

#define NETCALC_DFLT_FLAG(net, dflts, flgs) (((net&flgs)) ? (net&flgs) : (dflts&flgs))


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
netcalc_convert_eui48(
         netcalc_net_t *               net );


static int
netcalc_convert_eui64(
         netcalc_net_t *               net );


static int
netcalc_convert_inet(
         netcalc_net_t *               net );


static int
netcalc_convert_inet6(
         netcalc_net_t *               net,
         const netcalc_net_t *         prefix );


static const char *
netcalc_ntop_eui(
         netcalc_net_t *               net,
         char *                        dst,
         size_t                        size,
         int                           flags );


static const char *
netcalc_ntop_inet(
         netcalc_net_t *               net,
         char *                        dst,
         size_t                        size,
         int                           flags );


static const char *
netcalc_ntop_inet6(
         netcalc_net_t *               net,
         char *                        dst,
         size_t                        size,
         int                           flags );


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


extern size_t
netcalc_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

const netcalc_addr_t _netcalc_netmasks[] =
{  {  .addr8   = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /0

   {  .addr8   = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /1
   {  .addr8   = { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /2
   {  .addr8   = { 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /3
   {  .addr8   = { 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /4
   {  .addr8   = { 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /5
   {  .addr8   = { 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /6
   {  .addr8   = { 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /7
   {  .addr8   = { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /8

   {  .addr8   = { 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /9
   {  .addr8   = { 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /10
   {  .addr8   = { 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /11
   {  .addr8   = { 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /12
   {  .addr8   = { 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /13
   {  .addr8   = { 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /14
   {  .addr8   = { 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /15
   {  .addr8   = { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /16

   {  .addr8   = { 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /17
   {  .addr8   = { 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /18
   {  .addr8   = { 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /19
   {  .addr8   = { 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /20
   {  .addr8   = { 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /21
   {  .addr8   = { 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /22
   {  .addr8   = { 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /23
   {  .addr8   = { 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /24

   {  .addr8   = { 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /25
   {  .addr8   = { 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /26
   {  .addr8   = { 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /27
   {  .addr8   = { 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /28
   {  .addr8   = { 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /29
   {  .addr8   = { 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /30
   {  .addr8   = { 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /31
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /32

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /33
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /34
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /35
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /36
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /37
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /38
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /39
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /40

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /41
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /42
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /43
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /44
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /45
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /46
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /47
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /48

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /49
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /50
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /51
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /52
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /53
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /54
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /55
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /56

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /57
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /58
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /59
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /60
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /61
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /62
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /63
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /64

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /65
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /66
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /67
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /68
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /69
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /70
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /71
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /72

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /73
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /74
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /75
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /76
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /77
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /78
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /79
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /80

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /81
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /82
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /83
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /84
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /85
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /86
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /87
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /88

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /89
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /90
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /91
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /92
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /93
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /94
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /95
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 } },  // cidr: /96

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00 } },  // cidr: /97
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00 } },  // cidr: /98
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00 } },  // cidr: /99
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00 } },  // cidr: /100
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00 } },  // cidr: /101
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00 } },  // cidr: /102
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00 } },  // cidr: /103
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 } },  // cidr: /104

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00 } },  // cidr: /105
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00 } },  // cidr: /106
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00 } },  // cidr: /107
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00 } },  // cidr: /108
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00 } },  // cidr: /109
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00 } },  // cidr: /110
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00 } },  // cidr: /111
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00 } },  // cidr: /112

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00 } },  // cidr: /113
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00 } },  // cidr: /114
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00 } },  // cidr: /115
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00 } },  // cidr: /116
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00 } },  // cidr: /117
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00 } },  // cidr: /118
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00 } },  // cidr: /119
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00 } },  // cidr: /120

   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80 } },  // cidr: /121
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0 } },  // cidr: /122
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0 } },  // cidr: /123
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0 } },  // cidr: /124
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8 } },  // cidr: /125
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc } },  // cidr: /126
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe } },  // cidr: /127
   {  .addr8   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } },  // cidr: /128
};


// IPv6 Prefix for IPv4 addresses: ::ffff:0000:0000/96
const netcalc_net_t _netcalc_ipv4_mapped_ipv6  =
{
   .net_flags        = NETCALC_AF_INET6,
   .net_cidr         = 96,
   .net_addr         = { .addr8 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 } },
};


// IPv4 Link-local addresses: 169.254.0.0/16
const netcalc_net_t _netcalc_link_local_in =
{
   .net_flags        = NETCALC_AF_INET,
   .net_cidr         = 112,
   .net_addr         = { .addr8 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0xfe, 0x00, 0x00 } },
};


// IPv6 Link-local addresses: fe80::/10
const netcalc_net_t _netcalc_link_local_in6 =
{
   .net_flags        = NETCALC_AF_INET6,
   .net_cidr         = 10,
   .net_addr         = { .addr8 = { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
};


// IPv4 localhost: 127.0.0.0/8
const netcalc_net_t _netcalc_lo_in =
{
   .net_flags        = NETCALC_AF_INET,
   .net_port         = 0,
   .net_cidr         = 104,
   .net_addr         = { .addr8 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x01 } },
   .net_scope_name   = NULL
};


// IPv6 localhost: ::1/128
const netcalc_net_t _netcalc_lo_in6 =
{
   .net_flags        = NETCALC_AF_INET6,
   .net_port         = 0,
   .net_cidr         = 128,
   .net_addr         = { .addr8 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 } },
   .net_scope_name   = NULL
};


// IPv6 SLAAC addresses: fe80::/64
const netcalc_net_t _netcalc_slaac_in6 =
{
   .net_flags        = NETCALC_AF_INET6,
   .net_cidr         = 64,
   .net_addr         = { .addr8 = { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
};



/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
netcalc_convert(
         netcalc_net_t *               net,
         int                           family,
         const netcalc_net_t *         prefix )
{
   assert(net != NULL);
   switch(family)
   {  case NETCALC_AF_EUI48:  return(netcalc_convert_eui48(net));
      case NETCALC_AF_EUI64:  return(netcalc_convert_eui64(net));
      case NETCALC_AF_INET:   return(netcalc_convert_inet(net));
      case NETCALC_AF_INET6:  return(netcalc_convert_inet6(net, prefix));
      default:                break;
   };
   return(NETCALC_ENOTSUP);
}


int
netcalc_convert_eui48(
         netcalc_net_t *               net  )
{
   uint8_t *      addr8;

   assert(net != NULL);

   addr8 = (uint8_t *)&net->net_addr.addr8;

   switch(net->net_flags & NETCALC_AF)
   {  case NETCALC_AF_EUI48:
         return(0);

      case NETCALC_AF_INET6:
      case NETCALC_AF_EUI64:
         if ( (addr8[11] != 0xff) || (addr8[12] != 0xfe) )
            return(NETCALC_ENOTSUP);
         net->net_addr.addr8[12] = net->net_addr.addr8[10];
         net->net_addr.addr8[11] = net->net_addr.addr8[9];
         net->net_addr.addr8[10] = (net->net_addr.addr8[8] ^ 0x02);
         net->net_addr.addr8[9]  = 0;
         net->net_addr.addr8[8]  = 0;
         net->net_addr.addr32[0] = 0;
         net->net_addr.addr32[1] = 0;
         net->net_flags = (net->net_flags & ~NETCALC_AF) | NETCALC_AF_EUI48;
         return(0);

      case NETCALC_AF_INET:
         return(NETCALC_ENOTSUP);

      default:
         break;
   };

   return(NETCALC_ENOTSUP);
}


int
netcalc_convert_eui64(
         netcalc_net_t *               net )
{
   assert(net != NULL);
   switch(net->net_flags & NETCALC_AF)
   {  case NETCALC_AF_EUI48:
         net->net_addr.addr32[0] = 0;
         net->net_addr.addr32[1] = 0;
         net->net_addr.addr8[8]  = (net->net_addr.addr8[10] ^ 0x02);
         net->net_addr.addr8[9]  = net->net_addr.addr8[11];
         net->net_addr.addr8[10] = net->net_addr.addr8[12];
         net->net_addr.addr8[11] = 0xff;
         net->net_addr.addr8[12] = 0xfe;
         net->net_flags = (net->net_flags & ~NETCALC_AF) | NETCALC_AF_EUI64;
         return(0);

      case NETCALC_AF_EUI64:
         return(0);

      case NETCALC_AF_INET:
         return(NETCALC_ENOTSUP);

      case NETCALC_AF_INET6:
         net->net_addr.addr32[0] = 0;
         net->net_addr.addr32[1] = 0;
         net->net_flags = (net->net_flags & ~NETCALC_AF) | NETCALC_AF_EUI64;
         return(0);

      default:
         break;
   };
   return(NETCALC_ENOTSUP);
}


int
netcalc_convert_inet(
         netcalc_net_t *               net )
{
   assert(net != NULL);
   switch(net->net_flags & NETCALC_AF)
   {  case NETCALC_AF_EUI48:
         return(NETCALC_ENOTSUP);

      case NETCALC_AF_EUI64:
         return(NETCALC_ENOTSUP);

      case NETCALC_AF_INET:
         return(0);

      case NETCALC_AF_INET6:
         net->net_addr.addr32[0] = 0;
         net->net_addr.addr32[1] = 0;
         net->net_addr.addr32[2] = 0;
         net->net_flags = (net->net_flags & ~NETCALC_AF) | NETCALC_AF_INET;
         return(0);

      default:
         break;
   };
   return(NETCALC_ENOTSUP);
}


int
netcalc_convert_inet6(
         netcalc_net_t *               net,
         const netcalc_net_t *         prefix )
{
   int      rc;

   assert(net != NULL);

   switch(net->net_flags & NETCALC_AF)
   {  case NETCALC_AF_EUI48:
         if ((rc = netcalc_convert_eui64(net)) != NETCALC_SUCCESS)
            return(NETCALC_ENOTSUP);

      case NETCALC_AF_EUI64:
         net->net_flags = (net->net_flags & ~NETCALC_AF) | NETCALC_AF_INET6;
         prefix         = ((prefix)) ? prefix : &_netcalc_slaac_in6;
         return(netcalc_network_mask(net, prefix, net->net_cidr));

      case NETCALC_AF_INET:
         net->net_flags = (net->net_flags & ~NETCALC_AF) | NETCALC_AF_INET6;
         prefix         = ((prefix)) ? prefix : &_netcalc_ipv4_mapped_ipv6;
         return(netcalc_network_mask(net, prefix, net->net_cidr));

      case NETCALC_AF_INET6:
         return(0);

      default:
         break;
   };
   return(NETCALC_ENOTSUP);
}


int
netcalc_dflt_flags(
         int                           family )
{
   family &= NETCALC_AF;
   switch(family)
   {  case NETCALC_AF_EUI48: return(NETCALC_DFLT_EUI48);
      case NETCALC_AF_EUI64: return(NETCALC_DFLT_EUI64);
      case NETCALC_AF_INET:  return(NETCALC_DFLT_INET);
      case NETCALC_AF_INET6: return(NETCALC_DFLT_INET6);
      default: break;
   };
   return(0);
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

   assert( net      != NULL );
   assert( outvalue != NULL );

   family = (int)(net->net_flags & NETCALC_AF);

   switch(option)
   {
      case NETCALC_FLD_ADDR:
         if ((ptr = malloc(sizeof(net->net_addr.addr8))) == NULL)
            return(NETCALC_ENOMEM);
         memcpy(ptr, &net->net_addr, sizeof(netcalc_addr_t) );
         *((uint8_t **)outvalue) = (uint8_t *)ptr;
         return(NETCALC_SUCCESS);

      case NETCALC_FLD_ADDRLEN:
         *((int *)outvalue) = (int)sizeof(netcalc_addr_t);
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

      case NETCALC_FLD_IG:
      case NETCALC_FLD_IG_BIT:
         switch(net->net_flags & NETCALC_AF)
         {  case NETCALC_AF_EUI48: val_i = net->net_addr.addr8[10] & 0x01; break;
            case NETCALC_AF_EUI64: val_i = net->net_addr.addr8[8]  & 0x01; break;
            case NETCALC_AF_INET6: val_i = net->net_addr.addr8[8]  & 0x01; break;
            default: return(NETCALC_ENOTSUP);
         };
         *((int *)outvalue) = val_i;
         return(NETCALC_SUCCESS);

      case NETCALC_FLD_IPV4_MAPPED:
         *((int *)outvalue) = 0;
         if ((net->net_flags & NETCALC_AF) == NETCALC_AF_INET6)
         {  if (net->net_addr.addr32[2] == 0)
               *((int *)outvalue) = 1;
            if (net->net_addr.addr32[2] == 0xffffffff)
               *((int *)outvalue) = 1;
            if ((net->net_flags & NETCALC_FLG_V4MAPPED))
               *((int *)outvalue) = 1;
         };
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

      case NETCALC_FLD_UL:
         switch(net->net_flags & NETCALC_AF)
         {  case NETCALC_AF_EUI48: val_i = (net->net_addr.addr8[10] & 0x02) >> 1;         break;
            case NETCALC_AF_EUI64: val_i = ((net->net_addr.addr8[8] & 0x02) >> 1) ^ 0x01; break;
            case NETCALC_AF_INET6: val_i = ((net->net_addr.addr8[8] & 0x02) >> 1) ^ 0x01; break;
            default: return(NETCALC_ENOTSUP);
         };
         *((int *)outvalue) = val_i;
         return(NETCALC_SUCCESS);

      case NETCALC_FLD_UL_BIT:
         switch(net->net_flags & NETCALC_AF)
         {  case NETCALC_AF_EUI48: val_i = (net->net_addr.addr8[10] & 0x02) >> 1; break;
            case NETCALC_AF_EUI64: val_i = (net->net_addr.addr8[8] & 0x02)  >> 1; break;
            case NETCALC_AF_INET6: val_i = (net->net_addr.addr8[8] & 0x02)  >> 1; break;
            default: return(NETCALC_ENOTSUP);
         };
         *((int *)outvalue) = val_i;
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
netcalc_network_mask(
         netcalc_net_t *               net,
         const netcalc_net_t *         prefix,
         int                           cidr )
{
   int                        family;
   netcalc_addr_t *           net_addr;
   const netcalc_addr_t *     mask_addr;
   const netcalc_addr_t *     pre_addr;

   assert(net != NULL);

   // verifies prefix and net are of the same address family
   family = net->net_flags & NETCALC_AF;
   if ( (family != NETCALC_AF_INET) && (family != NETCALC_AF_INET6) )
      return(NETCALC_ENOTSUP);

   // normalizes CIDR
   if ( ((cidr)) && (family == NETCALC_AF_INET) )
      cidr += 96;
   if (cidr > 128)
      return(NETCALC_EINVAL);

   // updates CIDR and exits if prefix is not specified
   if (!(prefix))
   {  if (cidr >= 0)
         net->net_cidr = cidr;
      return(0);
   };
   net->net_cidr = ((cidr > 0) && (cidr < 129)) ? cidr : prefix->net_cidr;

   net_addr    = &net->net_addr;
   pre_addr    = &prefix->net_addr;
   mask_addr   = &_netcalc_netmasks[prefix->net_cidr];

   // copy network prefix to network
   net_addr->addr32[0]  = (  mask_addr->addr32[0] & pre_addr->addr32[0] )
                        | ( ~mask_addr->addr32[0] & net_addr->addr32[0] );
   net_addr->addr32[1]  = (  mask_addr->addr32[1] & pre_addr->addr32[1] )
                        | ( ~mask_addr->addr32[1] & net_addr->addr32[1] );
   net_addr->addr32[2]  = (  mask_addr->addr32[2] & pre_addr->addr32[2] )
                        | ( ~mask_addr->addr32[2] & net_addr->addr32[2] );
   net_addr->addr32[3]  = (  mask_addr->addr32[3] & pre_addr->addr32[3] )
                        | ( ~mask_addr->addr32[3] & net_addr->addr32[3] );

   return(0);
}


const char *
netcalc_ntop(
         netcalc_net_t *               net,
         char *                        dst,
         size_t                        size,
         int                           type,
         int                           flags )
{
   int                     pos;
   netcalc_net_t           nbuff;
   const uint8_t *         addr8;
   char                    tmp[32];
   static char             dst_buffer[NETCALC_ADDRESS_LENGTH];

   assert(net != NULL);
   assert( ((!(dst)) && (!(size))) || (((dst))  && ((size))) );

   size  = ((dst)) ? size : sizeof(dst_buffer);
   dst   = ((dst)) ? dst  : dst_buffer;

   nbuff.net_cidr       = net->net_cidr;
   nbuff.net_flags      = net->net_flags;
   nbuff.net_port       = net->net_port;
   nbuff.net_scope_name = net->net_scope_name;
   addr8                = (const uint8_t *)&net->net_addr.addr8;

   switch(type)
   {
      case NETCALC_TYPE_ADDRESS:
         memcpy(&nbuff.net_addr, &net->net_addr, sizeof(netcalc_addr_t));
         break;

      case NETCALC_TYPE_ARPA_NET:
         flags = NETCALC_UNSET(flags, NETCALC_FLG_IFACE);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_PORT);
         if ((net->net_flags & NETCALC_AF) == NETCALC_AF_INET)
            nbuff.net_cidr = (net->net_cidr / 8) * 8;
         else if ((net->net_flags & NETCALC_AF) == NETCALC_AF_INET6)
            nbuff.net_cidr = (net->net_cidr / 4) * 4;
         else
            return(NULL);
         for(pos = 0; (pos < 16); pos++)
            nbuff.net_addr.addr8[pos] = net->net_addr.addr8[pos] & _netcalc_netmasks[nbuff.net_cidr].addr8[pos];
         break;

      case NETCALC_TYPE_ARPA_HOST:
         if ((net->net_flags & NETCALC_AF) == NETCALC_AF_INET)
            snprintf(dst, size, "%i.%i.%i.%i.in-addr.arpa.", addr8[15], addr8[14], addr8[13], addr8[12]);
         else if ((net->net_flags & NETCALC_AF) == NETCALC_AF_INET6)
            snprintf(dst, size,
               "%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.ip6.arpa.",
               (addr8[15] & 0x0f), ((addr8[15] >> 4) & 0x0f), (addr8[14] & 0x0f), ((addr8[14] >> 4) & 0x0f),
               (addr8[13] & 0x0f), ((addr8[13] >> 4) & 0x0f), (addr8[12] & 0x0f), ((addr8[12] >> 4) & 0x0f),
               (addr8[11] & 0x0f), ((addr8[11] >> 4) & 0x0f), (addr8[10] & 0x0f), ((addr8[10] >> 4) & 0x0f),
               (addr8[9]  & 0x0f), ((addr8[9]  >> 4) & 0x0f), (addr8[8]  & 0x0f), ((addr8[8]  >> 4) & 0x0f),
               (addr8[7]  & 0x0f), ((addr8[7]  >> 4) & 0x0f), (addr8[6]  & 0x0f), ((addr8[6]  >> 4) & 0x0f),
               (addr8[5]  & 0x0f), ((addr8[5]  >> 4) & 0x0f), (addr8[4]  & 0x0f), ((addr8[4]  >> 4) & 0x0f),
               (addr8[3]  & 0x0f), ((addr8[3]  >> 4) & 0x0f), (addr8[2]  & 0x0f), ((addr8[2]  >> 4) & 0x0f),
               (addr8[1]  & 0x0f), ((addr8[1]  >> 4) & 0x0f), (addr8[0]  & 0x0f), ((addr8[0]  >> 4) & 0x0f)
            );
         else
            return(NULL);
         return(dst);

      case NETCALC_TYPE_ARPA_REC:
         if ((net->net_flags & NETCALC_AF) == NETCALC_AF_INET)
         {  dst[0]         = '\0';
            nbuff.net_cidr = (net->net_cidr / 8) * 8;
            for(pos = 15; (pos > (nbuff.net_cidr/8)); pos--)
            {  snprintf(tmp, sizeof(tmp), "%i.", addr8[pos]);
               netcalc_strlcat(dst, tmp, size);
            };
            snprintf(tmp, sizeof(tmp), "%i", addr8[pos]);
            netcalc_strlcat(dst, tmp, size);
            return(dst);
         };
         if ((net->net_flags & NETCALC_AF) == NETCALC_AF_INET6)
         {  dst[0]         = '\0';
            nbuff.net_cidr = (net->net_cidr / 4) * 4;
            for(pos = 31; (pos > (nbuff.net_cidr/4)); pos--)
            {  snprintf(tmp, sizeof(tmp), "%x.", (addr8[pos/2] >> (((pos%2)) ? 0 : 4)) & 0x0f);
               netcalc_strlcat(dst, tmp, size);
            };
            snprintf(tmp, sizeof(tmp), "%x", ((addr8[pos/2] >> 0) & 0x0f));
            netcalc_strlcat(dst, tmp, size);
            return(dst);
         };
         return(NULL);

      case NETCALC_TYPE_ARPA_ZONE:
         if ((net->net_flags & NETCALC_AF) == NETCALC_AF_INET)
         {  dst[0]         = '\0';
            nbuff.net_cidr = (net->net_cidr / 8) * 8;
            for(pos = (nbuff.net_cidr/8)-1; (pos > 12); pos--)
            {  snprintf(tmp, sizeof(tmp), "%i.", addr8[pos]);
               netcalc_strlcat(dst, tmp, size);
            };
            snprintf(tmp, sizeof(tmp), "%i.in-addr.arpa.", addr8[pos]);
            netcalc_strlcat(dst, tmp, size);
            return(dst);
         };
         if ((net->net_flags & NETCALC_AF) == NETCALC_AF_INET6)
         {  dst[0]         = '\0';
            nbuff.net_cidr = (net->net_cidr / 4) * 4;
            for(pos = ((nbuff.net_cidr/4)-1); (pos > 0); pos--)
            {  snprintf(tmp, sizeof(tmp), "%x.", (addr8[pos/2] >> (((pos%2)) ? 0 : 4)) & 0x0f);
               netcalc_strlcat(dst, tmp, size);
            };
            snprintf(tmp, sizeof(tmp), "%x.ip6.arpa.", ((addr8[pos/2] >> 4) & 0x0f));
            netcalc_strlcat(dst, tmp, size);
            return(dst);
         };
         return(NULL);

      case NETCALC_TYPE_BROADCAST:
         flags = NETCALC_UNSET(flags, NETCALC_FLG_COMPR);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_IFACE);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_PORT);
         for(pos = 0; (pos < 16); pos++)
            nbuff.net_addr.addr8[pos] = net->net_addr.addr8[pos] | ~_netcalc_netmasks[net->net_cidr].addr8[pos];
         break;

      case NETCALC_TYPE_FIRST:
         flags = NETCALC_UNSET(flags, NETCALC_FLG_COMPR);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_IFACE);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_PORT);
         for(pos = 0; (pos < 16); pos++)
            nbuff.net_addr.addr8[pos] = net->net_addr.addr8[pos] & _netcalc_netmasks[net->net_cidr].addr8[pos];
         if (net->net_cidr < 127)
            nbuff.net_addr.addr8[15]++;
         break;

      case NETCALC_TYPE_LAST:
         flags = NETCALC_UNSET(flags, NETCALC_FLG_COMPR);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_IFACE);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_PORT);
         for(pos = 0; (pos < 16); pos++)
            nbuff.net_addr.addr8[pos] = net->net_addr.addr8[pos] | ~_netcalc_netmasks[net->net_cidr].addr8[pos];
         if (net->net_cidr < 127)
            nbuff.net_addr.addr8[15]--;
         break;

      case NETCALC_TYPE_NETMASK:
         flags = NETCALC_UNSET(flags, NETCALC_FLG_CIDR);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_IFACE);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_PORT);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_V4MAPPED);
         memcpy(&nbuff.net_addr, &_netcalc_netmasks[net->net_cidr], sizeof(netcalc_addr_t));
         break;

      case NETCALC_TYPE_NETWORK:
         flags = NETCALC_UNSET(flags, NETCALC_FLG_IFACE);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_PORT);
         for(pos = 0; (pos < 16); pos++)
            nbuff.net_addr.addr8[pos] = net->net_addr.addr8[pos] & _netcalc_netmasks[net->net_cidr].addr8[pos];
         break;

      case NETCALC_TYPE_WILDCARD:
         flags = NETCALC_UNSET(flags, NETCALC_FLG_CIDR);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_IFACE);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_PORT);
         flags = NETCALC_UNSET(flags, NETCALC_FLG_V4MAPPED);
         for(pos = 0; (pos < 16); pos++)
            nbuff.net_addr.addr8[pos] = ~_netcalc_netmasks[net->net_cidr].addr8[pos];
         break;

      default:
         return(NULL);
   };
   net = &nbuff;

   switch(net->net_flags & NETCALC_AF)
   {
      case NETCALC_AF_EUI48:  return(netcalc_ntop_eui(net, dst, size, flags));
      case NETCALC_AF_EUI64:  return(netcalc_ntop_eui(net, dst, size, flags));
      case NETCALC_AF_INET:   return(netcalc_ntop_inet(net, dst, size, flags));
      case NETCALC_AF_INET6:  return(netcalc_ntop_inet6(net, dst, size, flags));

      default:
         dst[0] = '\0';
         break;
   };

   return(dst);
}


const char *
netcalc_ntop_eui(
         netcalc_net_t *               net,
         char *                        dst,
         size_t                        size,
         int                           flags )
{
   size_t   idx;
   size_t   idx_start;
   size_t   off;
   int      delim;
   char     map[] = "0123456789abcdef";

   assert(net != NULL);
   assert( ((!(dst)) && (!(size))) || (((dst))  && ((size))) );

   delim = (flags & NETCALC_DELIM);
   switch(net->net_flags & NETCALC_AF)
   {
      case NETCALC_AF_EUI64:
         idx_start = 8;
         delim       = ((delim)) ? delim : (NETCALC_DFLT_EUI64 & NETCALC_DELIM);
         break;

      case NETCALC_AF_EUI48:
      default:
         idx_start = 10;
         delim       = ((delim)) ? delim : (NETCALC_DFLT_EUI48 & NETCALC_DELIM);
         break;
   };

   for(idx = idx_start, off = 0; (idx < 16); idx++)
   {
      if (size < (off + 3))
         return(NULL);
      switch(delim)
      {
         case NETCALC_FLG_DOT:
            if ((off % 5) == 4)
               dst[off++] = '.';
            dst[off++] = map[(net->net_addr.addr8[idx] >> 4) & 0x0f];
            dst[off++] = map[(net->net_addr.addr8[idx] >> 0) & 0x0f];
            break;

         case NETCALC_FLG_NODELIM:
            dst[off++] = map[(net->net_addr.addr8[idx] >> 4) & 0x0f];
            dst[off++] = map[(net->net_addr.addr8[idx] >> 0) & 0x0f];
            break;

         case NETCALC_FLG_DASH:
         case NETCALC_FLG_COLON:
         default:
            if ((off))
               dst[off++] = ((delim == NETCALC_FLG_DASH) ? '-' : ':');
            dst[off++] = map[(net->net_addr.addr8[idx] >> 4) & 0x0f];
            dst[off++] = map[(net->net_addr.addr8[idx] >> 0) & 0x0f];
            break;
      };
   };

   if (size < (off + 1))
      return(NULL);
   dst[off] = '\0';

   return(dst);
}


const char *
netcalc_ntop_inet(
         netcalc_net_t *               net,
         char *                        dst,
         size_t                        size,
         int                           flags )
{
   size_t   idx;
   size_t   off;
   size_t   buff_len;
   size_t   pos;
   char     buff[8];

   assert(net != NULL);
   assert( ((!(dst)) && (!(size))) || (((dst))  && ((size))) );

   off = 0;

   // adjusting flags NETCALC_FLG_CIDR
   if ((flags & NETCALC_FLG_CIDR))
      flags |= (net->net_cidr == 128) ? 0 : NETCALC_FLG_CIDR_ALWAYS;

   // adjusting flags NETCALC_FLG_PORT
   if (!(net->net_port))
      flags = flags & ~NETCALC_FLG_PORT;

   for(idx = 12; (idx < 16); idx++)
   {
      if ((flags & NETCALC_FLG_SUPR))
         buff_len = snprintf(buff, sizeof(buff), "%i", net->net_addr.addr8[idx]);
      else
         buff_len = snprintf(buff, sizeof(buff), "%03i", net->net_addr.addr8[idx]);
      if (size < (off+buff_len+2))
         return(NULL);
      if ((off))
         dst[off++] = '.';
      for(pos = 0; (pos < buff_len); pos++, off++)
         dst[off] = buff[pos];
   };

   // append CIDR
   if ((flags & NETCALC_FLG_CIDR_ALWAYS))
   {
      buff_len = snprintf(buff, sizeof(buff), "/%i", (net->net_cidr-96));
      if (size < (off+buff_len+1))
         return(NULL);
      for(pos = 0; (pos < buff_len); pos++, off++)
         dst[off] = buff[pos];
   };

   // append port
   if ((flags & NETCALC_FLG_PORT))
   {
      buff_len = snprintf(buff, sizeof(buff), ":%i", net->net_port);
      if (size < (off+buff_len+1))
         return(NULL);
      for(pos = 0; (pos < buff_len); pos++, off++)
         dst[off] = buff[pos];
   };

   dst[off] = '\0';

   return(dst);
}


const char *
netcalc_ntop_inet6(
         netcalc_net_t *               net,
         char *                        dst,
         size_t                        size,
         int                           flags )
{
   int         idx;
   size_t      off;
   char        buff[8];
   int         buff_len;
   int         pos;
   int         bracketed;
   int         zero_off;
   int         zero_len;
   int         zero_max_off;
   int         zero_max_len;
   int         ipv4_flags;
   int         max_len;
   uint8_t *   dat8;
   char        map[] = "0123456789abcdef";
   char        ipv4[NETCALC_ADDRESS_LENGTH];

   assert(net != NULL);
   assert( ((!(dst)) && (!(size))) || (((dst))  && ((size))) );

   off            = 0;
   bracketed      = 0;
   dat8           = net->net_addr.addr8;
   zero_max_off   = -1;
   zero_max_len   = 0;

   if (size <= 3)
      return(NULL);

   // adjusting flags NETCALC_FLG_CIDR and NETCALC_FLG_CIDR_ALWAYS
   if ((flags & NETCALC_FLG_CIDR))
      flags |= (net->net_cidr == 128) ? 0 : NETCALC_FLG_CIDR_ALWAYS;

   // adjusting flags NETCALC_FLG_PORT
   if (!(net->net_port))
      flags = flags & ~NETCALC_FLG_PORT;

   // adjusting flags NETCALC_FLG_IFACE
   if ( (!(net->net_scope_name)) || (!(net->net_scope_name[0])) )
      flags = flags & ~NETCALC_FLG_IFACE;

   // determine if brackets are needed
   if ((flags & NETCALC_FLG_PORT))
      bracketed = 1;
   if ( ((flags & NETCALC_FLG_CIDR_ALWAYS)) && ((flags & NETCALC_FLG_IFACE)) )
      bracketed = 1;
   if ((bracketed))
      dst[off++] = '[';

   // calculates zero compression
   if ((flags & NETCALC_FLG_COMPR))
   {
      zero_off = -1;
      zero_len = 0;
      max_len  = (flags & NETCALC_FLG_V4MAPPED) ? 12 : 16;
      for(idx = 0; (idx < max_len); idx += 2)
      {
         if ( ((dat8[idx+0])) || ((dat8[idx+1])) )
         {
            if (zero_off == -1)
               continue;
            if (zero_len > zero_max_len)
            {  zero_max_off = zero_off;
               zero_max_len = zero_len;
            };
            zero_off = -1;
            zero_len = 0;
         } else
         {
            if (zero_off == -1)
               zero_off = idx;
            zero_len += 2;
         };
      };
      if ( (zero_off != -1) && (zero_len > zero_max_len) )
      {  zero_max_off = zero_off;
         zero_max_len = zero_len;
      };
      if (zero_max_len < 4)
         zero_max_off = -1;
   };

   for(idx = 0; (idx < 16); idx += 2)
   {
      // apply zero compression
      if (idx == zero_max_off)
      {  if (size <= (off+3))
            return(NULL);
         idx += zero_max_len;
         dst[off++] = ':';
         if (idx >= 16)
         {  dst[off++] = ':';
            continue;
         };
      };

      // apply delimiter
      if ( (!(idx % 2)) && ((idx)) )
      {  if (size <= (off+2))
            return(NULL);
         dst[off++] = ':';
      };

      // save data
      if ( ((flags & NETCALC_FLG_V4MAPPED)) && (idx == 12) )
      {
         ipv4_flags = flags;
         ipv4_flags = NETCALC_UNSET(ipv4_flags, NETCALC_FLG_PORT);
         ipv4_flags = NETCALC_UNSET(ipv4_flags, NETCALC_FLG_CIDR);
         ipv4_flags = NETCALC_UNSET(ipv4_flags, NETCALC_FLG_CIDR_ALWAYS);
         if (!(netcalc_ntop_inet(net, ipv4, sizeof(ipv4), ipv4_flags)))
            return(NULL);
         if (size <= (off+strlen(ipv4)+1))
            return(NULL);
         dst[off] = '\0';
         off += netcalc_strlcat(dst, ipv4, size);
         continue;
      };
      if (size <= (off+5))
         return(NULL);
      if ( (((dat8[idx+0] >> 4) & 0x0f)) || (!(flags & NETCALC_FLG_SUPR)) )
         dst[off++] = map[(dat8[idx+0] >> 4) & 0x0f];
      if ( ((dat8[idx+0])) || (!(flags & NETCALC_FLG_SUPR)) )
         dst[off++] = map[(dat8[idx+0] >> 0) & 0x0f];
      if ( ((dat8[idx+0])) || (((dat8[idx+1] >> 4) & 0x0f)) || (!(flags & NETCALC_FLG_SUPR)) )
         dst[off++] = map[(dat8[idx+1] >> 4) & 0x0f];
      dst[off++] = map[(dat8[idx+1] >> 0) & 0x0f];
   };

   // append scope name
   if ( ((flags & NETCALC_FLG_IFACE)) )
   {
      if (size <= (off+2+strlen(net->net_scope_name)))
         return(NULL);
      dst[off++] = '%';
      for(idx = 0; ((net->net_scope_name[idx])); idx++)
         dst[off++] = net->net_scope_name[idx];
   };

   if (size <= (off+2))
      return(NULL);
   if ((bracketed))
      dst[off++] = ']';

   // append CIDR
   if ((flags & NETCALC_FLG_CIDR_ALWAYS))
   {
      buff_len = snprintf(buff, sizeof(buff), "/%i", net->net_cidr);
      if (size <= (off+buff_len+1))
         return(NULL);
      for(pos = 0; (pos < buff_len); pos++, off++)
         dst[off] = buff[pos];
   };

   // append port
   if ((flags & NETCALC_FLG_PORT))
   {
      buff_len = snprintf(buff, sizeof(buff), ":%i", net->net_port);
      if (size <= (off+buff_len+1))
         return(NULL);
      for(pos = 0; (pos < buff_len); pos++, off++)
         dst[off] = buff[pos];
   };

   dst[off] = '\0';

   return(dst);
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

   memset(&net_addr, 0, sizeof(netcalc_addr_t));
   delim = 0;
   addr8 = net_addr.addr8;
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

   memset(net_addr.addr8, 0, sizeof(net_addr.addr8));
   addr8 = net_addr.addr8;
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
         addr8[12+byte] = dec;
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

   memcpy(&n->net_addr.addr8[12], &net_addr.addr8[12], 4);
   n->net_cidr =  (uint8_t)((cidr != -1) ? cidr : n->net_cidr);
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

   // attempt to process as IPv4-mapped IPv6 address
   if (netcalc_parse_inet(n, address) == NETCALC_SUCCESS)
   {  n->net_flags |= NETCALC_FLG_V4MAPPED;
      n->net_addr.addr8[10] = 0xff;
      n->net_addr.addr8[11] = 0xff;
      return(NETCALC_SUCCESS);
   };

   memset(&net_addr, 0, sizeof(netcalc_addr_t));
   memset(scope_name, 0, sizeof(scope_name));
   zero        = 0;
   addr8       = net_addr.addr8;
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
      if (cidr != -1)
         return(NETCALC_EBADADDR);
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
         addr8[(wyde*2)+0] = (hex >> 8) & 0xff;
         addr8[(wyde*2)+1] = (hex >> 0) & 0xff;
         if (str[pos+1] == ':')
         {
            if (str[pos+2] == ':')
               return(NETCALC_EBADADDR);
            if ((zero))
               return(NETCALC_EBADADDR);
            zero++;
            pos++;
            for(fwd = pos+1, wyde_fwd = 1; ((str[fwd])); fwd++)
            {
               if (str[fwd] == ':')
                  wyde_fwd++;
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
         {  n->net_cidr    =  (uint8_t)((cidr != -1)  ? cidr : n->net_cidr);
            n->net_port    =  (uint16_t)((port != -1) ? port : n->net_port);
            n->net_flags   |= NETCALC_FLG_V4MAPPED;
            memcpy(&n->net_addr.addr8, &net_addr.addr8, 12);
            return(0);
         };
      };

      if ( (wyde > 7) || (digit > 4) || (hex > 0xffff) )
         return(NETCALC_EBADADDR);
   };

   if (wyde != 7)
      return(NETCALC_EBADADDR);

   addr8[(wyde*2)+0] = (hex >> 8) & 0xff;
   addr8[(wyde*2)+1] = (hex >> 0) & 0xff;

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
      case NETCALC_ENOTSUP:      return("operation not supported");

      case NETCALC_EUNKNOWN:     break;
      default:                   break;
   };
   return("unknown error");
}


size_t
netcalc_strfnet(
         netcalc_net_t *               net,
         char *                        s,
         size_t                        maxsize,
         const char *                  fmt,
         int                           flags )
{
   int         mflgs;
   int         modifiers;
   size_t      pos;
   size_t      off;
   size_t      padding;
   size_t      buff_len;
   size_t      buff_pos;
   size_t      pad_to_right;
   char        buff[NETCALC_ADDRESS_LENGTH];

   assert(net != NULL);

   maxsize = ((s)) ? maxsize : 0;
   if ((maxsize))
      s[0] = '\0';

   for(pos = 0, off = 0; ((fmt[pos])); pos++)
   {
      // copy non-keywords
      if (fmt[pos] != '%')
      {  if ((off+1) < maxsize)
         {  s[off+0] = fmt[pos];
            s[off+1] = '\0';
         };
         off++;
         continue;
      };

      if (!(fmt[++pos]))
         continue;

      // applies modifiers
      mflgs       = flags;
      modifiers   = 1;
      padding     = 0;
      pad_to_right        = 0;
      while ((modifiers))
      {  // do not allow multiple padding modifiers
         if ( ( (fmt[pos] >= '0') && (fmt[pos] <= '9') ) || (fmt[pos] == '-') )
            if ( ((padding)) || ((pad_to_right)) )
               return(0);

         // process padding modifiers
         while ( ( (fmt[pos] >= '0') && (fmt[pos] <= '9') ) || (fmt[pos] == '-') )
         {
            if (fmt[pos] == '-') // left adjustment
            {  if ( ((pad_to_right)) || ((padding)) )
                  return(0);
               pad_to_right = 1;
            } else               // padding size
            {  padding *= 10;
               padding += fmt[pos] - '0';
            };
            pos++;
         };

         // process character modifiers
         switch(fmt[pos])
         {  case 'Z': mflgs = NETCALC_UNSET( mflgs, NETCALC_FLG_COMPR);       break;
            case 'z': mflgs = NETCALC_SET(   mflgs, NETCALC_FLG_COMPR);       break;
            case 'M': mflgs = NETCALC_UNSET( mflgs, NETCALC_FLG_V4MAPPED);    break;
            case 'm': mflgs = NETCALC_SET(   mflgs, NETCALC_FLG_V4MAPPED);    break;
            case 'S': mflgs = NETCALC_UNSET( mflgs, NETCALC_FLG_SUPR);        break;
            case 's': mflgs = NETCALC_SET(   mflgs, NETCALC_FLG_SUPR);        break;

            case 'B': mflgs &= ~NETCALC_DELIM; mflgs |=  NETCALC_FLG_NODELIM; break;
            case 'L': mflgs &= ~NETCALC_DELIM; mflgs |=  NETCALC_FLG_COLON;   break;
            case 'H': mflgs &= ~NETCALC_DELIM; mflgs |=  NETCALC_FLG_DASH;    break;
            case 'O': mflgs &= ~NETCALC_DELIM; mflgs |=  NETCALC_FLG_DOT;  break;

            default: // applies spacing modifer
               modifiers = 0;
               break;
         };

         // move to next modifier
         if ((modifiers))
            pos++;
      };

      mflgs &= NETCALC_FLG_COMPR | NETCALC_FLG_SUPR | NETCALC_FLG_V4MAPPED | NETCALC_DELIM;

      // process key words
      switch(fmt[pos])
      {
         case '%':
            buff[0] = '%';
            buff[1] = '\0';
            break;

         // copy address with default options
         case 'A':
            mflgs |= NETCALC_FLG_IFACE | NETCALC_FLG_CIDR | NETCALC_FLG_PORT;
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_ADDRESS, mflgs);
            break;

         // copy address without interface, CIDR, or port
         case 'a':
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_ADDRESS, mflgs);
            break;

         // copy broadcast
         case 'b':
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_BROADCAST, mflgs);
            break;

         // copy CIDR with delimiter
         case 'c':
            if (net->net_cidr == 128)
               continue;
            snprintf(buff, sizeof(buff), "/%i", (int)net->net_cidr);
            break;

         // copy CIDR without delimiter
         case 'C':
            snprintf(buff, sizeof(buff), "%i", (int)net->net_cidr);
            break;

         // copy DNS arpa zone
         case 'D':
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_ARPA_ZONE, mflgs);
            break;

         // copy DNS arpa host
         case 'd':
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_ARPA_HOST, mflgs);
            break;

         // copy family
         case 'F':
            snprintf(buff, sizeof(buff), "EUI48");
            switch(net->net_flags & NETCALC_AF)
            {  case NETCALC_AF_EUI48: snprintf(buff, sizeof(buff), "EUI48");  break;
               case NETCALC_AF_EUI64: snprintf(buff, sizeof(buff), "EUI64");  break;
               case NETCALC_AF_INET:  snprintf(buff, sizeof(buff), "IPv4");   break;
               case NETCALC_AF_INET6: snprintf(buff, sizeof(buff), "IPv6");   break;
               default: snprintf(buff, sizeof(buff), "unknown"); break;
            };
            break;

         // copy first usable address
         case 'f':
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_FIRST, mflgs);
            break;

         // copy interface without delimiter
         case 'I':
            if ( (!(net->net_scope_name)) || (!(net->net_scope_name[0])) )
               continue;
            snprintf(buff, sizeof(buff), "%s", net->net_scope_name);
            break;

         // copy interface with delimiter
         case 'i':
            if ( (!(net->net_scope_name)) || (!(net->net_scope_name[0])) )
               continue;
            snprintf(buff, sizeof(buff), "%%%s", net->net_scope_name);
            break;

         // copy first usable address
         case 'l':
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_LAST, mflgs);
            break;

         // copy netmask
         case 'N':
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_NETMASK, mflgs);
            break;

         // copy network address without interface, CIDR, or port
         case 'n':
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_NETWORK, mflgs);
            break;

         // copy port without delimiter
         case 'P':
            snprintf(buff, sizeof(buff), "%i", (int)net->net_port);
            break;

         // copy port with delimiter
         case 'p':
            if (!(net->net_port))
               continue;
            snprintf(buff, sizeof(buff), ":%i", (int)net->net_port);
            break;

         // copy wildcard
         case 'W':
            netcalc_ntop(net, buff, sizeof(buff), NETCALC_TYPE_WILDCARD, mflgs);
            break;

         // unknown option
         default:
            return(0);
      };

      // apply padding to string
      buff_len = strlen(buff);
      padding  = (padding < (sizeof(buff)-1)) ? padding : (sizeof(buff)-1);
      if ( buff_len < padding )
      {  if ((pad_to_right))
         {  for(buff_pos = buff_len; (buff_pos < padding); buff_pos++)
               buff[buff_pos] = ' ';
         } else
         {  for(buff_pos = buff_len; (buff_pos > 0); buff_pos--)
               buff[buff_pos + (padding - buff_len) - 1] = buff[buff_pos-1];
            for(buff_pos = (padding - buff_len); (buff_pos > 0); buff_pos--)
               buff[buff_pos-1] = ' ';
         };
         buff[padding] = '\0';
      };

      off = netcalc_strlcat(s, buff, maxsize);
   };

   // NULL terminate string
   if ((s))
      s[ (off < maxsize) ? off : (maxsize - 1) ] = '\0';

   return(off);
}


size_t
netcalc_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize )
{
   size_t      pos;
   size_t      offset;
   size_t      len;

   assert(src     != NULL);

   if (!(dst))
      return(strlen(src));
   pos = strlen(dst);

   for(offset = 0; ((src[offset])); offset++)
      if ((pos+offset) < dstsize)
         dst[pos+offset] = src[offset];
   len = offset + pos;

   dst[((len < dstsize) ? len : (dstsize-1))] = '\0';

   return(len);
}


int
netcalc_superblock(
         netcalc_net_t **              netp,
         netcalc_net_t **              nets,
         size_t                        nel )
{
   int                     byte;
   int                     cidr;
   int                     matches;
   size_t                  idx;
   netcalc_net_t           nbuff;
   netcalc_addr_t *        ref;
   const uint8_t *         addr8;

   assert(netp != NULL);
   assert(nets != NULL);
   assert(nel  != 0);

   for(idx = 0; (idx < nel); idx++)
      assert(nets[idx] != NULL);

   matches  = 1;
   ref      = &nets[0]->net_addr;

   if (nel == 1)
      return(netcalc_dup(netp, nets[0]));

   memset(&nbuff, 0, sizeof(netcalc_net_t));
   nbuff.net_flags = nets[0]->net_flags & NETCALC_AF;

   cidr = 1;
   while ((cidr < 129) && ((matches)))
   {
      addr8 = (const uint8_t *)&_netcalc_netmasks[cidr].addr8;
      for(idx = 1; ((idx < nel) && ((matches))); idx++)
      {
         byte = (cidr-1) / 8;
         if ( (ref->addr8[byte] & addr8[byte]) != (nets[idx]->net_addr.addr8[byte] & addr8[byte]) )
            matches = 0;
         else if (cidr > nets[idx]->net_cidr)
            matches = 0;
      };
      if ((matches))
         cidr++;
   };
   cidr--;

   nbuff.net_cidr = cidr;
   for(byte = 0; (byte < 16); byte++)
      nbuff.net_addr.addr8[byte] = nets[0]->net_addr.addr8[byte] & _netcalc_netmasks[cidr].addr8[byte];

   return(netcalc_dup(netp, &nbuff));
}

/* end of source */
