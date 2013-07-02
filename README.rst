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

Usage Restrictions
~~~~~~~~~~~~~~~~~~

The current byte order handling of fields requires that the sender and
receiver of the packed data agree on the number of bits per unsigned
char.  This is because the number of bits per unsigned char dictates
the field size at which the byte order of a field needs to be handled.
For example, there's no need to handle the byte order for a 16 bit
field on a system where an unsigned char is 16 bits.  On that system,
a 16 bit integer can be transfered without any byte swaping because
only one byte is required to send the integer.

If that 16 bit field was then transmited from a system with a byte
that's 16 bits to a system where a byte is 8 bits, the receiving
system would not know how to interpret the two bytes that make up the
16 bit field because the system from which the field data originated
sent the data in its native endianess.

A similar problem exists when the source system has less bits per
unsigned char than the destination system.  In this scenario, the
destination system is not able to undo the byte swapping performed by
the source system because it's unable to address the first and last 8
bits of the field.

Defining Protocols
==================

Protocols in this library are defined by two attributes:

* Byte order
* Field list

Protocol byte order
~~~~~~~~~~~~~~~~~~~

A protocol's byte order defines the order in which its field's bytes
are stored in a unsigned char buffer that represents the protocol's
field data.  There are three byte order types that can be assigned to
a protocol:

* Most significant byte first
* Least significant byte first
* Mixed byte order

In a most significant byte first protocol, a field's bytes are stored
from most significant to least significant.

In a least significant byte first protocol, a field's bytes are stored
from least significant to most significant.

In a mixed byte order protocol, each field defines its own byte order.
In this case each field's byte order can be either most significant
byte first, or least significant byte first as described above.

Protocol field list
~~~~~~~~~~~~~~~~~~~

A protocol's field list defines the length and underlying type for
each field.  Fields must have a fixed size and currently must be no
more than 64 bits.
