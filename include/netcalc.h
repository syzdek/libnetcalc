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
 *  include/netcalc.h - common includes and prototypes
 */
#ifndef __NETCALC_H
#define __NETCALC_H 1

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <inttypes.h>
#include <stddef.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros

#undef _NETCALC_I
#undef _NETCALC_F
#undef _NETCALC_V
#ifdef _LIB_LIBNETCALC_H
#  define _NETCALC_I    inline
#  define _NETCALC_F    /* empty */
#  define _NETCALC_V    extern
#else
#  define _NETCALC_I    extern
#  define _NETCALC_F    extern
#  define _NETCALC_V    extern
#endif


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions

#define NETCALC_ADDRESS_LENGTH         128
#define NETCALC_SCOPE_NAME_LENGTH      48


#define NETCALC_SUCCESS                0
#define NETCALC_EUNKNOWN               -1
#define NETCALC_ENOMEM                 -2
#define NETCALC_EBUFFLEN               -3
#define NETCALC_EBADADDR               -4
#define NETCALC_EINVAL                 -5
#define NETCALC_EFIELD                 -6


#define NETCALC_FLD_FAMILY             0
#define NETCALC_FLD_FLAGS              1
#define NETCALC_FLD_PORT               2
#define NETCALC_FLD_CIDR               3
#define NETCALC_FLD_SCOPE_NAME         4
#define NETCALC_FLD_ADDR               5
#define NETCALC_FLD_ADDRLEN            6


#define NETCALC_FLG                    0x000fffff  // flag: mask for flags
#define NETCALC_FLG_V4MAPPED           0x00000001  // flag: display IPv6 addresses as IPv4 mapped address
#define NETCALC_FLG_COMPR              0x00000002  // flag: zero compression
#define NETCALC_FLG_SUPR               0x00000004  // flag: zero suppression
#define NETCALC_FLG_CIDR               0x00000008  // flag: append CIDR
#define NETCALC_FLG_PORT               0x00000010  // flag: append port
#define NETCALC_FLG_IFACE              0x00000020  // flag: append interface
//                                     0x00000040  // flag: OPEN
//                                     0x00000080  // flag: OPEN
#define NETCALC_FLG_NOV4MAPPED         0x00000100  // flag: do not display IPv6 addresses as IPv4 mapped address
#define NETCALC_FLG_NOCOMPR            0x00000200  // flag: no zero compression
#define NETCALC_FLG_NOSUPR             0x00000400  // flag: no zero suppression
#define NETCALC_FLG_NOCIDR             0x00000800  // flag: do not append CIDR
#define NETCALC_FLG_NOPORT             0x00001000  // flag: do not append port
#define NETCALC_FLG_NOIFACE            0x00002000  // flag: do not append interface
//                                     0x00004000  // flag: OPEN
//                                     0x00008000  // flag: OPEN
#define NETCALC_DELIM                  0x000f0000  // delimiter: mask for EUI delimiters
#define NETCALC_FLG_COLON              0x00010000  // delimiter: use colon delimiters with 1 byte chunks
#define NETCALC_FLG_DASH               0x00020000  // delimiter: use dash delimiters with 1 byte chunks
#define NETCALC_FLG_DOT                0x00040000  // delimiter: use dot delimiters with 2 byte chunks
#define NETCALC_FLG_NODELIM            0x00080000  // delimiter: do not use delimiters
//                                     0x00f00000  // UNUSED
#define NETCALC_AF                     0x0f000000  // af: mask for address familly
#define NETCALC_AF_AUTO                0x00000000  // af: added for consistenency
#define NETCALC_AF_INET                0x01000000  // af: IPv4 encapsulation [RFC2003]
#define NETCALC_AF_INET6               0x02000000  // af: IPv6 encapsulation [RFC2473]
#define NETCALC_AF_EUI48               0x04000000  // af: MAC-48/EUI-48 encapsulation
#define NETCALC_AF_EUI64               0x08000000  // af: EUI-64 encapsulation
//                                     0x70000000  // UNUSED
//                                     0x80000000  // DO NOT USE
#define NETCALC_AF_MAC                 NETCALC_AF_EUI48


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types

typedef struct _libnetcalc_address     netcalc_addr_t;
typedef struct _libnetcalc_network     netcalc_net_t;


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

_NETCALC_F netcalc_net_t *
netcalc_alloc(
         void );


_NETCALC_F void
netcalc_free(
         netcalc_net_t *               net );


_NETCALC_F int
netcalc_get_field(
         netcalc_net_t  *              net,
         int                           option,
         void *                        outvalue );


_NETCALC_F int
netcalc_initialize(
         netcalc_net_t **              netp,
         const char *                  address,
         int                           flags );


_NETCALC_F const char *
netcalc_strerror(
         int                           errnum );


#endif /* end of header */
