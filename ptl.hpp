#include <cstring>
#include <tuple>
#include <limits>
#include <array>
#include <type_traits>

namespace ptl
{
	static constexpr std::size_t bits_per_byte = static_cast<std::size_t>(std::numeric_limits<unsigned char>::digits);

	// Returns the number of bytes required to store the provided bits
	constexpr std::size_t required_bytes(std::size_t bits) { return (bits / ptl::bits_per_byte) + ((bits % ptl::bits_per_byte) != 0); }

	/// Returns the mask necessary to select bits starting from the most significant bit
	/**
	 *  @param bits the number of bits to select
	 *
	 *  @param start Starting most significant bit (0 based)
	 *
	 *  @tparam T the type of value
	 */
	template <class T>
	constexpr T msb_mask(std::size_t bits, std::size_t start) {
		return bits > 0 ? (static_cast<T>(1) << (std::numeric_limits<T>::digits - (bits + start))) +
			ptl::msb_mask<T>(bits - 1, start) : 0;
	}

	/// Returns the mask necessary to select bits starting from the least significant bit
	/**
	 *  @param bits The number of bits to select
	 *
	 *  @param start Starting least significant bit (0 based)
	 *
	 *  @tparam T The type of value
	 */
	template <class T>
	constexpr T lsb_mask(std::size_t bits, std::size_t start) {
		return bits > 0 ? (static_cast<T>(1) << (bits - 1 + start)) + ptl::lsb_mask<T>(bits - 1, start) : 0;
	}

	/// Returns the number of bits to read from a byte
	/**
	 *  @param byte_offset The field's offset into the byte
	 */
	constexpr std::size_t byte_mask_len(std::size_t byte_offset) {
		return ptl::bits_per_byte - byte_offset;
	}

	/// Returns whether or not Bits + Offset spans multiple bytes
	/**
	 *  @param bits The number of bits
	 *  @param offset The offset
	 */
	constexpr bool spans_bytes(std::size_t bits, std::size_t offset) {
		return bits + (offset % ptl::bits_per_byte) > ptl::bits_per_byte;
	}

	/// Provides the offset into a byte
	/**
	 *  @param bit_offset The number of bits
	 */
	constexpr std::size_t byte_offset(std::size_t bit_offset) {
		return bit_offset % ptl::bits_per_byte;
	}

	/// Represents a field in a binary protocol
	/**
	 *  @tparam Bits Number of bits that make up the field
	 *  @tparam T Type used to represent the field
	 */
	template<std::size_t Bits, typename T>
	struct field
	{
		static_assert(Bits > 0,
			      "The number of bits must be greater than 0");
		static_assert(!std::numeric_limits<T>::is_signed,
			      "A field's type must be unsigned");
		static_assert(static_cast<std::size_t>(std::numeric_limits<T>::digits) >= Bits,
			      "The number of bits in a field's type must be greater than or equal to the number of bits in the field");

		typedef T value_type;
		static constexpr std::size_t bits = Bits;

		/// Number of bytes required to store the field's value
		static constexpr std::size_t bytes = ptl::required_bytes(bits);
	};

	/// Returns the number of bits in a field
	/**
	 *  @tparam I Order number of the element within the tuple
	 *  @tparam Tuple Tuple that contains the field
	 */
	template<std::size_t I, class Tuple>
	struct field_bits
	{
		static constexpr std::size_t value = std::tuple_element<I, Tuple>::type::bits;
	};

	/// Provides a typedef for a fields type
	/**
	 *  @tparam I Order number of the element within the tuple
	 *  @tparam Tuple Tuplethat contains the field
	 */
	template<std::size_t I, class Tuple>
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
	template<std::size_t I, class Tuple>
	struct field_bit_offset;

	/// Recursive implementation of bit_offset
	template<std::size_t I, class Tuple>
	struct field_bit_offset
	{
		static constexpr std::size_t value = ptl::field_bits<I - 1, Tuple>::value +
			ptl::field_bit_offset<I - 1, Tuple>::value;
	};

	/// Base implementation of bit_offset for the zero element
	template <class Tuple>
	struct field_bit_offset<0, Tuple>
	{
		static constexpr std::size_t value = 0;
	};

	/// Returns the byte index of a field element in a tuple
	/**
	 *  @tparam I Order number of the element within the tuple
	 *
	 *  @tparam Tuple The tuple which contains the fields that represent
	 *  the protocol
	 */
	template<std::size_t I, class Tuple>
	struct field_first_byte
	{
		static constexpr std::size_t value = ptl::field_bit_offset<I, Tuple>::value / ptl::bits_per_byte;
	};

	/// Returns the last byte index of a field
	/**
	 *  @tparam I Order number of the element within the tuple
	 *  @tparam Tuple The tuple the represents the protocol
	 */
	template <std::size_t I, class Tuple>
	struct field_last_byte
	{
		static constexpr std::size_t value = (ptl::field_bit_offset<I, Tuple>::value +
					     ptl::field_bits<I, Tuple>::value - 1) / ptl::bits_per_byte;
	};

	/// Returns whether the field spans multiple bytes
	/**
	 * @tparam I Order number of the element within the tuple
	 * @tparam Tuple The tuple that represents the protocol
	 */
	template <std::size_t I, class Tuple>
	struct field_spans_bytes
	{
		static constexpr bool value = ((ptl::field_bit_offset<I, Tuple>::value % ptl::bits_per_byte) +
					   ptl::field_bits<I, Tuple>::value) > ptl::bits_per_byte;
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
		static constexpr std::size_t value = ptl::field_bit_offset<std::tuple_size<Tuple>::value - 1, Tuple>::value +
			ptl::field_bits<std::tuple_size<Tuple>::value - 1, Tuple>::value;
	};

	/// Specialization for when the field does not span multiple bytes
	template<std::size_t Field_Bits, std::size_t Byte_Offset, class T>
	struct terminal_field_value
	{
		private:
		static constexpr unsigned char byte_mask = ptl::msb_mask<unsigned char>(Field_Bits, Byte_Offset);
		static constexpr std::size_t value_shift = ptl::bits_per_byte - Field_Bits - Byte_Offset;

		public:
		static const T get(unsigned char const * const buf) {
			// Select the bits from buf with the byte order's byte
			// mask and right shift the resulting value the
			// appropriate bits to fit in the value in the remaining
			// bits of the field
			return (buf[0] & byte_mask) >> value_shift;
		}

		static const void set(unsigned char * const buf, const T value) {

			static constexpr T value_mask = ptl::msb_mask<T>(Field_Bits,
									 std::numeric_limits<T>::digits - Field_Bits);
			// Clear the current value
			buf[0] &= static_cast<unsigned char>(~byte_mask);

			// Set the new value
			buf[0] |= ((value & value_mask) << value_shift);
		}
	};

	/// Sets and returns the value of a field
	/**
	 *   @tparam Field_Bits The remaining number of bits in a field to process
	 *   @tparam Byte_Offset The field's offset into the current byte
	 *   @tparam T The type used to represent the field
	 */
	template<std::size_t Field_Bits, std::size_t Byte_Offset, class T>
	struct recursive_field_value
	{
		private:
		static constexpr unsigned char byte_mask = ptl::msb_mask<unsigned char>(ptl::byte_mask_len(Byte_Offset),
											Byte_Offset);
		static constexpr std::size_t value_shift = Field_Bits - (ptl::bits_per_byte - Byte_Offset);
		static constexpr std::size_t next_bit_count = Field_Bits - ptl::byte_mask_len(Byte_Offset);
		static constexpr bool next_spans_bytes = ptl::spans_bytes(next_bit_count, 0);

		public:
		static const T get(unsigned char const * const buf) {

			// Selects the bits from buf with the mask, and left
			// shifts the resulting value the appropriate bits to fit
			// the next byte's value.  The formula below is:

			// buf[0] & (mask to select field value in this byte) << (number of remaining field bits)
			return (static_cast<T>((buf[0] & byte_mask)) << value_shift) +
				std::conditional<next_spans_bytes,
						 ptl::recursive_field_value<next_bit_count, 0, T>,
						 ptl::terminal_field_value<next_bit_count, 0, T>
						 >::type::get(buf + 1);
		}

		static void set(unsigned char * const buf, const T val) {

			static constexpr T value_mask = ptl::msb_mask<T>(ptl::byte_mask_len(Byte_Offset),
									 (std::numeric_limits<T>::digits - Field_Bits));

			// Clear the current value
			buf[0] &= static_cast<unsigned char>(~byte_mask);

			// Set current byte
			buf[0] |= ((val & value_mask) >> value_shift);

			std::conditional<next_spans_bytes,
					 ptl::recursive_field_value<next_bit_count, 0, T>,
					 ptl::terminal_field_value<next_bit_count, 0, T>
					 >::type::set(buf + 1, val);
		}
	};

	template <std::size_t Bits, std::size_t Offset, class T>
	struct field_value
	{
		typedef typename std::conditional<ptl::spans_bytes(Bits, Offset),
						  ptl::recursive_field_value<Bits, Offset, T>,
						  ptl::terminal_field_value<Bits, Offset, T>
						  >::type type;
	};

	/// Defines the field traits which are dependent on the protocol
	template <std::size_t Field, class Tuple>
	struct field_protocol_traits
	{
		/// The type of the field
		typedef typename std::tuple_element<Field, Tuple>::type type;
		/// The field index in the protocol
		static constexpr std::size_t index = Field;
		/// The number of bits before the field's bits in a buffer
		static constexpr std::size_t bit_offset = ptl::field_bit_offset<Field, Tuple>::value;
		/// The bit offset into the fields first byte
		static constexpr std::size_t byte_bit_offset = bit_offset % ptl::bits_per_byte;
		/// The index of the field's first byte in a buffer
		static constexpr std::size_t byte_index = ptl::field_first_byte<Field, Tuple>::value;
		/// True if the field spans multiple bytes in a buffer
		static constexpr bool spans_bytes = ptl::spans_bytes(type::bits, bit_offset);
	};

	template <class Tuple>
	struct protocol_traits
	{
		/// number of bits in the protocol
		static constexpr std::size_t bits = ptl::protocol_length<Tuple>::value;

		/// number of bytes required to store the protocols buffer
		static constexpr std::size_t bytes = ptl::required_bytes(bits);

		/// Number of fields the protocol has
		static constexpr std::size_t fields = std::tuple_size<Tuple>::value;

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
		template<std::size_t I>
		static const typename ptl::field_type<I, Tuple>::type field_value(unsigned char const * const buf);

		/// Sets a protocol field's value
		/**
		 *  @tparam I Order number of the field in the protocol tuple,
		 *  @param buf Protocol buffern
		 *  @param val Value to set the field to
		 */
		template<std::size_t I>
		static void field_value(unsigned char * const buf, const typename ptl::field_type<I, Tuple>::type value);

		/// Defines the field_protocol_traits for the field
		/**
		 *  @tparam I Order number of the field in the protocol tuple
		 */
		template<std::size_t I>
		struct field_traits: public ptl::field_protocol_traits<I, Tuple> {};

		/// Provides a field type
		/**
		 *  @tparam I Order number of the field in the protocol tuple
		 */
		template <std::size_t I>
		struct field: public ptl::field<ptl::field_bits<I, Tuple>::value,
							    typename ptl::field_type<I, Tuple>::type> {};
	};

	template<class Tuple>
	template<std::size_t I>
	const typename ptl::field_type<I, Tuple>::type protocol<Tuple>::field_value(unsigned char const * const buf)
	{
		return ptl::field_value<ptl::field_bits<I, Tuple>::value,
					ptl::byte_offset(ptl::field_bit_offset<I, Tuple>::value),
					typename ptl::field_type<I, Tuple>::type
					>::type::get(buf + ptl::field_first_byte<I, Tuple>::value);
	}

	template<class Tuple>
	template<std::size_t I>
	void protocol<Tuple>::field_value(unsigned char * const buf, const typename ptl::field_type<I, Tuple>::type val)
	{
		ptl::field_value<ptl::field_bits<I, Tuple>::value,
				 ptl::byte_offset(ptl::field_bit_offset<I, Tuple>::value),
				 typename ptl::field_type<I, Tuple>::type
				 >::type::set(buf + ptl::field_first_byte<I, Tuple>::value, val);
	}
}
