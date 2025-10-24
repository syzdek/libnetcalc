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
#define __TESTS_NETCALC_SET_DATA_C 1
#include "libnetcalc-tests.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <stdint.h>

#include <netcalc.h>


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

const char *      test_set_data[] =
{  "2001:db8:e:ffff::/64",
   "2001:db8:d:f000::/64",
   "2001:db8:d:ffff::/64",
   "2001:db8:d:fffe::/64",
   "2001:db8:d:100::/64",
   "2001:db8:d:101::/64",
   "2001:db8:d:100::/56",
   "2001:db8:d::/48",
   "2001:db8:d::/56",
   "2001:db8:d:a::/64",
   "2001:db8:e:a::/64",
   "2001:db8:f:ffff::/64",
   "2001:db8::/64",
   "2001:db8:d:101::1/128",
   "::/1",
   "::/0",
   "8000::/1",
   "2001:db8::/32",
   "203.0.113.198/31",
   "203.0.113.0/24",
   "203.0.113.192/29",
   "203.0.113.196/30",
   "203.0.113.199/32",
//      "00:16:3e:15:70:92",
//      "00:16:3e:06:e5:9f",
   NULL
};


/* end of source */
