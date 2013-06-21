#include <cstring>
#include <cstdint>
#include <iostream>
#include <tuple>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <string>
#include "protocol_helper.hpp"

using namespace std;
using namespace protocol_helper;

typedef tuple<
    field<8, uint16_t>, // field 0
    field<1, bool>,           // field 1
    field<6, uint16_t>, // field 2
    field<17, uint32_t>,  // field 3
    field<9, uint16_t>, // field 4
    field<7, uint16_t>, // field 5
    field<16, uint16_t>,  // field 6
    field<1, bool>,           // field 7
    field<7, uint16_t>, // field 8
    field<1, bool>,           // field 9
    field<9, uint16_t>  // field 10
    > test;

typedef protocol<big_endian, test> test_proto;

template<size_t Bit, size_t Field, class Protocol>
struct test_field_bit
{
    static void test(unsigned char * const buf)
    {
	typedef typename field_type<Field, typename Protocol::tuple>::type ftype;

	ftype val = msb_mask<1, numeric_limits<ftype>::digits - field_bits<Field, typename Protocol::tuple>::value + Bit, ftype>::value;

	test_proto::field_value<Field>(buf, val);

	if (val != test_proto::field_value<Field>(buf)) {
	    stringstream ss;
	    ss << "field: " << std::dec << Field <<
		", Bit: " << std::dec << Bit <<
		std::hex << std::showbase << " test failure: expected = " << val <<
		", actual = " << test_proto::field_value<Field>(buf);

	    throw logic_error(ss.str());
	}

	test_field_bit<Bit - 1, Field, Protocol>::test(buf);
    }
};

template<size_t Field, class Protocol>
struct test_field_bit<0, Field, Protocol>
{
    static void test(unsigned char * const buf)
    {
	typedef typename field_type<Field, typename Protocol::tuple>::type ftype;

	ftype val = msb_mask<1, numeric_limits<ftype>::digits - field_bits<Field, typename Protocol::tuple>::value, ftype>::value;

	test_proto::field_value<Field>(buf, val);

	if (val != test_proto::field_value<Field>(buf)) {
	    stringstream ss;
	    ss << "field: " << std::dec << Field <<
		", Bit: " << std::dec << 0 <<
		std::hex << std::showbase << " test failure: expected = " << val <<
		", actual = " << test_proto::field_value<Field>(buf);

	    throw logic_error(ss.str());
	}

	return;
    }
};

template<size_t Field, class Protocol>
struct test_field
{
    static void test(unsigned char * const buf)
    {
	test_field_bit<field_bits<Field, typename Protocol::tuple>::value - 1, Field, Protocol>::test(buf);
	test_field<Field - 1, Protocol>::test(buf);
    }
};

template<class Protocol>
struct test_field<0, Protocol>
{
    static void test(unsigned char * const buf)
    {
	test_field_bit<field_bits<0, typename Protocol::tuple>::value - 1, 0, Protocol>::test(buf);
    }
};

template<class Protocol>
void test_protocol(unsigned char * const buf)
{
    test_field<Protocol::field_count - 1, Protocol>::test(buf);
}

template <size_t Bit, size_t Field_Offset, size_t Field_Size, class Type>
struct test_bit
{
    static void test(unsigned char * const buf) {
	typedef field_value<((Field_Offset % bits_per_byte::value) + Field_Size > bits_per_byte::value), big_endian, Field_Size, Field_Size, Field_Offset, Type> fv_type;
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

	test_bit<Bit - 1, Field_Offset, Field_Size, Type>::test(buf);
    }
};

template <size_t Field_Offset, size_t Field_Size, class Type>
struct test_bit<0, Field_Offset, Field_Size, Type>
{
    static void test(unsigned char * const buf) {
	typedef field_value<((Field_Offset % bits_per_byte::value) + Field_Size > bits_per_byte::value), big_endian, Field_Size, Field_Size, Field_Offset, Type> fv_type;
	Type val = msb_mask<1, numeric_limits<Type>::digits - Field_Size, Type>::value;

	fv_type::set(buf, val);

	if (val != fv_type::get(buf)) {
	    stringstream ss;
	    ss << "bit: " << std::dec << 0 <<
		", offset: " << std::dec << Field_Offset <<
		", field size: " << std::dec << Field_Size <<
		", type size: " << std::dec << numeric_limits<Type>::digits <<
		", expected value: " << std::hex << std::showbase << val <<
		", actual value: " << std::hex << std::showbase << fv_type::get(buf) << endl;

	    throw logic_error(ss.str());
	}
    }
};

template<size_t Field_Offset, size_t Field_Size, class Type>
struct test_field_offset
{
    static void test() {
	unsigned char buf[(Field_Offset + Field_Size) % 8 ? ((Field_Offset + Field_Size) / 8) + 1 : (Field_Offset + Field_Size) / 8];

	memset(buf, 1, sizeof(buf));
	test_bit<Field_Size - 1, Field_Offset, Field_Size, Type>::test(buf);

	memset(buf, 0, sizeof(buf));
	test_bit<Field_Size - 1, Field_Offset, Field_Size, Type>::test(buf);

	test_field_offset<Field_Offset - 1, Field_Size, Type>::test();
    }
};

template<size_t Field_Size, class Type>
struct test_field_offset<0, Field_Size, Type>
{
    static void test() {
	unsigned char buf[(Field_Size) % 8 ? ((Field_Size) / 8) + 1 : (Field_Size) / 8];

	memset(buf, 1, sizeof(buf));
	test_bit<Field_Size - 1, 0, Field_Size, Type>::test(buf);

	memset(buf, 0, sizeof(buf));
	test_bit<Field_Size - 1, 0, Field_Size, Type>::test(buf);
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

template<size_t I>
void check_value(unsigned char const * const buf)
{
    typename field_type<I, test>::type value = test_proto::field_value<I>(buf);
    typename field_type<I, test>::type bit1 = value >> (field_bits<I, test>::value - 1); 
    typename field_type<I, test>::type bit2 = value & ~(numeric_limits<typename field_type<I, test>::type>::max() -1);

    if (bit1 != 1 || bit2 != 1){
	stringstream ss;
	ss << I << ' ';
	throw logic_error(string("field ") + ss.str() + string("failed check"));
    }
}

int main()
{
    unsigned char buf[(numeric_limits<uint64_t>::digits / 8) + 1];

    try {
	test_types<tuple<uint16_t> >();

	memset(buf, 0, sizeof(buf));
	test_bit<0, 7, numeric_limits<uint64_t>::digits, uint64_t>::test(buf);

	memset(buf, 0, sizeof(buf));
	test_bit<0, 6, numeric_limits<uint64_t>::digits, uint64_t>::test(buf);

	memset(buf, 0, sizeof(buf));
	test_bit<0, 5, numeric_limits<uint64_t>::digits, uint64_t>::test(buf);

	memset(buf, 0, sizeof(buf));
	test_bit<0, 4, numeric_limits<uint64_t>::digits, uint64_t>::test(buf);

	memset(buf, 0, sizeof(buf));
	test_bit<0, 3, numeric_limits<uint64_t>::digits, uint64_t>::test(buf);

	memset(buf, 0, sizeof(buf));
	test_bit<0, 2, numeric_limits<uint64_t>::digits, uint64_t>::test(buf);

	memset(buf, 0, sizeof(buf));
	test_bit<0, 1, numeric_limits<uint64_t>::digits, uint64_t>::test(buf);

	memset(buf, 0, sizeof(buf));
	test_bit<0, 0, numeric_limits<uint64_t>::digits, uint64_t>::test(buf);

	test_protocol<test_proto>(buf);
    }
    catch(exception& ex)
    {
	cerr << ex.what() << endl;
	return 1;
    }

    return 0;
}
