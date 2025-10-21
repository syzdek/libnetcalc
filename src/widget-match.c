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
#define __SRC_WIDGET_MATCH_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include "netcalc-utility.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>


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



//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
my_widget_match(
         my_config_t *                 cnf )
{
   int                  rc;
   char *               comment;
   netcalc_set_t *      ns;
   netcalc_net_t *      res;
   const char *         addr;

   if ( (cnf->argc < 2) && (!(cnf->in_filename)) )
   {  cnf->in_filename  = "-";
      cnf->in_fd        = STDIN_FILENO;
   };

   // initializes set
   if ((rc = netcalc_set_init(&ns, 0)) != 0)
   {  printf("%s: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
      return(1);
   };

   // import set from file and cli arguments
   if ((my_set_import(cnf, ns)))
   {  netcalc_set_free(ns);
      return(1);
   };

   // search set for matching record
   if ((rc = netcalc_set_query_str(ns, cnf->argv[0], &res, &comment, NULL, NULL)) != 0)
   {  fprintf(stderr, "%s: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
      netcalc_set_free(ns);
      return(1);
   };

   addr = netcalc_ntop(res, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
   if ((comment))
      printf("%s  %s\n", addr, comment);
   else
      printf("%s\n", addr);

   if ((comment))
      free(comment);
   netcalc_free(res);
   netcalc_set_free(ns);

   return(0);
}


/* end of source */
