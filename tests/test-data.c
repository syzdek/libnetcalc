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
#define __TESTS_NETCALC_DATA_C 1
#include "libnetcalc-tests.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <netcalc.h>


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

//testdata_t test_data[] =
testdata_t test_data[] =
{
   // EUI48
   {  .addr_str         = "b2:be:e2:af:07:74",
      .addr_flgs        = NETCALC_AF_EUI48,
      .addr_port        = MY_NOPORT,
      .addr_cidr        = MY_NOCIDR,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "b2be.e2af.0774",
      .addr_flgs        = NETCALC_AF_EUI48,
      .addr_port        = MY_NOPORT,
      .addr_cidr        = MY_NOCIDR,
      .addr_fail        = MY_PASS
   },

   // EUI64
   {  .addr_str         = "b2:be:e2:af:07:74:89:11",
      .addr_flgs        = NETCALC_AF_EUI64,
      .addr_port        = MY_NOPORT,
      .addr_cidr        = MY_NOCIDR,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "b2be.e2af.0774.8911",
      .addr_flgs        = NETCALC_AF_EUI64,
      .addr_port        = MY_NOPORT,
      .addr_cidr        = MY_NOCIDR,
      .addr_fail        = MY_PASS
   },

   // IPv4
   {  .addr_str         = "209.193.4.8",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = 0,
      .addr_cidr        = 32,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "209.193.4.8/32:5432",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = MY_NOPORT,
      .addr_cidr        = MY_NOCIDR,
      .addr_fail        = MY_FAIL
   },
   {  .addr_str         = "209.193.4.8:5432/32",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = MY_NOPORT,
      .addr_cidr        = MY_NOCIDR,
      .addr_fail        = MY_FAIL
   },
   {  .addr_str         = "209.193.4.8/32",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = 0,
      .addr_cidr        = 32,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "209.193.4.8/31",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = 0,
      .addr_cidr        = 31,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "209.193.4.8/30",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = 0,
      .addr_cidr        = 30,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "209.193.4.8/29",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = 0,
      .addr_cidr        = 29,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "209.193.4.8/28",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = 0,
      .addr_cidr        = 28,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "209.193.4.7",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = 0,
      .addr_cidr        = 32,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "209.193.4.7/32",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = 0,
      .addr_cidr        = 32,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "209.193.4.7:123",
      .addr_flgs        = NETCALC_AF_INET,
      .addr_port        = 123,
      .addr_cidr        = 32,
      .addr_fail        = MY_PASS
   },

   // IPv6
   {  .addr_str         = "fe80::d55c:5431:458c:e98d%utun1",
      .addr_flgs        = NETCALC_AF_INET6,
      .addr_port        = 0,
      .addr_cidr        = 128,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "fe80::d55c:5431:458c:e98d/64%utun1",
      .addr_flgs        = NETCALC_AF_INET6,
      .addr_port        = 0,
      .addr_cidr        = 64,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "fe80::d55c:5431:458c:e98d/64",
      .addr_flgs        = NETCALC_AF_INET6,
      .addr_port        = 0,
      .addr_cidr        = 64,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "fe80:0:0:0:d55c:5431:458c:e98d/64",
      .addr_flgs        = NETCALC_AF_INET6,
      .addr_port        = 0,
      .addr_cidr        = 64,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "2001:4948::/32",
      .addr_flgs        = NETCALC_AF_INET6,
      .addr_port        = 0,
      .addr_cidr        = 32,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "2001:4948:e::/32",
      .addr_flgs        = NETCALC_AF_INET6,
      .addr_port        = 0,
      .addr_cidr        = 32,
      .addr_fail        = MY_PASS
   },
   {  .addr_str         = "::1",
      .addr_flgs        = NETCALC_AF_INET6,
      .addr_port        = 0,
      .addr_cidr        = 128,
      .addr_fail        = MY_PASS
   },

   // end of data
   {  .addr_str         = NULL,
      .addr_flgs        = 0,
      .addr_port        = 0,
      .addr_cidr        = 0,
      .addr_fail        = 0
   },
};


/* end of source */
