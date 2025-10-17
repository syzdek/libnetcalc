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
#define __SRC_WIDGET_TREE_C 1


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

static void
my_widget_tree_print(
         my_config_t *                 cnf,
         netcalc_net_t *               net,
         int                           count,
         int                           depth,
         int                           maxdepth,
         size_t                        maxlen,
         const char *                  comment );


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
my_widget_tree(
         my_config_t *                 cnf )
{
   int                  idx;
   int                  rc;
   int                  depth;
   int                  maxdepth;
   char *               comment;
   const char *         addr;
   size_t               maxlen;
   size_t               len;
   netcalc_set_t *      ns;
   netcalc_net_t *      net;
   netcalc_cur_t *      cur;

   // initializes set
   if ((rc = netcalc_set_init(&ns, 0)) != 0)
   {  printf("%s: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
      return(1);
   };

   // process network arguments
   for(idx = 0; (idx < cnf->argc); idx++)
   {  if ((rc = netcalc_init(&net, cnf->argv[idx], cnf->flags)) != NETCALC_SUCCESS)
      {  fprintf(stderr, "%s: %s: %s\n", my_prog_name(cnf), cnf->argv[idx], netcalc_strerror(rc));
         netcalc_set_free(ns);
         return(1);
      };
      if ((rc = netcalc_set_add(ns, net, NULL, NULL, 0)) != 0)
      {  fprintf(stderr, "%s: %s: %s\n", my_prog_name(cnf), cnf->argv[idx], netcalc_strerror(rc));
         netcalc_set_free(ns);
         return(1);
      };
      netcalc_free(net);
   };

   netcalc_set_maxdepth(ns, &maxdepth);

   // initialize cursor
   if ((rc = netcalc_cur_init(ns, &cur)) != 0)
   {  fprintf(stderr, "%s: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
      netcalc_set_free(ns);
      return(1);
   };

   // determine max length of address
   if ((rc = netcalc_cur_first(cur, &net, NULL, NULL, NULL, NULL)) != 0)
   {  fprintf(stderr, "%s: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
      netcalc_cur_free(cur);
      netcalc_set_free(ns);
      return(1);
   };
   addr   = netcalc_ntop(net, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
   maxlen = strlen(addr);
   if ((net))
      netcalc_free(net);;
   while((rc = netcalc_cur_next(cur, &net, NULL, NULL, NULL, NULL)) == 0)
   {  addr     = netcalc_ntop(net, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
      len      = strlen(addr);
      maxlen   = (len > maxlen) ? len : maxlen;
      if ((net))
         netcalc_free(net);
   };

   // print records
   idx = 1;
   if ((rc = netcalc_cur_first(cur, &net, &comment, NULL, NULL, &depth)) != 0)
   {  fprintf(stderr, "%s: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
      netcalc_cur_free(cur);
      netcalc_set_free(ns);
      return(1);
   };
   my_widget_tree_print(cnf, net, idx, depth, maxdepth, maxlen, comment);
   if ((net))
      netcalc_free(net);
   if ((comment))
      free(comment);
   while((rc = netcalc_cur_next(cur, &net, &comment, NULL, NULL, &depth)) == 0)
   {  idx++;
      my_widget_tree_print(cnf, net, idx, depth, maxdepth, maxlen, comment);
      if ((net))
         netcalc_free(net);
      if ((comment))
         free(comment);
   };


   netcalc_cur_free(cur);
   netcalc_set_free(ns);

   return(0);
}


void
my_widget_tree_print(
         my_config_t *                 cnf,
         netcalc_net_t *               net,
         int                           count,
         int                           depth,
         int                           maxdepth,
         size_t                        maxlen,
         const char *                  comment )
{
   char        addr[NETCALC_ADDRESS_LENGTH];
   char        str[NETCALC_ADDRESS_LENGTH*3];

   assert(cnf != NULL);
   assert(net != NULL);

   netcalc_ntop(net, addr, sizeof(addr), NETCALC_TYPE_ADDRESS, cnf->flags);

   maxdepth = (maxdepth * 3) + (int)maxlen;

   if ((comment))
   {  snprintf(str, sizeof(str), "%*s%s", (depth*3), "", addr);
      printf("%5i. %-*s %s\n", count, maxdepth, str, comment);
   } else
   {  snprintf(str, sizeof(str), "%*s%s", (depth*3), "", addr);
      printf("%5i. %s\n", count, str);
   };

   return;
}


/* end of source */
