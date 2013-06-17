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

typedef protocol<test> test_proto;

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
    unsigned char buf[11];

    memset(buf, 1, sizeof(buf));

    try {
	test_protocol<test_proto>(buf);
    }
    catch(exception& ex)
    {
	cerr << ex.what() << endl;
	return 1;
    }

    return 0;
}
