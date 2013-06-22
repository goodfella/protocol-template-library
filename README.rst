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

* Bit order
* Field list

Protocol bit order
~~~~~~~~~~~~~~~~~~

A protocol's bit order defines the order in which its field's bits are
stored in a unsigned char buffer that represents the protocol's field
data.  There are three bit field types that can be assigned to a
protocol:

* Most significant bit first
* Least significant bit first
* Mixed bit order

In a most significant bit first protocol, a field's bits are stored
from most significant to least significant.

In a least significant protocol, a field's bits are stored from least
significant to most significant.

In a mixed bit order protocol, each field defines its own bit order.
In this case each field's bit order can be either most significant bit
first, or least significant bit first as described above.

Protocol field list
~~~~~~~~~~~~~~~~~~~

A protocol's field list defines the length and underlying type for
each field.  Fields must have a fixed size and currently must be no
more than 64 bits.
