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
#define __LIB_LIBNETCALC_LADDR_C 1
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


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
netcalc_addr_cmp(
         const netcalc_addr_t *        a1,
         uint8_t                       c1,
         const netcalc_addr_t *        a2,
         uint8_t                       c2 )
{
   uint8_t                 cidr;
   uint32_t                idx8;
   uint32_t                idx32;
   uint8_t                 b1;   // byte 1 (32 bits)
   uint8_t                 b2;   // byte 2 (32 bits)
   uint32_t                t1;   // tetra 1 (32 bits)
   uint32_t                t2;   // tetra 2 (32 bits)
   const netcalc_addr_t *  mask;

   cidr = (c1 < c2) ? c1 : c2;
   mask = &_netcalc_netmasks[cidr];

   for(idx32 = 0; (idx32 < 4); idx32++)
   {  t1 = mask->addr32[idx32] & a1->addr32[idx32];
      t2 = mask->addr32[idx32] & a2->addr32[idx32];
      if ( t1 != t2 )
      {  for(idx8 = (idx32*4); (idx8 < ((idx32*4)+4)); idx8++)
         {  b1 = mask->addr8[idx8] & a1->addr8[idx8];
            b2 = mask->addr8[idx8] & a2->addr8[idx8];
            if (b1 < b2)
               return(NETCALC_CMP_BEFORE);
            if (b1 > b2)
               return(NETCALC_CMP_AFTER);
         };
      };
   };

   if (c1 < c2)
      return(NETCALC_CMP_SUPERNET);
   if (c2 < c1)
      return(NETCALC_CMP_SUBNET);
   return(NETCALC_CMP_SAME);
}


int
netcalc_addr_convert_eui48(
         netcalc_addr_t *              addr,
         int                           family  )
{
   assert(addr != NULL);

   switch(family)
   {  case NETCALC_AF_EUI48:
         return(0);

      case NETCALC_AF_INET6:
      case NETCALC_AF_EUI64:
         if ( (addr->addr8[11] != 0xff) || (addr->addr8[12] != 0xfe) )
            return(NETCALC_ENOTSUP);
         addr->addr8[12] = addr->addr8[10];
         addr->addr8[11] = addr->addr8[9];
         addr->addr8[10] = (addr->addr8[8] ^ 0x02);
         addr->addr8[9]  = 0;
         addr->addr8[8]  = 0;
         addr->addr32[0] = 0;
         addr->addr32[1] = 0;
         return(0);

      case NETCALC_AF_INET:
         return(NETCALC_ENOTSUP);

      default:
         break;
   };

   return(NETCALC_ENOTSUP);
}


int
netcalc_addr_convert_eui64(
         netcalc_addr_t *              addr,
         int                           family  )
{
   assert(addr != NULL);
   switch(family)
   {  case NETCALC_AF_EUI48:
         addr->addr32[0] = 0;
         addr->addr32[1] = 0;
         addr->addr8[8]  = (addr->addr8[10] ^ 0x02);
         addr->addr8[9]  = addr->addr8[11];
         addr->addr8[10] = addr->addr8[12];
         addr->addr8[11] = 0xff;
         addr->addr8[12] = 0xfe;
         return(0);

      case NETCALC_AF_EUI64:
         return(0);

      case NETCALC_AF_INET:
         return(NETCALC_ENOTSUP);

      case NETCALC_AF_INET6:
         addr->addr32[0] = _netcalc_slaac_in6.net_addr.addr32[0];
         addr->addr32[1] = _netcalc_slaac_in6.net_addr.addr32[1];
         return(0);

      default:
         break;
   };
   return(NETCALC_ENOTSUP);
}


int
netcalc_addr_convert_inet(
         netcalc_addr_t *              addr,
         int                           family  )
{
   assert(addr != NULL);
   switch(family)
   {  case NETCALC_AF_EUI48:
         return(NETCALC_ENOTSUP);

      case NETCALC_AF_EUI64:
         return(NETCALC_ENOTSUP);

      case NETCALC_AF_INET:
         return(0);

      case NETCALC_AF_INET6:
         addr->addr32[0] = _netcalc_ipv4_mapped_ipv6.net_addr.addr32[0];
         addr->addr32[1] = _netcalc_ipv4_mapped_ipv6.net_addr.addr32[1];
         addr->addr32[2] = _netcalc_ipv4_mapped_ipv6.net_addr.addr32[2];
         return(0);

      default:
         break;
   };
   return(NETCALC_ENOTSUP);
}


int
netcalc_addr_convert_inet6(
         netcalc_addr_t *              addr,
         int                           family  )
{
   int      rc;

   assert(addr != NULL);

   switch(family)
   {  case NETCALC_AF_EUI48:
         if ((rc = netcalc_addr_convert_eui64(addr, family)) != NETCALC_SUCCESS)
            return(rc);

      case NETCALC_AF_EUI64:
         addr->addr64[0] = _netcalc_slaac_in6.net_addr.addr64[0];
         return(0);

      case NETCALC_AF_INET:
         addr->addr64[0] = _netcalc_ipv4_mapped_ipv6.net_addr.addr64[0];
         addr->addr32[2] = _netcalc_ipv4_mapped_ipv6.net_addr.addr32[2];
         return(0);

      case NETCALC_AF_INET6:
         return(0);

      default:
         break;
   };
   return(NETCALC_ENOTSUP);
}


/* end of source */
