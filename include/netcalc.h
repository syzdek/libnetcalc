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


#define NETCALC_SET(flgs, flg)      ( flgs | flg )
#define NETCALC_UNSET(flgs, flg)    ( flgs & ~flg )


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
#define NETCALC_ENOTSUP                -7


#define NETCALC_FLD_FAMILY             0
#define NETCALC_FLD_FLAGS              1
#define NETCALC_FLD_PORT               2
#define NETCALC_FLD_CIDR               3
#define NETCALC_FLD_SCOPE_NAME         4
#define NETCALC_FLD_ADDR               5
#define NETCALC_FLD_ADDRLEN            6
#define NETCALC_FLD_UL                 7  // Universal vs. local (interprets U/L bit)
#define NETCALC_FLD_UL_BIT             8  // Universal vs. local (returns U/L bit)
#define NETCALC_FLD_IG                 9  // Unicast vs. multicast (interprets  I/G bit)
#define NETCALC_FLD_IG_BIT             10 // Unicast vs. multicast (returns  I/G bit)
#define NETCALC_FLD_IPV4_MAPPED        11

#define NETCALC_IG_UNICAST             0  // is locally administered
#define NETCALC_IG_MULTICAST           1  // is universally administered

#define NETCALC_UL_LOCAL               1  // is locally administered
#define NETCALC_UL_UNIVERSAL           0  // is universally administered


#define NETCALC_FLG                    0x000fffff  // flag: mask for flags
#define NETCALC_FLG_V4MAPPED           0x00000001  // flag: display IPv6 addresses as IPv4 mapped address
#define NETCALC_FLG_COMPR              0x00000002  // flag: zero compression
#define NETCALC_FLG_SUPR               0x00000004  // flag: zero suppression
#define NETCALC_FLG_IFACE              0x00000008  // flag: append interface
#define NETCALC_FLG_CIDR               0x00000010  // flag: append CIDR
#define NETCALC_FLG_CIDR_ALWAYS        0x00000020  // flag: always append CIDR
#define NETCALC_FLG_PORT               0x00000040  // flag: append port
//                                     0x00000010  // flag: OPEN
//                                     0x00000020  // flag: OPEN
//                                     0x00000040  // flag: OPEN
//                                     0x00000080  // flag: OPEN
#define NETCALC_USR                    0x0000f000
#define NETCALC_FLG_USR1               0x00001000
#define NETCALC_FLG_USR2               0x00002000
#define NETCALC_FLG_USR3               0x00004000
#define NETCALC_FLG_USR4               0x00008000
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


// defaults flags
#define NETCALC_DFLTS                        (NETCALC_FLG_SUPR | NETCALC_FLG_COMPR | NETCALC_FLG_CIDR | NETCALC_FLG_PORT | NETCALC_FLG_IFACE)
#define NETCALC_DFLT_EUI48                   (NETCALC_FLG_COLON)
#define NETCALC_DFLT_EUI64                   (NETCALC_FLG_DOT)
#define NETCALC_DFLT_INET                    (NETCALC_FLG_SUPR | NETCALC_FLG_CIDR | NETCALC_FLG_PORT)
#define NETCALC_DFLT_INET6                   (NETCALC_FLG_SUPR | NETCALC_FLG_COMPR | NETCALC_FLG_CIDR | NETCALC_FLG_PORT | NETCALC_FLG_IFACE)


#define NETCALC_TYPE_ADDRESS           0x00
#define NETCALC_TYPE_NETWORK           0x01
#define NETCALC_TYPE_BROADCAST         0x02
#define NETCALC_TYPE_NETMASK           0x03
#define NETCALC_TYPE_WILDCARD          0x04
#define NETCALC_TYPE_FIRST             0x05
#define NETCALC_TYPE_LAST              0x06
#define NETCALC_TYPE_ARPA_NET          0x07
#define NETCALC_TYPE_ARPA_ZONE         0x08
#define NETCALC_TYPE_ARPA_REC          0x09
#define NETCALC_TYPE_ARPA_HOST         0x0a


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types

typedef union  _libnetcalc_address     netcalc_addr_t;
typedef struct _libnetcalc_network     netcalc_net_t;
typedef struct _libnetcalc_record      netcalc_rec_t;
typedef struct _libnetcalc_set         netcalc_set_t;


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

_NETCALC_F int
netcalc_convert(
         netcalc_net_t *               net,
         int                           family,
         const netcalc_net_t *         prefix );


_NETCALC_F int
netcalc_dflt_flags(
         int                           family );


_NETCALC_F int
netcalc_dup(
         netcalc_net_t **              netp,
         const netcalc_net_t *         src );


_NETCALC_F void
netcalc_free(
         netcalc_net_t *               net );


_NETCALC_F int
netcalc_get_field(
         const netcalc_net_t  *        net,
         int                           option,
         void *                        outvalue );


_NETCALC_F int
netcalc_initialize(
         netcalc_net_t **              netp,
         const char *                  address,
         int                           flags );


_NETCALC_F int
netcalc_network_mask(
         netcalc_net_t *               net,
         const netcalc_net_t *         prefix,
         int                           cidr );


_NETCALC_F const char *
netcalc_ntop(
         const netcalc_net_t *         net,
         char *                        dst,
         size_t                        size,
         int                           type,
         int                           flags );


_NETCALC_F void
netcalc_set_free(
         netcalc_set_t *               ns );


_NETCALC_F int
netcalc_set_init(
         netcalc_set_t **              nsp,
         int                           flags );


_NETCALC_F const char *
netcalc_strerror(
         int                           errnum );


_NETCALC_F size_t
netcalc_strfnet(
         const netcalc_net_t *         net,
         char *                        s,
         size_t                        maxsize,
         const char *                  format,
         int                           flags );


_NETCALC_F int
netcalc_superblock(
         netcalc_net_t **              netp,
         const netcalc_net_t * const * nets,
         size_t                        nel );


#endif /* end of header */
