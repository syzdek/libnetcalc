
Address Formats
===============

EUI-48
------

  * Address Styles
    - Colon Delimiter:  01:23:45:67:89:ab
    - Dash Delimiter:   01-23-45-67-89-ab
    - Dot Delimiter:    0123.4567.89ab
    - No Delimiter:     0123456789ab


EUI-64
------

  * Address Styles
    - Colon Delimiter:  01:23:45:67:89:ab:cd:ef
    - Dash Delimiter:   01-23-45-67-89-ab-cd-ef
    - Dot Delimiter:    0123.4567.89ab.cdef
    - No Delimiter:     0123456789abcdef


IPv4 Formats
------------

  * Formats
    - <address>
    - <address>/<cidr>
    - <address>:<port>
    - <address>/<cidr>:<port>

  * Address Styles
    - Full Address:                203.000.113.001
    - Zero Suppression:            203.0.113.1

  * Examples

    * Any
      - 0.0.0.0/0             (zero suppression)
      - 000.000.000.000/0

    * Localhost
      - 127.0.0.1/8           (zero suppression with CIDR)
      - 127.000.000.001/8

    * Plain Address
      - 203.0.113.1           (zero suppression)
      - 203.000.113.001

    * With CIDR
      - 203.0.113.1/29        (zero suppression)
      - 203.000.113.001/29

    * With Port
      - 203.0.113.1:80        (zero suppression)
      - 203.000.113.001:80

    * With Port and CIDR
      - 203.0.113.1/29:80     (zero suppression)
      - 203.000.113.001/29:80


IPv6 Formats
------------

  * Formats
    - <address>
    - <address>/<cidr>
    - <address>%<iface>
    - [<address>]
    - [<address>]/<cidr>
    - [<address>]:<port>
    - [<address>]/<cidr>:<port>
    - [<address>%<iface>]
    - [<address>%<iface>]/<cidr>
    - [<address>%<iface>]:<port>
    - [<address>%<iface>]/<cidr>:<port>

  * Address Styles
    - Full Address:                2001:0db8:000f:0300:0000:0000:0000:054f
    - Zero Compression:            2001:0db8:000f:0300::054f
    - Zero Suppression:            2001:db8:f:300:0:0:0:54f
    - Compression and Suppression: 2001:db8:f:300::54f

  * Examples

     * Any With CIDR
       - ::/0                                                  (zero compression)
       - 0:0:0:0:0:0:0:0/0                                     (zero suppression)
       - 0000:0000:0000:0000:0000:0000:0000:0000/0             (full addresss)

     * Localhost With CIDR
       - ::1/128                                               (zero commpression and suppression)
       - ::0001/128                                            (zero compression)
       - 0:0:0:0:0:0:0:1/128                                   (zero suppression)
       - 0000:0000:0000:0000:0000:0000:0000:0001/128           (full addresss)

     * Localhost
       - ::1                                                   (zero commpression and suppression)
       - ::0001                                                (zero compression)
       - 0:0:0:0:0:0:0:1                                       (zero suppression)
       - 0000:0000:0000:0000:0000:0000:0000:0001               (full address)

     * With IFACE
       - fe80::216:3eff:fefe:b381%eth0                         (zero commpression and suppression)
       - fe80::0216:3eff:fefe:b381%eth0                        (zero compression)
       - fe80:0:0:0:216:3eff:fefe:b381%eth0                    (zero suppression)
       - fe80:0000:0000:0000:0216:3eff:fefe:b381%eth0          (full address)

     * With IFACE and CIDR
       - [fe80::216:3eff:fefe:b381%eth0]/64                    (zero commpression and suppression)
       - [fe80::0216:3eff:fefe:b381%eth0]/64                   (zero compression)
       - [fe80:0:0:0:216:3eff:fefe:b381%eth0]/64               (zero suppression)
       - [fe80:0000:0000:0000:0216:3eff:fefe:b381%eth0]/64     (full address)

     * With IFACE and Port
       - [fe80::216:3eff:fefe:b381%eth0]:80                    (zero commpression and suppression)
       - [fe80::0216:3eff:fefe:b381%eth0]:80                   (zero compression)
       - [fe80:0:0:0:216:3eff:fefe:b381%eth0]:80               (zero suppression)
       - [fe80:0000:0000:0000:0216:3eff:fefe:b381%eth0]:80     (full address)

     * With IFACE, CIDR, and Port
       - [fe80::216:3eff:fefe:b381%eth0]/64:80                 (zero commpression and suppression)
       - [fe80::0216:3eff:fefe:b381%eth0]/64:80                (zero compression)
       - [fe80:0:0:0:216:3eff:fefe:b381%eth0]/64:80            (zero suppression)
       - [fe80:0000:0000:0000:0216:3eff:fefe:b381%eth0]/64:80  (full address)

     * Address With CIDR
       - 2001:db8::/32                                         (zero commpression and suppression)
       - 2001:0db8::/32                                        (zero compression)
       - 2001:db8:0:0:0:0:0:0/32                               (zero suppression)
       - 2001:0db8:0000:0000:0000:0000:0000:0000/32            (full address)

     * Plain Address
       - 2001:db8:f:300::54f                                   (zero commpression and suppression)
       - 2001:0db8:000f:0300::054f                             (zero compression)
       - 2001:db8:f:300:0:0:0:54f                              (zero suppression)
       - 2001:0db8:000f:0300:0000:0000:0000:054f               (full address)

     * Address with CIDR
       - 2001:db8:f:300::54f/64                                (zero commpression and suppression)
       - 2001:0db8:000f:0300::054f/64                          (zero compression)
       - 2001:db8:f:300:0:0:0:54f/64                           (zero suppression)
       - 2001:0db8:000f:0300:0000:0000:0000:054f/64            (full address)

     * Bracketed Address
       - [2001:db8:f:300::54f]                                 (zero commpression and suppression)
       - [2001:0db8:000f:0300::054f]                           (zero compression)
       - [2001:db8:f:300:0:0:0:54f]                            (zero suppression)
       - [2001:0db8:000f:0300:0000:0000:0000:054f]             (full address)

     * Bracketed Address with CIDR
       - [2001:db8:f:300::54f]/64                              (zero commpression and suppression)
       - [2001:0db8:000f:0300::054f]/64                        (zero compression)
       - [2001:db8:f:300:0:0:0:54f]/64                         (zero suppression)
       - [2001:0db8:000f:0300:0000:0000:0000:054f]/64          (full address)

     * Bracketed Address with Port
       - [2001:db8:f:300::54f]:80                              (zero commpression and suppression)
       - [2001:0db8:000f:0300::054f]:80                        (zero compression)
       - [2001:db8:f:300:0:0:0:54f]:80                         (zero suppression)
       - [2001:0db8:000f:0300:0000:0000:0000:054f]:80          (full address)

     * Bracketed Address with CIDR and Port
       - [2001:db8:f:300::54f]/64:80                           (zero commpression and suppression)
       - [2001:0db8:000f:0300::054f]/64:80                     (zero compression)
       - [2001:db8:f:300:0:0:0:54f]/64:80                      (zero suppression)
       - [2001:0db8:000f:0300:0000:0000:0000:054f]/64:80       (full address)


