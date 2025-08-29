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
#define __LIB_LIBNETCALC_LSETS_C 1
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

const char *
netcalc_strerror(
         int                           errnum )
{
   switch(errnum)
   {  case NETCALC_SUCCESS:      return("success");

      case NETCALC_EBADADDR:     return("bad address string");
      case NETCALC_EBUFFLEN:     return("buffer length exceeeded");
      case NETCALC_EFIELD:       return("unknown or unsupported field");
      case NETCALC_EINVAL:       return("invalid argument");
      case NETCALC_ENOMEM:       return("out of virtual memory");
      case NETCALC_ENOTSUP:      return("operation not supported");
      case NETCALC_ETYPE:        return("not of type");

      case NETCALC_EUNKNOWN:     break;
      default:                   break;
   };
   return("unknown error");
}


size_t
netcalc_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize )
{
   size_t      len;

   assert(src     != NULL);
   assert(dstsize  > 0);

   for(len = 0; ((*dst)); len++, dst++);
   if (!(src))
      return(len);

   dstsize--;
   for(; ( (len < dstsize) && ((*dst = *src)) ); len++, dst++, src++);
   *dst = '\0';

   for(; ((*src)); len++, src++);

   return(len);
}


size_t
netcalc_strlcpy(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize )
{
   size_t      len;

   assert(dst     != NULL);
   assert(src     != NULL);
   assert(dstsize  > 0);

   dstsize--;
   for(len = 0; ( (len < dstsize) && ((*dst = *src)) ); len++, dst++, src++);
   *dst = '\0';

   for(; ((*src)); len++, src++);

   return(len);
}


/* end of source */
