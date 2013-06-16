#include <tuple>

namespace protocol_helper
{
    /// Represents a field in a binary protocol
    /**
     *  @tparam Bits Number of bits that make up the field
     *  @tparam T Type used to represent the field
     */
    template<size_t Bits, typename T>
    struct field
    {
	typedef T type;
	enum : size_t { bits = Bits };
    };

    /// Returns the number of bits in a field
    /**
     *  @tparam I Order number of the element within the tuple
     *  @tparam Tuple Tuple that contains the field
     */
    template<size_t I, class Tuple>
    struct field_bits
    {
	enum : size_t { value = std::tuple_element<I, Tuple>::type::bits };
    };

    /// Provides a typedef for a fields type
    /**
     *  @tparam I Order number of the element within the tuple
     *  @tparam Tuple Tuplethat contains the field
     */
    template<size_t I, class Tuple>
    struct field_type
    {
	typedef typename std::tuple_element<I, Tuple>::type::type type;
    };

    /// Returns the bit offset of a field element within a tuple
    /**
     *  @tparam I Order number of the element within the tuple
     *
     *  @tparam Tuple The tuple which contains the fields that represent
     *  the protocol
     */
    template<size_t I, class Tuple>
    struct field_bit_offset;

    /// Recursive implementation of bit_offset
    template<size_t I, class Tuple>
    struct field_bit_offset
    {
	enum : size_t { value = protocol_helper::field_bits<I - 1, Tuple>::value + protocol_helper::field_bit_offset<I - 1, Tuple>::value };
    };

    /// Base implementation of bit_offset for the zero element
    template <class Tuple>
    struct field_bit_offset<0, Tuple>
    {
	enum : size_t { value = 0 };
    };

    /// Returns the byte offset of a field element in a tuple
    /**
     *  @tparam I Order number of the element within the tuple
     *
     *  @tparam Tuple The tuple which contains the fields that represent
     *  the protocol
     */
    template<size_t I, class Tuple>
    struct field_byte_offset;

    /// Implementation of byte_offset
    template<size_t I, class Tuple>
    struct field_byte_offset
    {
	enum : size_t { value = protocol_helper::field_bit_offset<I, Tuple>::value / 8 };
    };

    /// Returns the starting bit for a fields byte mask
    /**
     * @tparam Field_Offset the bit offset
     */
    template<size_t Field_Offset>
    struct field_mask_start
    {
	enum : size_t { value = Field_Offset % 8 };
    };

    /// Returns the number of bits to read from a byte
    /**
     *  @tparam the number of bits in the byte for the field
     *  @tparam The field's offset into the byte
     */
    template<size_t Field_Bits, size_t Field_Offset>
    struct field_mask_bits
    {
	enum : size_t { value = Field_Bits > 8 ? 8 - field_mask_start<Field_Offset>::value : Field_Bits };
    };

    /// Returns the length in bits of the protocol
    /**
     *
     *  @tparam Tuple The tuple which contains the fields that
     *  represents the protocol
     */
    template<class Tuple>
    struct protocol_length
    {
	enum : size_t { value = protocol_helper::field_bit_offset<std::tuple_size<Tuple>::value - 1, Tuple>::value +
			protocol_helper::field_bits<std::tuple_size<Tuple>::value - 1, Tuple>::value };
    };

    /// Returns the mask necessary to select the number of bits from the left
    /**
     *  @tparam Bits the number of bits from the left of an unsigned char
     *  to select
     */
    template<size_t Bits, size_t Start>
    struct lbit_mask
    {
	enum : unsigned char { value = (1 << (8 - (Bits + Start))) + protocol_helper::lbit_mask<Bits - 1, Start>::value };
    };

    /// Base implementation of lbit_mask
    template<size_t Start>
    struct lbit_mask<0, Start>
    {
	enum: unsigned char { value = 0 };
    };

    /// Returns the value of a field
    /**
     *   @tparam span True if the field spans multiple bytes
     *   @tparam Field_Bits The number of bits in a field
     *   @tparam Field_Offset The number of bits prior to this field
     *   @tparam The type used to represent the field
     */
    template<bool Span, size_t Field_Bits, size_t Field_Offset, class T>
    struct field_value;

    /// Specialization for when the field does not span multiple bytes
    template<size_t Field_Bits, size_t Field_Offset, class T>
    struct field_value<false, Field_Bits, Field_Offset, T>
    {
	static const T get(unsigned char const * const buf) {
	    // Select the bits from buf with lbit_mask and right shift
	    // the resulting value the appropriate bits to fit in the
	    // space made by the true specialization
	    return (buf[0] & protocol_helper::lbit_mask<Field_Bits, protocol_helper::field_mask_start<Field_Offset>::value>::value) >> (8 - Field_Bits - protocol_helper::field_mask_start<Field_Offset>::value);
	}
    };

    /// Specialization for when the field spans multiple bytes
    template<size_t Field_Bits, size_t Field_Offset, class T>
    struct field_value<true, Field_Bits, Field_Offset, T>
    {
	static const T get(unsigned char const * const buf) {
	    // Select the bits from buf with lbit_mask and left shift
	    // the resulting value the appropriate bits to fit the
	    // next byte's value
	    return ((buf[0] & protocol_helper::lbit_mask<protocol_helper::field_mask_bits<Field_Bits, Field_Offset>::value, protocol_helper::field_mask_start<Field_Offset>::value>::value) << (Field_Bits > protocol_helper::field_mask_bits<Field_Bits, Field_Offset>::value ? Field_Bits - protocol_helper::field_mask_bits<Field_Bits, Field_Offset>::value : 0)) +
		protocol_helper::field_value<(Field_Bits - protocol_helper::field_mask_bits<Field_Bits, Field_Offset>::value > 8), Field_Bits - protocol_helper::field_mask_bits<Field_Bits, Field_Offset>::value, 0, T>::get(buf + 1);
	}
    };

    /// Wrapper around a protocol tuple
    /**
     *  @tparam Tuple The tuple that represents the protocol
     */
    template<class Tuple>
    class protocol
    {
	public:

	/// Accessor for a protocol field given a protocol buffer
	/**
	 *  @tparam I Order number of the field in the protocol tuple.
	 *  @param buf Protocol buffer.
	 */
	template<size_t I>
	static const typename protocol_helper::field_type<I, Tuple>::type field_value(unsigned char const * const buf);

	/// Length in bits of the protocol
	enum : size_t { bit_length = protocol_helper::protocol_length<Tuple>::value };

	/// Length in bytes required to store the protocols buffer
	enum : size_t { byte_length = protocol<Tuple>::bit_length % 8 ? (protocol<Tuple>::bit_length / 8) + 1 : protocol<Tuple>::bit_length / 8 };
    };

    template<class Tuple>
    template<size_t I>
    const typename protocol_helper::field_type<I, Tuple>::type protocol<Tuple>::field_value(unsigned char const * const buf)
    {
	return
	    protocol_helper::field_value<
		((protocol_helper::field_bit_offset<I, Tuple>::value % 8) + protocol_helper::field_bits<I, Tuple>::value > 8),
		protocol_helper::field_bits<I, Tuple>::value,
		protocol_helper::field_bit_offset<I, Tuple>::value,
		typename protocol_helper::field_type<I, Tuple>::type>::get(&buf[protocol_helper::field_byte_offset<I, Tuple>::value]);
    }
}
