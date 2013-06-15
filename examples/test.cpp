#include <iostream>
#include <tuple>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <string>
#include "protocol_helper.hpp"

using namespace std;
using namespace protocol_helper;

typedef tuple<
    field<8, unsigned short>, // field 0
    field<1, bool>,           // field 1
    field<6, unsigned short>, // field 2
    field<17, unsigned int>,  // field 3
    field<9, unsigned short>, // field 4
    field<7, unsigned short>, // field 5
    field<16, unsigned int>,  // field 6
    field<1, bool>,           // field 7
    field<7, unsigned short>, // field 8
    field<1, bool>,           // field 9
    field<9, unsigned short>  // field 10
    > test;

typedef protocol<test> test_proto;

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

    memset(buf, 0, sizeof(buf));

    // Each field is set so that its first bit and last bit are 1.
    // This makes it easy to check protocol::field_value
    buf[0] = 0x81;  // field 0
    buf[1] = 0x80;  // field 1
    buf[1] |= 0x42; // field 2
    buf[1] |= 0x01; // beginning of field 3
    buf[2] = 0x00;  // middle of field 3
    buf[3] = 0x01;  // end of field 3
    buf[4] = 0x80;  // beginning of field 4
    buf[5] = 0x80;  // end of field 4
    buf[5] |= 0x41; // field 5
    buf[6] = 0x80;  // beginning of field 6
    buf[7] = 0x01;  // end of field 6
    buf[8] = 0x80;  // field 7
    buf[8] |= 0x41; // field 8
    buf[9] |= 0x80; // field 9
    buf[9] |= 0x40; // beginning of field 10
    buf[10] = 0x40; // end of field 10

    cout << "protocol bit length: " << test_proto::bit_length << endl;
    cout << "protocol byte length: " << test_proto::byte_length << endl;
    cout << "field 0: " << std::hex << std::showbase << test_proto::field_value<0>(buf) << endl;
    cout << "field 1: " << std::hex << std::showbase << test_proto::field_value<1>(buf) << endl;
    cout << "field 2: " << std::hex << std::showbase << test_proto::field_value<2>(buf) << endl;
    cout << "field 3: " << std::hex << std::showbase << test_proto::field_value<3>(buf) << endl;
    cout << "field 4: " << std::hex << std::showbase << test_proto::field_value<4>(buf) << endl;
    cout << "field 5: " << std::hex << std::showbase << test_proto::field_value<5>(buf) << endl;
    cout << "field 6: " << std::hex << std::showbase << test_proto::field_value<6>(buf) << endl;
    cout << "field 7: " << std::hex << std::showbase << test_proto::field_value<7>(buf) << endl;
    cout << "field 8: " << std::hex << std::showbase << test_proto::field_value<8>(buf) << endl;
    cout << "field 9: " << std::hex << std::showbase << test_proto::field_value<9>(buf) << endl;
    cout << "field 10: " << std::hex << std::showbase << test_proto::field_value<10>(buf) << endl;

    try {
	check_value<0>(buf);
	check_value<1>(buf);
	check_value<2>(buf);
	check_value<3>(buf);
	check_value<4>(buf);
	check_value<5>(buf);
	check_value<6>(buf);
	check_value<7>(buf);
	check_value<8>(buf);
	check_value<9>(buf);
	check_value<10>(buf);
    }
    catch(exception& ex)
    {
	cerr << ex.what() << endl;
	return 1;
    }

    return 0;
}
