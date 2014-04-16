#include <cstring>
#include <cstdint>
#include <iostream>
#include <tuple>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <string>
#include <array>
#include "protocol_helper.hpp"

using namespace std;
using namespace protocol_helper;

typedef tuple<field<1, bool>,
	      field<1, uint8_t>,
	      field<2, uint8_t>,
	      field<3, uint8_t>,
	      field<4, uint8_t>,
	      field<5, uint8_t>,
	      field<6, uint8_t>,
	      field<7, uint8_t>,
	      field<8, uint8_t>,

	      field<1, uint16_t>,
	      field<2, uint16_t>,
	      field<3, uint16_t>,
	      field<4, uint16_t>,
	      field<5, uint16_t>,
	      field<6, uint16_t>,
	      field<7, uint16_t>,
	      field<8, uint16_t>,
	      field<9, uint16_t>,
	      field<10, uint16_t>,
	      field<11, uint16_t>,
	      field<12, uint16_t>,
	      field<13, uint16_t>,
	      field<14, uint16_t>,
	      field<15, uint16_t>,
	      field<16, uint16_t>,

	      field<1, uint32_t>,
	      field<2, uint32_t>,
	      field<3, uint32_t>,
	      field<4, uint32_t>,
	      field<5, uint32_t>,
	      field<6, uint32_t>,
	      field<7, uint32_t>,
	      field<8, uint32_t>,
	      field<9, uint32_t>,
	      field<10, uint32_t>,
	      field<11, uint32_t>,
	      field<12, uint32_t>,
	      field<13, uint32_t>,
	      field<14, uint32_t>,
	      field<15, uint32_t>,
	      field<16, uint32_t>,
	      field<17, uint32_t>,
	      field<18, uint32_t>,
	      field<19, uint32_t>,
	      field<20, uint32_t>,
	      field<21, uint32_t>,
	      field<22, uint32_t>,
	      field<23, uint32_t>,
	      field<24, uint32_t>,
	      field<25, uint32_t>,
	      field<26, uint32_t>,
	      field<27, uint32_t>,
	      field<28, uint32_t>,
	      field<29, uint32_t>,
	      field<30, uint32_t>,
	      field<31, uint32_t>,
	      field<32, uint32_t>,

	      field<1, uint64_t>,
	      field<2, uint64_t>,
	      field<3, uint64_t>,
	      field<4, uint64_t>,
	      field<5, uint64_t>,
	      field<6, uint64_t>,
	      field<7, uint64_t>,
	      field<8, uint64_t>,
	      field<9, uint64_t>,
	      field<10, uint64_t>,
	      field<11, uint64_t>,
	      field<12, uint64_t>,
	      field<13, uint64_t>,
	      field<14, uint64_t>,
	      field<15, uint64_t>,
	      field<16, uint64_t>,
	      field<17, uint64_t>,
	      field<18, uint64_t>,
	      field<19, uint64_t>,
	      field<20, uint64_t>,
	      field<21, uint64_t>,
	      field<22, uint64_t>,
	      field<23, uint64_t>,
	      field<24, uint64_t>,
	      field<25, uint64_t>,
	      field<26, uint64_t>,
	      field<27, uint64_t>,
	      field<28, uint64_t>,
	      field<29, uint64_t>,
	      field<30, uint64_t>,
	      field<31, uint64_t>,
	      field<32, uint64_t>,
	      field<33, uint64_t>,
	      field<34, uint64_t>,
	      field<35, uint64_t>,
	      field<36, uint64_t>,
	      field<37, uint64_t>,
	      field<38, uint64_t>,
	      field<39, uint64_t>,
	      field<40, uint64_t>,
	      field<41, uint64_t>,
	      field<42, uint64_t>,
	      field<43, uint64_t>,
	      field<44, uint64_t>,
	      field<45, uint64_t>,
	      field<46, uint64_t>,
	      field<47, uint64_t>,
	      field<48, uint64_t>,
	      field<49, uint64_t>,
	      field<50, uint64_t>,
	      field<51, uint64_t>,
	      field<52, uint64_t>,
	      field<53, uint64_t>,
	      field<54, uint64_t>,
	      field<55, uint64_t>,
	      field<56, uint64_t>,
	      field<57, uint64_t>,
	      field<58, uint64_t>,
	      field<59, uint64_t>,
	      field<60, uint64_t>,
	      field<61, uint64_t>,
	      field<62, uint64_t>,
	      field<63, uint64_t>,
	      field<64, uint64_t>
	      > test;

typedef protocol<test> test_proto;

template <class Field_Traits>
static void check_field(typename Field_Traits::type::value_type expected,
			typename Field_Traits::type::value_type real,
			char const * const msg,
			unsigned char const * const buf) {
	typedef typename Field_Traits::type field;
	if (expected != real) {
		stringstream ss;
		ss << "field: " << std::dec << Field_Traits::index
		   << ", bits: " << field::bits
		   << ", offset: " << Field_Traits::byte_bit_offset << endl

		   << "expected value (" << std::hex << std::showbase << expected
		   << ") != real value (" << std::hex << std::showbase << real << ")" << endl

		   << msg << endl;

		ss << "field bytes: ";
		for (size_t i = 0; i < field::bytes; ++i) {
		      ss << std::hex << std::showbase << static_cast<uint16_t>(buf[i + Field_Traits::byte_index]) << ' ';
		   }
		throw logic_error(ss.str());
	}
}

// middle field, so previous and next fields exist
template <class PField_Traits,
	  class Field_Traits,
	  class NField_Traits,
	  class Protocol>
struct test_field
{
	static void test(unsigned char * const buf) {
		typedef PField_Traits pfield_traits;
		typedef NField_Traits nfield_traits;
		typedef Field_Traits field_traits;
		typedef typename Field_Traits::type field;
		const typename field::value_type expected_fv = lsb_mask<field::bits,
									0,
									typename field::value_type
									>::value;

		// set the Previous and Next field values
		Protocol::template field_value<pfield_traits::index>(buf, 0);
		Protocol::template field_value<nfield_traits::index>(buf, 0);

		Protocol::template field_value<field_traits::index>(buf, expected_fv);
		const typename field::value_type real_fv = Protocol::template field_value<field_traits::index>(buf);

		check_field<field_traits>(expected_fv,
					  real_fv,
					  "error setting field",
					  buf);
		check_field<pfield_traits>(0,
					   Protocol::template field_value<pfield_traits::index>(buf),
					   "setting field affected previous field",
					   buf);
		check_field<nfield_traits>(0,
					   Protocol::template field_value<nfield_traits::index>(buf),
					   "setting field affected next field",
					   buf);
	}
};

// Last field, so there's a previous field, but no next field
template <class PField_Traits,
	  class Field_Traits,
	  class Protocol>
struct test_field<PField_Traits, Field_Traits, void, Protocol>
{
	static void test(unsigned char * const buf) {
		typedef PField_Traits pfield_traits;
		typedef Field_Traits field_traits;
		typedef typename Field_Traits::type field;
		const typename field::value_type expected_fv = lsb_mask<field::bits,
									0,
									typename field::value_type
									>::value;

		// set the Previous field value
		Protocol::template field_value<pfield_traits::index>(buf, 0);

		Protocol::template field_value<field_traits::index>(buf, expected_fv);
		const typename field::value_type real_fv = Protocol::template field_value<field_traits::index>(buf);

		check_field<field_traits>(expected_fv,
					  real_fv,
					  "error setting field",
					  buf);
		check_field<pfield_traits>(0,
					   Protocol::template field_value<pfield_traits::index>(buf),
					   "setting field affected previous field",
					   buf);
	}
};

// First field to test and the number of fields is greater than 1, so
// there's no previous field, but there's a next field
template <class Field_Traits,
	  class NField_Traits,
	  class Protocol>
struct test_field<void, Field_Traits, NField_Traits, Protocol>
{
	static void test(unsigned char * const buf) {
		typedef NField_Traits nfield_traits;
		typedef Field_Traits field_traits;
		typedef typename Field_Traits::type field;
		const typename field::value_type expected_fv = lsb_mask<field::bits,
									0,
									typename field::value_type
									>::value;

		Protocol::template field_value<nfield_traits::index>(buf, 0);

		Protocol::template field_value<field_traits::index>(buf, expected_fv);
		const typename field::value_type real_fv = Protocol::template field_value<field_traits::index>(buf);

		check_field<field_traits>(expected_fv,
					  real_fv,
					  "error setting field",
					  buf);
		check_field<nfield_traits>(0,
					   Protocol::template field_value<nfield_traits::index>(buf),
					   "setting field affected next field",
					   buf);
	}
};

// Only one field, so there's no previous field and no next field
template <class Field_Traits,
	  class Protocol>
struct test_field<void, Field_Traits, void, Protocol>
{
	static void test(unsigned char * const buf) {
		typedef Field_Traits field_traits;
		typedef typename Field_Traits::type field;
		const typename field::value_type expected_fv = lsb_mask<field::bits,
									0,
									typename field::value_type
									>::value;

		Protocol::template field_value<field_traits::index>(buf, expected_fv);
		const typename field::value_type real_fv = Protocol::template field_value<field_traits::index>(buf);

		check_field<field_traits>(expected_fv,
					  real_fv,
					  "error setting field",
					  buf);
	}
};

// Field with a previous and a next field
template<size_t Prev_Field, size_t Field, class Protocol>
struct test_fields
{
	static void test(unsigned char * const buf) {
		test_field<typename Protocol::template field_traits<Prev_Field>,
			   typename Protocol::template field_traits<Field>,
			   typename Protocol::template field_traits<Field - 1>,
			   Protocol>::test(buf);
		test_fields<Field, Field - 1, Protocol>::test(buf);
	}
};

// First field to test, so there's no previous field, and there's a next field
template<size_t Field, class Protocol>
struct test_fields<0, Field, Protocol>
{
	static void test(unsigned char * const buf) {
		test_field<void,
			   typename Protocol::template field_traits<Field>,
			   typename Protocol::template field_traits<Field - 1>,
			   Protocol>::test(buf);
		test_fields<Field, Field - 1, Protocol>::test(buf);
	}
};

// Last field to test, so there's a previous field, but no next field
template<size_t Prev_Field, class Protocol>
struct test_fields<Prev_Field, 0, Protocol>
{
	static void test(unsigned char * const buf) {
		test_field<typename Protocol::template field_traits<Prev_Field>,
			   typename Protocol::template field_traits<0>,
			   void,
			   Protocol>::test(buf);
	}
};

// Only one field to test, so there's no previous field, and no next field
template <class Protocol>
struct test_fields<0, 0, Protocol>
{
	static void test(unsigned char * const buf) {
		test_field<void,
			   typename Protocol::template field_traits<0>,
			   void,
			   Protocol>::test(buf);
	}
};

template<class Protocol>
void test_protocol(unsigned char * const buf)
{
	test_fields<0, Protocol::traits::fields - 1, Protocol>::test(buf);
}

template <size_t Bit, size_t Field_Offset, size_t Field_Size, class Type>
void test_bit(unsigned char * const buf) {
	typedef typename protocol_helper::field_value<Field_Size, Field_Offset, Type>::type fv_type;
	Type val = msb_mask<1, numeric_limits<Type>::digits - Field_Size + Bit, Type>::value;

	fv_type::set(buf, val);

	if (val != fv_type::get(buf)) {
	   stringstream ss;
	   ss << "bit: " << std::dec << Bit <<
	      ", offset: " << std::dec << Field_Offset <<
	      ", field size: " << std::dec << Field_Size <<
	      ", type size: " << std::dec << numeric_limits<Type>::digits <<
	      ", expected value: " << std::hex << std::showbase << val <<
	      ", actual value: " << std::hex << std::showbase << fv_type::get(buf) << endl;

	   throw logic_error(ss.str());
	}	
}

template <size_t Bit, size_t Field_Offset, size_t Field_Size, class Type>
struct test_bits
{
	static void test(unsigned char * const buf) {
		test_bit<Bit, Field_Offset, Field_Size, Type>(buf);
		test_bits<Bit - 1, Field_Offset, Field_Size, Type>::test(buf);
	}
};

template <size_t Field_Offset, size_t Field_Size, class Type>
struct test_bits<0, Field_Offset, Field_Size, Type>
{
	static void test(unsigned char * const buf) {
		test_bit<0, Field_Offset, Field_Size, Type>(buf);
	}
};

template<size_t Field_Offset, size_t Field_Size, class Type>
struct test_field_offset
{
	static void test() {
		unsigned char buf[(Field_Offset + Field_Size) % 8 ? ((Field_Offset + Field_Size) / 8) + 1 : (Field_Offset + Field_Size) / 8];

		memset(buf, 1, sizeof(buf));
		test_bits<Field_Size - 1, Field_Offset, Field_Size, Type>::test(buf);

		memset(buf, 0, sizeof(buf));
		test_bits<Field_Size - 1, Field_Offset, Field_Size, Type>::test(buf);

		test_field_offset<Field_Offset - 1, Field_Size, Type>::test();
	}
};

template<size_t Field_Size, class Type>
struct test_field_offset<0, Field_Size, Type>
{
	static void test() {
		unsigned char buf[(Field_Size) % 8 ? ((Field_Size) / 8) + 1 : (Field_Size) / 8];

		memset(buf, 1, sizeof(buf));
		test_bits<Field_Size - 1, 0, Field_Size, Type>::test(buf);

		memset(buf, 0, sizeof(buf));
		test_bits<Field_Size - 1, 0, Field_Size, Type>::test(buf);
	}
};

template <size_t Field_Size, class Type>
struct test_field_size
{
	static void test() {
		test_field_offset<7, Field_Size, Type>::test();
		test_field_size<Field_Size - 1, Type>::test();
	}
};

template<class Type>
struct test_field_size<0, Type>
{
	static void test() {
		return;
	}
};

template <size_t I, class Types_Tuple>
struct test_type
{
	static void test() {
		test_field_size<numeric_limits<typename tuple_element<I, Types_Tuple>::type>::digits, typename tuple_element<I, Types_Tuple>::type>::test();
		test_type<I - 1, Types_Tuple>::test();
	}
};

template <class Types_Tuple>
struct test_type<0, Types_Tuple>
{
	static void test() {
		test_field_size<numeric_limits<typename tuple_element<0, Types_Tuple>::type>::digits, typename tuple_element<0, Types_Tuple>::type>::test();
	}
};

template <class Types_Tuple>
void test_types()
{
	test_type<tuple_size<Types_Tuple>::value - 1, Types_Tuple>::test();
}

int main()
try {
	test_proto::traits::array_type proto_buf;
	test_protocol<test_proto>(proto_buf.data());
	return 0;

} catch(exception& ex) {
	cerr << ex.what() << endl;
	return 1;
}
