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

    /// Returns the value of a field's subsequent byte
    /**
     *  @tparam gt8 True if Field_Bits is greater than 8
     *
     *  @tparam Field_Bits The number of bits left at a given subsequent
     *  byte.
     *
     *  @tparam T Type used to represent the field
     */
    template<bool gt8, size_t Field_Bits, class T>
    struct get_subbyte;

    /// Specialization of get_subbyte when gt8 is true
    template<size_t Field_Bits, class T>
    struct get_subbyte<true, Field_Bits, T>
    {
	static const T value(unsigned char const * const buf) {
	    return (buf[0] << 8) + get_subbyte< (Field_Bits - 8 > 8), Field_Bits - 8, T>(buf);
	}
    };

    /// Specialization of get_subbyte when gt8 is true
    template<class T>
    struct get_subbyte<true, 0, T>
    {
	static const T value(unsigned char const * const buf) {
	    return 0;
	}
    };

    /// Specialization of get_subbyte when gt8 is false
    template<size_t Field_Bits, class T>
    struct get_subbyte<false, Field_Bits, T>
    {
	static const T value(unsigned char const * const buf) {
	    return buf[0] & lbit_mask<Field_Bits, 0>::value;
	}
    };

    /// Specialization of get_subbyte when gt8 is false
    template<class T>
    struct get_subbyte<false, 0, T>
    {
	static const T value(unsigned char const * const buf) {
	    return 0;
	}
    };

    /// Returns the value of a field
    /**
     *   @tparam Field_Bits The number of bits in a field
     *   @tparam Field_Offset The number of bits prior to this field
     *   @tparam The type used to represent the field
     */
    template<size_t Field_Bits, size_t Field_Offset, class T>
    struct get_field
    {
	static const T value(unsigned char const * const buf) {
	    // What bit to start the bitmask
	    const size_t start = Field_Offset % 8;
	    // How many bits to include in the bitmask
	    const size_t bits = Field_Bits > 8 ? Field_Bits % 8 : Field_Bits;
	    return
		((buf[0] & lbit_mask<bits, start>::value) << (Field_Bits > bits ? Field_Bits - bits : 0)) +
		get_subbyte<(Field_Bits - bits > 8), Field_Bits - bits, T>::value(buf + 1);
        }
    };

    template<class Tuple>
    class protocol
    {
	public:

	template<size_t I>
	static const typename std::tuple_element<I, Tuple>::type::type get_field(unsigned char const * const buf);
    };

    template<class Tuple>
    template<size_t I>
    const typename std::tuple_element<I, Tuple>::type::type protocol<Tuple>::get_field(unsigned char const * const buf)
    {
	return
	    protocol_helper::get_field<
		std::tuple_element<I, Tuple>::type::bits,
		bit_offset<I, Tuple>::value,
		typename std::tuple_element<I, Tuple>::type::type
		>::value(&buf[byte_offset<I, Tuple>::value]);
    }
}
