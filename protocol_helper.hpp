#include <tuple>
#include <limits>

namespace protocol_helper
{
    /// Returns the number of bits per byte
    struct bits_per_byte
    {
	enum : size_t { value = static_cast<size_t>(std::numeric_limits<unsigned char>::digits) };
    };

    /// Constrains fields to be unsigned
    template<bool Signed>
    struct unsigned_type_constraint;

    /// Accept only unsigned types
    /**
     *  Right shifting of signed types has implementation defined
     *  behavior, and that's not portable.
     */
    template<>
    struct unsigned_type_constraint<true> {};

    /// Increments an object by one
    template<class T>
    struct increment
    {
	static T value(const T t) {
	    return t + 1;
	}
    };

    /// Decrements an object by one
    template<class T>
    struct decrement
    {
	static T value(const T t) {
	    return t - 1;
	}
    };

    /// Represents a field in a binary protocol
    /**
     *  @tparam Bits Number of bits that make up the field
     *  @tparam T Type used to represent the field
     */
    template<size_t Bits, typename T>
    struct field: unsigned_type_constraint<(!std::numeric_limits<T>::is_signed)>
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
    struct field_start_byte
    {
	enum : size_t { value = protocol_helper::field_bit_offset<I, Tuple>::value / protocol_helper::bits_per_byte::value };
    };

    /// Returns the number of bits to read from a byte
    /**
     *  @tparam Byte_Offset The field's offset into the byte
     */
    template<size_t Byte_Offset>
    struct byte_mask_len
    {
	enum : size_t { value = protocol_helper::bits_per_byte::value - Byte_Offset };
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

    /// Returns the mask necessary to select bits starting from the most significant bit
    /**
     *  @tparam Bits the number of bits to select
     *
     *  @tparam Start Starting most significant bit (0 based)
     *
     *  @tparam T the type of value
     */
    template<size_t Bits, size_t Start, class T>
    struct msb_mask
    {
	enum : T { value = (static_cast<T>(1) << (std::numeric_limits<T>::digits - (Bits + Start))) + protocol_helper::msb_mask<Bits - 1, Start, T>::value };
    };

    /// Base implementation of msb_mask
    template<size_t Start, class T>
    struct msb_mask<0, Start, T>
    {
	enum: T { value = 0 };
    };

    /// Definitions needed for most significant bit first protocols / fields
    struct msb_first
    {
	/// Returns the field's next byte in a unsigned char protocol buffer
	template<class T>
	struct next_byte
	{
	    typedef protocol_helper::increment<T> type;
	};

	/// Generates the mask for a field's byte
	template<size_t Bits, size_t Start, class T>
	struct byte_mask
	{
	    typedef protocol_helper::msb_mask<Bits, Start, T> type;
	};

	/// Generates the index of the first byte for the field
	template<size_t I, class Tuple>
	struct first_byte
	{
	    typedef protocol_helper::field_start_byte<I, Tuple> type;
	};
    };

    struct lsb_first;

    /// Used to specify that a protocol's fields have their own bit
    /// order
    struct mixed_bit_order;

    /// Returns the value of a field
    /**
     *   @tparam Span True if the field spans multiple bytes
     *   @tparam Field_Bits The remaining number of bits in a field to process
     *   @tparam Byte_Offset The number of bits prior to this field
     *   @tparam Bit_Order The bit order of the field
     *   @tparam T The type used to represent the field
     */
    template<bool Span, size_t Field_Bits, size_t Byte_Offset, class Bit_Order, class T>
    struct field_value;

    /// Specialization for when the field does not span multiple bytes
    template<size_t Field_Bits, size_t Byte_Offset, class Bit_Order, class T>
    struct field_value<false, Field_Bits, Byte_Offset, Bit_Order, T>
    {
	typedef typename Bit_Order:: template byte_mask<Field_Bits,
							Byte_Offset,
							unsigned char>::type byte_mask;

	static const T get(unsigned char const * const buf) {
	    // Select the bits from buf with msb_mask and right shift
	    // the resulting value the appropriate bits to fit in the
	    // space made by the true specialization
	    return (buf[0] & byte_mask::value) >>
		(protocol_helper::bits_per_byte::value - Field_Bits - Byte_Offset);
	}

	static const void set(unsigned char * const buf, const T value) {

	    typedef protocol_helper::msb_mask<Field_Bits,
					      std::numeric_limits<T>::digits - Field_Bits,
					      T> value_mask;

	    // Clear the current value
	    buf[0] &= static_cast<unsigned char>(~byte_mask::value);

	    // Set the new value
	    buf[0] |= ((value & value_mask::value) <<
		       (protocol_helper::bits_per_byte::value - Field_Bits - Byte_Offset));
	}
    };

    /// Specialization for when the field spans multiple bytes
    template<size_t Field_Bits, size_t Byte_Offset, class Bit_Order, class T>
    struct field_value<true, Field_Bits, Byte_Offset, Bit_Order, T>
    {
	// mask type provided by the Bit_Order
	typedef typename Bit_Order:: template byte_mask<protocol_helper::byte_mask_len<Byte_Offset>::value,
							Byte_Offset,
							unsigned char>::type byte_mask;

	static const T get(unsigned char const * const buf) {

	    typedef typename Bit_Order:: template next_byte<unsigned char const*>::type next_byte;

	    // Select the bits from buf with msb_mask and left shift
	    // the resulting value the appropriate bits to fit the
	    // next byte's value.  The formula below is:

	    // buf[0] & (mask to select field value in this byte) << (number of bits to fit the remaining field bits)
	    return (static_cast<T>((buf[0] & byte_mask::value)) <<
		    (Field_Bits - protocol_helper::byte_mask_len<Byte_Offset>::value)) +

		protocol_helper::field_value<(Field_Bits - protocol_helper::byte_mask_len<Byte_Offset>::value > protocol_helper::bits_per_byte::value),
		    Field_Bits - protocol_helper::byte_mask_len<Byte_Offset>::value,
		    0, Bit_Order, T>::get(next_byte::value(buf));
	}

	static void set(unsigned char * const buf, const T val) {

	    typedef typename Bit_Order:: template next_byte<unsigned char* const>::type next_byte;
	    typedef protocol_helper::msb_mask<protocol_helper::byte_mask_len<Byte_Offset>::value,
					      std::numeric_limits<T>::digits - Field_Bits,
					      T> value_mask;

	    // Clear the current value
	    buf[0] &= static_cast<unsigned char>(~byte_mask::value);

	    // Set current byte
	    buf[0] |= ((val & value_mask::value) >>
		       (Field_Bits - protocol_helper::byte_mask_len<Byte_Offset>::value));

	    // Set next byte
	    protocol_helper::field_value<
		(Field_Bits - protocol_helper::byte_mask_len<Byte_Offset>::value > protocol_helper::bits_per_byte::value),
		Field_Bits - protocol_helper::byte_mask_len<Byte_Offset>::value,
		0,
		Bit_Order,
		T>::set(next_byte::value(buf), val);
	}
    };

    /// Base class for a protocol
    /**
     *  Defines a protocol's members that are bit order agnostic
     */
    template<class Tuple>
    struct protocol_base
    {
	typedef Tuple tuple;

	/// Length in bits of the protocol
	enum : size_t { bit_length = protocol_helper::protocol_length<Tuple>::value };

	/// Length in bytes required to store the protocols buffer
	enum : size_t { byte_length = protocol_base<Tuple>::bit_length % protocol_helper::bits_per_byte::value ? (protocol_base<Tuple>::bit_length / protocol_helper::bits_per_byte::value) + 1 : protocol_base<Tuple>::bit_length / protocol_helper::bits_per_byte::value };

	/// Number of fields the protocol has
	enum : size_t { field_count = std::tuple_size<Tuple>::value };
    };

    /// Wrapper around a protocol tuple
    /**
     *  @tparam Bit_Order the protocol's bit order
     *  @tparam Tuple The tuple that represents the protocol
     */
    template<class Bit_Order, class Tuple>
    class protocol: public protocol_base<Tuple>
    {
	public:

	typedef Bit_Order bit_order;

	/// Accessor for a protocol field given a protocol buffer
	/**
	 *  @tparam I Order number of the field in the protocol tuple.
	 *  @param buf Protocol buffer.
	 */
	template<size_t I>
	static const typename protocol_helper::field_type<I, Tuple>::type field_value(unsigned char const * const buf);

	/// Sets a protocol field's value
	/**
	 *  @tparam I Order number of the field in the protocol tuple,
	 *  @param buf Protocol buffer
	 *  @param val Value to set the field to
	 */
	template<size_t I>
	static void field_value(unsigned char * const buf, const typename protocol_helper::field_type<I, Tuple>::type value);
    };

    template<class Bit_Order, class Tuple>
    template<size_t I>
    const typename protocol_helper::field_type<I, Tuple>::type protocol<Bit_Order, Tuple>::field_value(unsigned char const * const buf)
    {
	/// template function that returns the first byte index for
	/// the given bit order
	typedef typename Bit_Order:: template first_byte<I, Tuple>::type first_byte;

	return
	    protocol_helper::field_value<
		((protocol_helper::field_bit_offset<I, Tuple>::value % protocol_helper::bits_per_byte::value) + protocol_helper::field_bits<I, Tuple>::value > protocol_helper::bits_per_byte::value),
		protocol_helper::field_bits<I, Tuple>::value,
		protocol_helper::field_bit_offset<I, Tuple>::value % protocol_helper::bits_per_byte::value,
		Bit_Order,
		typename protocol_helper::field_type<I, Tuple>::type>::get(&buf[first_byte::value]);
    }

    template<class Bit_Order, class Tuple>
    template<size_t I>
    void protocol<Bit_Order, Tuple>::field_value(unsigned char * const buf, const typename protocol_helper::field_type<I, Tuple>::type val)
    {
	/// template function that returns the first byte index for
	/// the given bit order
	typedef typename Bit_Order:: template first_byte<I, Tuple>::type first_byte;

	protocol_helper::field_value<((protocol_helper::field_bit_offset<I, Tuple>::value % protocol_helper::bits_per_byte::value) + protocol_helper::field_bits<I, Tuple>::value > protocol_helper::bits_per_byte::value),
	    protocol_helper::field_bits<I, Tuple>::value,
	    protocol_helper::field_bit_offset<I, Tuple>::value % protocol_helper::bits_per_byte::value,
	    Bit_Order,
	    typename protocol_helper::field_type<I, Tuple>::type>::set(&buf[first_byte::value], val);
    }
}
