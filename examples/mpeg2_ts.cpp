#include <tuple>
#include <iostream>
#include "protocol_helper.hpp"
using namespace std;
using namespace protocol_helper;

// Partial MPEG 2 transport stream header
typedef tuple<field<8, unsigned char>,  // sync byte
	      field<1, bool>,           // transport error indicator
	      field<1, bool>,           // payload unit start indicator
	      field<1, bool>,           // transport priority
	      field<13, unsigned short> // PID
	      > mpeg2_ts_tpl;

int main()
{
    cout << tuple_element<0, mpeg2_ts_tpl>::type::bits << endl;
    cout << tuple_element<1, mpeg2_ts_tpl>::type::bits << endl;
    cout << bit_offset<4, mpeg2_ts_tpl>::value << endl;
    cout << bit_offset<1, mpeg2_ts_tpl>::value << endl;
    cout << bit_offset<0, mpeg2_ts_tpl>::value << endl;
    cout << byte_offset<1, mpeg2_ts_tpl>::value << endl;
    cout << byte_offset<0, mpeg2_ts_tpl>::value << endl;

    cout << std::hex << static_cast<int>(lbit_mask<0>::value) << endl;
    cout << std::hex << static_cast<int>(lbit_mask<1>::value) << endl;
    cout << std::hex << static_cast<int>(lbit_mask<2>::value) << endl;
    cout << std::hex << static_cast<int>(lbit_mask<3>::value) << endl;
    cout << std::hex << static_cast<int>(lbit_mask<4>::value) << endl;
    cout << std::hex << static_cast<int>(lbit_mask<8>::value) << endl;

    unsigned char b[3];
    
    b[1] = 0x10;
    b[2] = 0x01;

    cout << std::hex << get_field<tuple_element<4, mpeg2_ts_tpl>::type::bits, bit_offset<4, mpeg2_ts_tpl>::value, short>::value(&b[1]) << endl;
}
