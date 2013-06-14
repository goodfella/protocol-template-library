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

    /// Returns the bit offset of a field element within a tuple
    /**
     *  @tparam I Order number of the element within the tuple
     *
     *  @tparam Tuple The tuple which contains the fields that represent
     *  the protocol
     */
    template<size_t I, class Tuple>
    struct bit_offset;

    /// Recursive implementation of bit_offset
    template<size_t I, class Tuple>
    struct bit_offset
    {
	enum : size_t { value = std::tuple_element<I - 1, Tuple>::type::bits + bit_offset<I - 1, Tuple>::value };
    };

    /// Base implementation of bit_offset for the zero element
    template <class Tuple>
    struct bit_offset<0, Tuple>
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
    struct byte_offset;

    /// Implementation of byte_offset
    template<size_t I, class Tuple>
    struct byte_offset
    {
	enum : size_t { value = bit_offset<I, Tuple>::value / 8 };
    };

    /// Returns the mask necessary to select the number of bits from the left
    /**
     *  @tparam Bits the number of bits from the left of an unsigned char
     *  to select
     */
    template<size_t Bits, size_t Start>
    struct lbit_mask
    {
	enum : unsigned char { value = (1 << (8 - (Bits + Start))) + lbit_mask<Bits - 1, Start>::value };
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
	    const size_t start = Field_Offset % 8;
	    return (buf[0] & lbit_mask<Field_Bits, start>::value) >> (8 - Field_Bits - start);
	}
    };

    /// Specialization for when the field spans multiple bytes
    template<size_t Field_Bits, size_t Field_Offset, class T>
    struct field_value<true, Field_Bits, Field_Offset, T>
    {
	static const T get(unsigned char const * const buf) {
	    const size_t start = Field_Offset % 8;
	    const size_t bits = Field_Bits > 8 ? 8 - start : Field_Bits;
	    return ((buf[0] & lbit_mask<bits, start>::value) << (Field_Bits > bits ? Field_Bits - bits : 0)) +
		field_value<(Field_Bits - bits > 8), Field_Bits - bits, 0, T>::get(buf + 1);
	}
    };

    template<class Tuple>
    class protocol
    {
	public:

	template<size_t I>
	static const typename std::tuple_element<I, Tuple>::type::type field_value(unsigned char const * const buf);
    };

    template<class Tuple>
    template<size_t I>
    const typename std::tuple_element<I, Tuple>::type::type protocol<Tuple>::field_value(unsigned char const * const buf)
    {
	return
	    protocol_helper::field_value<
		((protocol_helper::bit_offset<I, Tuple>::value % 8) + std::tuple_element<I, Tuple>::type::bits > 8),
	    std::tuple_element<I, Tuple>::type::bits,
	    protocol_helper::bit_offset<I, Tuple>::value,
	    typename std::tuple_element<I, Tuple>::type::type>::get(&buf[protocol_helper::byte_offset<I, Tuple>::value]);
    }
}
