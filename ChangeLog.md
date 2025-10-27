
Network Calculations Library  
Copyright (C) 2025 David M. Syzdek <david@syzdek.net>.  
All rights reserved.  

0.6
   * libnetcalc:
     - adding IPv6 prefix to EUI-48, EUI-64, and IPv4 addresses

0.5 2025-10-23
   * libnetcalc:
     - fixing netcalc_set_query() handling of NETCALC_IDX_INSERT
     - adding support for escape characters in netcalc_strfnet()
     - adding ability to restrict address sets to specific superblock
     - fixing error checks for out of bounds CIDR
   * netcalc
     - general: adding man page
     - general: updating warning message format for network set imports
     - general: fixing CLI argument processing
     - general: removing ability to specify addresses sets as CLI argumetns
     - general: skipping blank lines when importing address sets
     - general: defaulting to STDIN if import file is not specified
     - general: adding missing CLI option definitions and usages
     - general: adding ability to restrict address set to specific superblock
     - info: adding man page
     - match: adding ability to search for more than one address
     - printf: adding man page
     - test: adding man page

0.4 2025-10-20
   * libnetcalc:
     - adding netcalc_set_query()
     - adding netcalc_set_query_str()
   * netcalc
     - general: adding ability to suppress errors when importing network sets
     - match: adding widget

0.3 2025-10-20
   * autotools
     - adding missing autoconf checks
   * netcalc
     - removing duplicate code
     - fixing processing of CIDR in netcalc_strfnet()
     - updating error message in set import functions

0.2 2025-10-20
   * docs:
     - adding TODO
     - adding examples to README
   * libnetcalc
     - changed default CIDR to 128 for IPv6 converted from EUI48 and EUI64.
     - replaced calls to strncpy() with internal function which
       mimics the behavior of BSD strlcpy().
     - refactored network conversion functions
     - refactored network set functions
     - fixing functions for converting between families
   * netcalc
     - general: adding the prefix 'net' for linked widgets
     - general: fixing handling of -p argument
     - general: adding -C option for converting families without prefix
     - sort widget: adding widget
     - test widget: cleaning up usage output
     - tree widget: adding widget

0.1 2025-08-06
   - initial alpha release

0.0 2025-02-17
   - starting project (syzdek)

