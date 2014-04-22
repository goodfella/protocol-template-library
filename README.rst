Introduction
============

A header library that aids in the parsing of binary protocols.  This
library uses C++11 features, so its usage requires a compliant C++11
compiler.

Building
========

The protocol template library is a header library, so it doesn't
require any building to use; however, the examples are built with
CMake.  The examples can be built by issuing the following commands at
the top of the build tree::

 mkdir build-dir
 cd build-dir
 cmake ..
 make

To build an optimized version issue the following commands::

 mkdir release-build-dir
 cd release-build-dir
 cmake -DCMAKE_BUILD_TYPE=Release ..
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

Currently, only big endian byte order is supported.

Defining Protocols
==================

Protocols are defined by a field list.  A field list is composed of
fields.  A field is defined by the number of bits that make up the
field, which must be between 1 and 64 inclusive, and an underlying
type for the field's value.  Fields are specified with the
ptl::field structure.  A protocol's field list is
represented by a std::tuple of ptl::field structs.  The
std::tuple field list is then passed to a ptl::protocol
class which provides access to the fields and defines various traits.

A RFC 3550 RTP protocol specification is given below::

 	typedef std::tuple<ptl::field<2, uint8_t>,    // Version
			   ptl::field<1, bool>,       // Padding bit
			   ptl::field<1, bool>,       // Extension bit
			   ptl::field<4, uint8_t>,    // CSRC count
			   ptl::field<1, bool>,       // Marker bit
			   ptl::field<7, uint8_t>,    // Payload type
			   ptl::field<16, uint16_t>,  // Sequence number
			   ptl::field<32, uint32_t>,  // Timestamp
			   ptl::field<32, uint32_t> > // SSRC
	rtp_field_list;

	typedef ptl::protocol<rtp_field_list> rtp;

The above code snippet defines the RTP protocol parts that are present
in every RTP packet.  An example of setting and retrieving the RTP
version is given below::

        // Declare a buffer big enough to store the protocol fields
        rtp::traits::array_type rtp_buf;
 
        // Set the RTP version
        rtp::field_value<0>(rtp_buf.data(), 2);
 
        // Retrieve the RTP version
        auto version = rtp::field_value<0>(rtp_buf.data());

It's also possible to provide convienent labels for a protocol's
fields via an enum class::

	enum class rtp_fields : size_t {
		version,
		padding_bit,
		extension_bit,
		csrc_count,
		marker_bit,
		payload_type,
		sequence_number,
		timestamp,
		ssrc
	};

This allows for the following::

        // Declare a buffer big enough to store the protocol fields
	rtp::traits::array_type rtp_buf;

	// Set the RTP version
	rtp::field_value<rtp_fields::version>(rtp_buf.data(), 2);

	// Retrieve the RTP version
	auto version = rtp::field_value<rtp_fields::version>(rtp_buf.data());
