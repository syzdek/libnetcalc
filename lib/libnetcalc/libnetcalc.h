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
/*
 *  lib/libnetcalc/libnetcalc.h - common includes and prototypes
 */
#ifndef __LIB_LIBNETCALC_H
#define __LIB_LIBNETCALC_H 1

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

// defined in the Single UNIX Specification
#ifndef _XOPEN_SOURCE
#   define _XOPEN_SOURCE 600
#endif

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <netcalc.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions

#define NETCALC_SET                    0x00fff000  // set:
#define NETCALC_SET_PORT               0x00001000  // set: CIDR is set
#define NETCALC_SET_CIDR               0x00002000  // set: port is set


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types

struct _libnetcalc_address
{
   union
   {
      uint8_t                    netcalc_addr8[16];
      uint16_t                   netcalc_addr16[8];
      uint32_t                   netcalc_addr32[4];
      uint64_t                   netcalc_addr64[2];
   } netcalc_addr;
#define addr_dat8               netcalc_addr.netcalc_addr8
#define addr_dat16              netcalc_addr.netcalc_addr16
#define addr_dat32              netcalc_addr.netcalc_addr32
#define addr_dat64              netcalc_addr.netcalc_addr64
};


struct _libnetcalc_network
{
   uint32_t                   net_flags;
   uint16_t                   net_port;
   uint8_t                    net_cidr;
   uint8_t                    __pad;
   netcalc_addr_t             net_addr;
   char *                     net_scope_name;
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

extern const netcalc_addr_t _netcalc_netmasks[];
extern const netcalc_net_t _netcalc_lo_in;
extern const netcalc_net_t _netcalc_lo_in6;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes


#endif /* end of header */

