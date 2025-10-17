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
 *  src/netcalc-utility.h - common includes and prototypes
 */
#ifndef __SRC_NETCALC_UTILITY_H
#define __SRC_NETCALC_UTILITY_H 1

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

#ifndef PROGRAM_NAME
#   define PROGRAM_NAME "netcalc"
#endif
#ifndef PACKAGE_BUGREPORT
#   define PACKAGE_BUGREPORT ""
#endif
#ifndef PACKAGE_COPYRIGHT
#   define PACKAGE_COPYRIGHT "Copyright (C) 2025 David M. Syzdek <david@syzdek.net>."
#endif
#ifndef PACKAGE_NAME
#   define PACKAGE_NAME "Network Calculations Library"
#endif
#ifndef PACKAGE_TARNAME
#   define PACKAGE_TARNAME "libnetcalc"
#endif
#ifndef PACKAGE_VERSION
#   define PACKAGE_VERSION ""
#endif


#undef  MY_FLG_SHOW_ADDR
#define MY_FLG_SHOW_ADDR      NETCALC_FLG_USR1


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types

typedef struct _my_config     my_config_t;
typedef struct _my_length     my_len_t;
typedef struct _my_record     my_rec_t;
typedef struct _my_widget     my_widget_t;


struct _my_config
{  int                        quiet;
   int                        verbose;
   int                        symlinked;
   int                        flags;
   int                        flags_negate;
   int                        net_prefix_family;
   int                        __pad_int;
   int                        argc;
   char * const *             argv;
   const char *               prog_name;
   netcalc_net_t *            net_prefix;
   const char *               net_prefix_str;
   const my_widget_t *        widget;
};


struct _my_length
{  int                  address;
   int                  ip_broadcast;
   int                  ip_netmask;
   int                  ip_network;
   int                  ip_wildcard;
};


struct _my_record
{  netcalc_net_t *      net;
   int                  family;
   int                  ip_superblock;
   int                  ip_cidr;
   int                  __int_pad;
   char                 address[NETCALC_ADDRESS_LENGTH];
   char                 ip_broadcast[NETCALC_ADDRESS_LENGTH];
   char                 ip_cidr_str[NETCALC_ADDRESS_LENGTH];
   char                 ip_netmask[NETCALC_ADDRESS_LENGTH];
   char                 ip_network[NETCALC_ADDRESS_LENGTH];
   char                 ip_subnets[NETCALC_ADDRESS_LENGTH];
   char                 ip_wildcard[NETCALC_ADDRESS_LENGTH];
};


struct _my_widget
{  const char *               name;
   const char *               desc;
   const char *               usage;
   const char *               short_opt;
   const char * const *       aliases;
   const struct option *      long_opt;
   int                        arg_min;
   int                        arg_max;
   int                        alias_idx;
   int                        padint;
   int  (*func_exec)(my_config_t * cnf);
   int  (*func_usage)(my_config_t * cnf);
   int  (*func_args)(my_config_t * cnf, int argc, char * const * argv );
};


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

extern void
my_nets_free(
         netcalc_net_t **              nets );


extern char *
my_prog_name(
         my_config_t *                 cnf );


// function in is in libnetcalc.a
extern size_t
netcalc_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


// function in is in libnetcalc.a
extern size_t
netcalc_strlcpy(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


//-------------------//
// record prototypes //
//-------------------//
#pragma mark record prototypes

extern void
my_rec_length(
         my_rec_t *                   rec,
         my_len_t *                    l );


extern void
my_rec_summary_ip(
         my_rec_t *                    r,
         my_len_t *                    l,
         int                           flags );


extern void
my_rec_process(
         my_config_t *                 cnf,
         my_rec_t *                    rec );


extern my_rec_t **
my_recs_alloc(
         my_config_t *                 cnf,
         size_t                        len );


extern void
my_recs_free(
         my_rec_t **                   recs );


extern void
my_recs_lengths(
         my_rec_t **                   recs,
         my_len_t *                    l );


//--------------------//
// widgets prototypes //
//--------------------//
#pragma mark widgets prototypes

extern int
my_widget_test(
         my_config_t *                 cnf );


extern int
my_widget_test_arguments(
         my_config_t *                 cnf,
         int                           argc,
         char * const *                argv );


extern int
my_widget_test_usage(
         my_config_t *                 cnf );


extern int
my_widget_debug(
         my_config_t *                 cnf );


extern int
my_widget_info(
         my_config_t *                 cnf );


extern int
my_widget_printf(
         my_config_t *                 cnf );


extern int
my_widget_printf_usage(
         my_config_t *                 cnf );


extern int
my_widget_sort(
         my_config_t *                 cnf );


extern int
my_widget_superblock(
         my_config_t *                 cnf );


extern int
my_widget_tree(
         my_config_t *                 cnf );


#endif /* end of header */

