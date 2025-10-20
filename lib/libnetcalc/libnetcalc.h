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


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types

typedef struct _libnetcalc_network_buffer    netcalc_buff_t;


union _libnetcalc_address
{  uint8_t                    addr8[16];
   uint16_t                   addr16[8];
   uint32_t                   addr32[4];
   uint64_t                   addr64[2];
};


struct _libnetcalc_network
{  uint32_t                   net_flags;
   uint16_t                   net_port;
   uint8_t                    net_cidr;
   uint8_t                    __pad;
   netcalc_addr_t             net_addr;
   char *                     net_scope_name;
};


struct _libnetcalc_records
{  uint32_t                   len;
   uint32_t                   size;
   netcalc_rec_t **           list;
};


struct _libnetcalc_record
{  uint32_t                   rec_flags;
   uint16_t                   __pad_uint16_t;
   uint8_t                    rec_cidr;
   uint8_t                    rec_used;
   netcalc_addr_t             rec_addr;
   char *                     rec_comment;
   void *                     rec_data;
   netcalc_recs_t             rec_children;
};


struct _libnetcalc_set
{  uint32_t                   set_flags;
   uint32_t                   set_serial;
   netcalc_recs_t             set_recs;
};


struct _libnetcalc_cursor
{  uint32_t                   cur_depth;
   uint32_t                   cur_serial;
   uint32_t                   cur_idx[130];
   netcalc_set_t *            cur_set;
};


struct _libnetcalc_network_buffer
{  char                       buff_scope_name[NETCALC_SCOPE_NAME_LENGTH];
   netcalc_net_t              buff_net;
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

extern const netcalc_net_t    _netcalc_ipv4_mapped_ipv6;
extern const netcalc_net_t    _netcalc_link_local_in;
extern const netcalc_net_t    _netcalc_link_local_in6;
extern const netcalc_net_t    _netcalc_lo_in;
extern const netcalc_net_t    _netcalc_lo_in6;
extern const netcalc_addr_t   _netcalc_netmasks[];
extern const netcalc_net_t    _netcalc_slaac_in6;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

extern int
netcalc_addr_cmp(
         const netcalc_addr_t *        a1,
         uint8_t                       c1,
         const netcalc_addr_t *        a2,
         uint8_t                       c2 );


extern int
netcalc_addr_convert(
         netcalc_addr_t *              addr,
         int                           family,
         int                           addr_family );


extern int
netcalc_addr_convert_eui48(
         netcalc_addr_t *              addr,
         int                           family  );


extern int
netcalc_addr_convert_eui64(
         netcalc_addr_t *              addr,
         int                           family  );


extern int
netcalc_addr_convert_inet(
         netcalc_addr_t *              addr,
         int                           family  );


extern int
netcalc_addr_convert_inet6(
         netcalc_addr_t *              addr,
         int                           family  );


extern int
netcalc_copy(
         netcalc_net_t *               dst,
         const netcalc_net_t *         src );


extern int
netcalc_parse(
         netcalc_buff_t *              b,
         const char *                  address,
         int                           flags );


extern size_t
netcalc_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


extern size_t
netcalc_strlcpy(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


#endif /* end of header */

