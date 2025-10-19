
Network Calculations Library
============================


Copyright
=========

Network Calculations Library  
Copyright (C) 2025 David M. Syzdek <david@syzdek.net>.  
All rights reserved.  

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of David M. Syzdek nor the
     names of its contributors may be used to endorse or promote products
     derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.


Contents
--------

   * Overview
   * Utility
     - sort widget
     - tree widget
   * Maintainers
   * Source Code

Overview
==========

This package contains a library with miscellaneous functions for manipulating
and sorting IPv4, IPv6, EUI48, and EUI64 addresses.  The accompanying utility
allows some of the library functions to be accessed from the command line
or in shell scripts.

Utility
=======

The `netcalc` utility can be used on the command line to access some of the
functionality of the netcalc library.  The utility contains the following
widgets:

   * info         display IP address information
   * printf       display formatted IP address information
   * superblock   display superblocks containing IP addresses
   * sort         sorts networks
   * syntaxes     display supported address syntaxes
   * test         compares two IP addresses or networks
   * tree         sorts networks and display is a tree structure

Normally a widget is invoked by specifying the widget name on the command line.
The following example invokes the info widget:

    netcalc info 203.0.113.192/29
    Network        Broadcast      Netmask          Wildcard  CIDR  /32s
    203.0.113.192  203.0.113.199  255.255.255.248  0.0.0.7   29    8

If netcalc is copied or linked to another filename, then netcalc will attempt
to determine the widget name from the filename which was executed on the
command line.

Referencing the netcalc info widget using the 'netcalc-' prefix:

    ln -s netcalc netcalc-info
    ./netcalc-info 203.0.113.192/29
    Network        Broadcast      Netmask          Wildcard  CIDR  /32s
    203.0.113.192  203.0.113.199  255.255.255.248  0.0.0.7   29    8

Referencing the netcalc info widget using the 'netcalc' prefix:

    ln -s netcalc netcalcinfo
    ./netcalcinfo 203.0.113.192/29
    Network        Broadcast      Netmask          Wildcard  CIDR  /32s
    203.0.113.192  203.0.113.199  255.255.255.248  0.0.0.7   29    8

Referencing the netcalc info widget using the 'net' prefix:

    ln -s netcalc netinfo
    ./netinfo 203.0.113.192/29
    Network        Broadcast      Netmask          Wildcard  CIDR  /32s
    203.0.113.192  203.0.113.199  255.255.255.248  0.0.0.7   29    8


info widget
-----------

The info widget parses one or more addresses and displays information about
the address.  If more than one address was provided, the information about
the superblock network which contains all provided addresses will also be
displayed.  Additionally the info widget can also be used to convert between
address families.

Basic example usage:

    netcalc info 2001:db8:e:ffff::/64 2001:db8:d:f000::/64 2001:db8:d:ffff::/64 \
        2001:db8:d:fffe::/64 2001:db8:d:100::/64  2001:db8:d:101::/64 \
        2001:db8:d:100::/56 2001:db8:d::/48 2001:db8:d::/56 2001:db8:d:a::/64 \
        2001:db8:e:a::/64 2001:db8:f:ffff::/64 2001:db8::/64 \
        2001:db8:d:101::1/128  2001:db8:0:e::203.0.113.198/127
    Address                      Network                  Broadcast                            Netmask                                  Wildcard                      CIDR  /64s
    2001:db8:e:ffff::/64         2001:db8:e:ffff::        2001:db8:e:ffff:ffff:ffff:ffff:ffff  ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8:d:f000::/64         2001:db8:d:f000::        2001:db8:d:f000:ffff:ffff:ffff:ffff  ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8:d:ffff::/64         2001:db8:d:ffff::        2001:db8:d:ffff:ffff:ffff:ffff:ffff  ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8:d:fffe::/64         2001:db8:d:fffe::        2001:db8:d:fffe:ffff:ffff:ffff:ffff  ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8:d:100::/64          2001:db8:d:100::         2001:db8:d:100:ffff:ffff:ffff:ffff   ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8:d:101::/64          2001:db8:d:101::         2001:db8:d:101:ffff:ffff:ffff:ffff   ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8:d:100::/56          2001:db8:d:100::         2001:db8:d:1ff:ffff:ffff:ffff:ffff   ffff:ffff:ffff:ff00::                    ::ff:ffff:ffff:ffff:ffff      56    256
    2001:db8:d::/48              2001:db8:d::             2001:db8:d:ffff:ffff:ffff:ffff:ffff  ffff:ffff:ffff::                         ::ffff:ffff:ffff:ffff:ffff    48    65536
    2001:db8:d::/56              2001:db8:d::             2001:db8:d:ff:ffff:ffff:ffff:ffff    ffff:ffff:ffff:ff00::                    ::ff:ffff:ffff:ffff:ffff      56    256
    2001:db8:d:a::/64            2001:db8:d:a::           2001:db8:d:a:ffff:ffff:ffff:ffff     ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8:e:a::/64            2001:db8:e:a::           2001:db8:e:a:ffff:ffff:ffff:ffff     ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8:f:ffff::/64         2001:db8:f:ffff::        2001:db8:f:ffff:ffff:ffff:ffff:ffff  ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8::/64                2001:db8::               2001:db8:0:0:ffff:ffff:ffff:ffff     ffff:ffff:ffff:ffff::                    ::ffff:ffff:ffff:ffff         64    1
    2001:db8:d:101::1            2001:db8:d:101::1        2001:db8:d:101:0:0:0:1               ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff  ::                            128   n/a
    2001:db8:0:e::cb00:71c6/127  2001:db8:0:e::cb00:71c6  2001:db8:0:e:0:0:cb00:71c7           ffff:ffff:ffff:ffff:ffff:ffff:ffff:fffe  ::1                           127   n/a
    SUPERBLOCK                   2001:db8::               2001:db8:f:ffff:ffff:ffff:ffff:ffff  ffff:ffff:fff0::                         ::f:ffff:ffff:ffff:ffff:ffff  44    1048576

Advanced example IPv4 usage:

    netcalc info -vv 203.0.113.198/29 
    203.0.113.198/29 (IPv4)
    -----------------------
    Host Address:        203.0.113.198
    Network Address:     203.0.113.192
    Broadcast Address:   203.0.113.199
    Netmask:             255.255.255.248
    Wildcard:            0.0.0.7
    CIDR:                29
    Usable Range:        203.0.113.193
                         203.0.113.198
    DNS ARPA Host:       198.113.0.203.in-addr.arpa.
    DNS ARPA Zone:       113.0.203.in-addr.arpa.
    DNS ARPA RR:         198 IN PTR

Advanced example IPv6 usage:

    netcalc info -vv 2001:db8:0:e::203.0.113.198/127
    2001:db8:0:e::203.0.113.198/127 (IPv6)
    --------------------------------------
    Host Address:        2001:db8:0:e::cb00:71c6
    IPv4-mapped Address: 2001:db8:0:e::203.0.113.198
    Network Address:     2001:db8:0:e::cb00:71c6
    Broadcast Address:   2001:db8:0:e:0:0:cb00:71c7
    Netmask:             ffff:ffff:ffff:ffff:ffff:ffff:ffff:fffe
    Wildcard:            ::1
    CIDR:                127
    Usable Range:        2001:db8:0:e:0:0:cb00:71c6
                         2001:db8:0:e:0:0:cb00:71c7
    DNS ARPA Host:       6.c.1.7.0.0.b.c.0.0.0.0.0.0.0.0.e.0.0.0.0.0.0.0.8.b.d.0.1.0.0.2.ip6.arpa.
    DNS ARPA Zone:       c.1.7.0.0.b.c.0.0.0.0.0.0.0.0.e.0.0.0.0.0.0.0.8.b.d.0.1.0.0.2.ip6.arpa.
    DNS ARPA RR:         6 IN PTR


sort widget
-----------

The sort widget uses the set family of functions to sort the list of networks.

Example usage:

    netcalc sort 2001:db8:e:ffff::/64 2001:db8:d:f000::/64 2001:db8:d:ffff::/64 \
       2001:db8:d:fffe::/64 2001:db8:d:100::/64  2001:db8:d:101::/64 \
       2001:db8:d:100::/56 2001:db8:d::/48 2001:db8:d::/56 2001:db8:d:a::/64 \
       2001:db8:e:a::/64 2001:db8:f:ffff::/64 2001:db8::/64 2001:db8:d:101::1/128 \
       ::/1 ::/0 8000::/1 2001:db8::/32 203.0.113.198/31 203.0.113.0/24 \
       203.0.113.192/29 203.0.113.196/30 203.0.113.199/32
       1: ::/0
       2: ::/1
       3: 203.0.113.0/24
       4: 203.0.113.192/29
       5: 203.0.113.196/30
       6: 203.0.113.198/31
       7: 203.0.113.199
       8: 2001:db8::/32
       9: 2001:db8::/64
      10: 2001:db8:d::/48
      11: 2001:db8:d::/56
      12: 2001:db8:d:a::/64
      13: 2001:db8:d:100::/56
      14: 2001:db8:d:100::/64
      15: 2001:db8:d:101::/64
      16: 2001:db8:d:101::1
      17: 2001:db8:d:f000::/64
      18: 2001:db8:d:fffe::/64
      19: 2001:db8:d:ffff::/64
      20: 2001:db8:e:a::/64
      21: 2001:db8:e:ffff::/64
      22: 2001:db8:f:ffff::/64
      23: 8000::/1


tree widget
-----------

The tree widget uses the set family of functions to sort the list of networks
and display the sorted networks in a hierarchy of subnets. 

Example usage:

    netcalc tree 2001:db8:e:ffff::/64 2001:db8:d:f000::/64 2001:db8:d:ffff::/64 \
       2001:db8:d:fffe::/64 2001:db8:d:100::/64  2001:db8:d:101::/64 \
       2001:db8:d:100::/56 2001:db8:d::/48 2001:db8:d::/56 2001:db8:d:a::/64 \
       2001:db8:e:a::/64 2001:db8:f:ffff::/64 2001:db8::/64 2001:db8:d:101::1/128 \
       ::/1 ::/0 8000::/1 2001:db8::/32 203.0.113.198/31 203.0.113.0/24 \
       203.0.113.192/29 203.0.113.196/30 203.0.113.199/32
        1. ::/0
        2.    ::/1
        3.       203.0.113.0/24
        4.          203.0.113.192/29
        5.             203.0.113.196/30
        6.                203.0.113.198/31
        7.                   203.0.113.199
        8.       2001:db8::/32
        9.          2001:db8::/64
       10.          2001:db8:d::/48
       11.             2001:db8:d::/56
       12.                2001:db8:d:a::/64
       13.             2001:db8:d:100::/56
       14.                2001:db8:d:100::/64
       15.                2001:db8:d:101::/64
       16.                   2001:db8:d:101::1
       17.             2001:db8:d:f000::/64
       18.             2001:db8:d:fffe::/64
       19.             2001:db8:d:ffff::/64
       20.          2001:db8:e:a::/64
       21.          2001:db8:e:ffff::/64
       22.          2001:db8:f:ffff::/64
       23.    8000::/1


Maintainers
===========

* David M. Syzdek <david@syzdek.net>


Source Code
===========

The source code for this project is maintained using git (http://git-scm.com).
The following contains information to checkout the source code from the git
repository.

Browse Source:

   * https://github.com/syzdek/netcalc.git

Git URLs:

   * https://github.com/syzdek/netcalc.git
   * git@github.com:syzdek/netcalc.git

Preparing Source:

    $ git clone https://github.com/syzdek/netcalc.git
    $ cd netcalc
    $ ./autogen.sh


Package Maintence Notes
=======================

This is a collection of notes for developers to use when maintaining this
package.

New Release Checklist:

   * Switch to 'master' branch in Git repository.
   * Update version in configure.ac.
   * Update date and version in ChangeLog.
   * Commit configure.ac and ChangeLog changes to repository.
   * Create tag in git repository:
   
         $ git tag -s v${MAJOR}.${MINOR}
       
   * Push repository to publishing server:
   
         $ git push --tags origin master:master next:next pu:pu

 Creating Source Distribution Archives:

    $ ./configure
    $ make update
    $ make distcheck
    $ make dist-bzip2
    $ make dist-xz

