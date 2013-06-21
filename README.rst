Introduction
============

A header library that aids in the parsing of binary protocols.  This
library uses C++11 features, so its usage requires a compliant C++11
compiler.

Building
========

protocol-helper is a header library, so it doesn't require any
building to use; however, the examples are built with CMake.  The
examples can be built by issuing the following commands at the top of
the build tree::

 mkdir build-dir
 cd build-dir
 cmake ..
 make

Defining Protocols
==================

Protocols in this library are defined by two attributes:

* Endianess
* Field list

Protocol endianess
~~~~~~~~~~~~~~~~~~

A protocol's endianess defines the order in which its field's bits are
stored in a unsigned char buffer that represents the protocol's field
data.  There are three endianess types that can be assigned to a
protocol:

* Big endian
* Little endian
* Mixed endian

In a big endian protocol, a field's bits are stored from most
significant to least significant.

In a little endian protocol, a field's bits are stored from least
significant to most significant.

In a mixed endian protocol, each field defines its own endianess.  In
this case each field's endianess can be either big endian, or little
endian as described above.

Protocol field list
~~~~~~~~~~~~~~~~~~~

A protocol's field list defines the length and underlying type for
each field.  Fields must have a fixed size and currently must be no
more than 64 bits.
