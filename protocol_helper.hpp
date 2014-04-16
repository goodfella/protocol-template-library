#include <cstring>
#include <tuple>
#include <limits>
#include <array>

namespace protocol_helper
{
	template <bool, class T, class F>
	struct condition
	{
		typedef T type;
	};

	template <class T, class F>
	struct condition<false, T, F>
	{
		typedef F type;
	};

	static const size_t bits_per_byte = static_cast<size_t>(std::numeric_limits<unsigned char>::digits);

	// Returns the number of bytes required to store the provided bits
	template <size_t Bits>
	struct required_bytes
	{
		static const size_t value = (Bits / protocol_helper::bits_per_byte) +
			((Bits % protocol_helper::bits_per_byte) == 0 ? 0 : 1);
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
		static const T value = (static_cast<T>(1) << (std::numeric_limits<T>::digits - (Bits + Start))) + protocol_helper::msb_mask<Bits - 1, Start, T>::value;
	};

	/// Base implementation of msb_mask
	template<size_t Start, class T>
	struct msb_mask<0, Start, T>
	{
		static const T value = 0;
	};

	/// Returns the mask necessary to select bits starting from the least significant bit
	/**
	 *  @tparam Bits The number of bits to select
	 *
	 *  @tparam Start Starting least significant bit (0 based)
	 *
	 *  @tparam T The type of value
	 */
	template<size_t Bits, size_t Start, class T>
	struct lsb_mask
	{
		static const T value = (static_cast<T>(1) << (Bits - 1 + Start)) + lsb_mask<Bits - 1, Start, T>::value;
	};

	/// Base implementation of lsb_mask
	template<size_t Start, class T>
	struct lsb_mask<0, Start, T>
	{
		static const T value = 0;
	};

	/// Returns the number of bits to read from a byte
	/**
	 *  @tparam Byte_Offset The field's offset into the byte
	 */
	template<size_t Byte_Offset>
	struct byte_mask_len
	{
		static const size_t value = protocol_helper::bits_per_byte - Byte_Offset;
	};

	/// Returns whether or not Bits + Offset spans multiple bytes
	/**
	 *  @tparam Bits The number of bits
	 *  @tparam Offset The offset
	 */
	template <size_t Bits, size_t Offset>
	struct spans_bytes
	{
		static const bool value = Bits + (Offset % protocol_helper::bits_per_byte) > protocol_helper::bits_per_byte;
	};

	/// Provides the offset into a byte
	/**
	 *  @tparam Bit_Offset The number of bits
	 */
	template <size_t Bit_Offset>
	struct byte_offset
	{
		static const size_t value = Bit_Offset % protocol_helper::bits_per_byte;
	};

	/// Represents a field in a binary protocol
	/**
	 *  @tparam Bits Number of bits that make up the field
	 *  @tparam T Type used to represent the field
	 */
	template<size_t Bits, typename T>
	struct field
	{
		static_assert(Bits > 0,
			      "The number of bits must be greater than 0");
		static_assert(!std::numeric_limits<T>::is_signed,
			      "A field's type must be unsigned");
		static_assert(static_cast<size_t>(std::numeric_limits<T>::digits) >= Bits,
			      "The number of bits in a field's type must be greater than or equal to the number of bits in the field");

		typedef T value_type;
		static const size_t bits = Bits;

		/// Number of bytes required to store the field's value
		static const size_t bytes = protocol_helper::required_bytes<bits>::value;
	};

	/// Returns the number of bits in a field
	/**
	 *  @tparam I Order number of the element within the tuple
	 *  @tparam Tuple Tuple that contains the field
	 */
	template<size_t I, class Tuple>
	struct field_bits
	{
		static const size_t value = std::tuple_element<I, Tuple>::type::bits;
	};

	/// Provides a typedef for a fields type
	/**
	 *  @tparam I Order number of the element within the tuple
	 *  @tparam Tuple Tuplethat contains the field
	 */
	template<size_t I, class Tuple>
	struct field_type
	{
		typedef typename std::tuple_element<I, Tuple>::type::value_type type;
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
		static const size_t value = protocol_helper::field_bits<I - 1, Tuple>::value +
			protocol_helper::field_bit_offset<I - 1, Tuple>::value;
	};

	/// Base implementation of bit_offset for the zero element
	template <class Tuple>
	struct field_bit_offset<0, Tuple>
	{
		static const size_t value = 0;
	};

	/// Returns the byte index of a field element in a tuple
	/**
	 *  @tparam I Order number of the element within the tuple
	 *
	 *  @tparam Tuple The tuple which contains the fields that represent
	 *  the protocol
	 */
	template<size_t I, class Tuple>
	struct field_first_byte
	{
		static const size_t value = protocol_helper::field_bit_offset<I, Tuple>::value / protocol_helper::bits_per_byte;
	};

	/// Returns the last byte index of a field
	/**
	 *  @tparam I Order number of the element within the tuple
	 *  @tparam Tuple The tuple the represents the protocol
	 */
	template <size_t I, class Tuple>
	struct field_last_byte
	{
		static const size_t value = (protocol_helper::field_bit_offset<I, Tuple>::value +
					     protocol_helper::field_bits<I, Tuple>::value - 1) / protocol_helper::bits_per_byte;
	};

	/// Returns whether the field spans multiple bytes
	/**
	 * @tparam I Order number of the element within the tuple
	 * @tparam Tuple The tuple that represents the protocol
	 */
	template <size_t I, class Tuple>
	struct field_spans_bytes
	{
		static const size_t value = ((protocol_helper::field_bit_offset<I, Tuple>::value % protocol_helper::bits_per_byte) +
					     protocol_helper::field_bits<I, Tuple>::value) > protocol_helper::bits_per_byte;
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
		static const size_t value = protocol_helper::field_bit_offset<std::tuple_size<Tuple>::value - 1, Tuple>::value +
			protocol_helper::field_bits<std::tuple_size<Tuple>::value - 1, Tuple>::value;
	};

	/// Specialization for when the field does not span multiple bytes
	template<size_t Field_Bits, size_t Byte_Offset, class T>
	struct terminal_field_value
	{
		typedef protocol_helper::msb_mask<Field_Bits,
						  Byte_Offset,
						  unsigned char> byte_mask;

		static const size_t value_shift = protocol_helper::bits_per_byte - Field_Bits - Byte_Offset;

		static const T get(unsigned char const * const buf) {
			// Select the bits from buf with the byte order's byte
			// mask and right shift the resulting value the
			// appropriate bits to fit in the value in the remaining
			// bits of the field
			return (buf[0] & byte_mask::value) >> value_shift;
		}

		static const void set(unsigned char * const buf, const T value) {

			typedef protocol_helper::msb_mask<Field_Bits,
				std::numeric_limits<T>::digits - Field_Bits,
				T> value_mask;

			// Clear the current value
			buf[0] &= static_cast<unsigned char>(~byte_mask::value);

			// Set the new value
			buf[0] |= ((value & value_mask::value) << value_shift);
		}
	};

	/// Sets and returns the value of a field
	/**
	 *   @tparam Field_Bits The remaining number of bits in a field to process
	 *   @tparam Byte_Offset The field's offset into the current byte
	 *   @tparam T The type used to represent the field
	 */
	template<size_t Field_Bits, size_t Byte_Offset, class T>
	struct recursive_field_value
	{
		typedef protocol_helper::msb_mask<protocol_helper::byte_mask_len<Byte_Offset>::value,
						  Byte_Offset,
						  unsigned char> byte_mask;

		static const size_t value_shift = Field_Bits - (protocol_helper::bits_per_byte - Byte_Offset);
		static const size_t next_bit_count = Field_Bits - protocol_helper::byte_mask_len<Byte_Offset>::value;
		static const bool next_spans_bytes = protocol_helper::spans_bytes<next_bit_count, 0>::value;

		static const T get(unsigned char const * const buf) {

			// Selects the bits from buf with the mask, and left
			// shifts the resulting value the appropriate bits to fit
			// the next byte's value.  The formula below is:

			// buf[0] & (mask to select field value in this byte) << (number of remaining field bits)
			return (static_cast<T>((buf[0] & byte_mask::value)) << value_shift) +
				protocol_helper::condition<next_spans_bytes,
							   protocol_helper::recursive_field_value<next_bit_count, 0, T>,
							   protocol_helper::terminal_field_value<next_bit_count, 0, T>
							   >::type::get(buf + 1);
		}

		static void set(unsigned char * const buf, const T val) {

			typedef protocol_helper::msb_mask<protocol_helper::byte_mask_len<Byte_Offset>::value,
				std::numeric_limits<T>::digits - Field_Bits,
				T> value_mask;

			// Clear the current value
			buf[0] &= static_cast<unsigned char>(~byte_mask::value);

			// Set current byte
			buf[0] |= ((val & value_mask::value) >> value_shift);

			protocol_helper::condition<next_spans_bytes,
						   protocol_helper::recursive_field_value<next_bit_count, 0, T>,
						   protocol_helper::terminal_field_value<next_bit_count, 0, T>
						   >::type::set(buf + 1, val);
		}
	};

	template <size_t Bits, size_t Offset, class T>
	struct field_value
	{
		typedef typename protocol_helper::condition<protocol_helper::spans_bytes<Bits, Offset>::value,
							    protocol_helper::recursive_field_value<Bits, Offset, T>,
							    protocol_helper::terminal_field_value<Bits, Offset, T>
							    >::type type;
	};

	/// Defines the field traits which are dependent on the protocol
	template <size_t Field, class Tuple>
	struct field_protocol_traits
	{
		/// The type of the field
		typedef typename std::tuple_element<Field, Tuple>::type type;
		/// The field index in the protocol
		static const size_t index = Field;
		/// The number of bits before the field's bits in a buffer
		static const size_t bit_offset = protocol_helper::field_bit_offset<Field, Tuple>::value;
		/// The bit offset into the fields first byte
		static const size_t byte_bit_offset = bit_offset % protocol_helper::bits_per_byte;
		/// The index of the field's first byte in a buffer
		static const size_t byte_index = protocol_helper::field_first_byte<Field, Tuple>::value;
		/// True if the field spans multiple bytes in a buffer
		static const bool spans_bytes = protocol_helper::spans_bytes<type::bits, bit_offset>::value;
	};

	template <class Tuple>
	struct protocol_traits
	{
		/// number of bits in the protocol
		static const size_t bits = protocol_helper::protocol_length<Tuple>::value;

		/// number of bytes required to store the protocols buffer
		static const size_t bytes = protocol_helper::required_bytes<bits>::value;

		/// Number of fields the protocol has
		static const size_t fields = std::tuple_size<Tuple>::value;

		/// std::array representation of the protocol's buffer
		typedef std::array<unsigned char, bytes> array_type;
	};

	/// Class that represents a protocol defined by a field tuple
	/**
	 *  @tparam Tuple The tuple that represents the protocol
	 */
	template<class Tuple>
	class protocol
	{
		public:

		typedef Tuple tuple_type;
		struct traits: public protocol_traits<Tuple> {};

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
		 *  @param buf Protocol buffern
		 *  @param val Value to set the field to
		 */
		template<size_t I>
		static void field_value(unsigned char * const buf, const typename protocol_helper::field_type<I, Tuple>::type value);

		/// Defines the field_protocol_traits for the field
		/**
		 *  @tparam I Order number of the field in the protocol tuple
		 */
		template<size_t I>
		struct field_traits: public protocol_helper::field_protocol_traits<I, Tuple> {};

		/// Provides a field type
		/**
		 *  @tparam I Order number of the field in the protocol tuple
		 */
		template <size_t I>
		struct field: public protocol_helper::field<protocol_helper::field_bits<I, Tuple>::value,
							    typename protocol_helper::field_type<I, Tuple>::type> {};
	};

	template<class Tuple>
	template<size_t I>
	const typename protocol_helper::field_type<I, Tuple>::type protocol<Tuple>::field_value(unsigned char const * const buf)
	{
		return protocol_helper::field_value<protocol_helper::field_bits<I, Tuple>::value,
						    protocol_helper::byte_offset<protocol_helper::field_bit_offset<I, Tuple>::value>::value,
						    typename protocol_helper::field_type<I, Tuple>::type
						    >::type::get(buf + protocol_helper::field_first_byte<I, Tuple>::value);
	}

	template<class Tuple>
	template<size_t I>
	void protocol<Tuple>::field_value(unsigned char * const buf, const typename protocol_helper::field_type<I, Tuple>::type val)
	{
		protocol_helper::field_value<protocol_helper::field_bits<I, Tuple>::value,
					     protocol_helper::byte_offset<protocol_helper::field_bit_offset<I, Tuple>::value>::value,
					     typename protocol_helper::field_type<I, Tuple>::type
					     >::type::set(buf + protocol_helper::field_first_byte<I, Tuple>::value, val);
	}
}
