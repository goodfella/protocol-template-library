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

typedef protocol<mpeg2_ts_tpl> ts_proto;

enum class mpeg2_ts {
    sync_byte = 0,
    tei,
    pusi,
    transport_priority,
    pid,
};

int main()
{
    cout << tuple_element<0, mpeg2_ts_tpl>::type::bits << endl;
    cout << tuple_element<1, mpeg2_ts_tpl>::type::bits << endl;
    cout << bit_offset<4, mpeg2_ts_tpl>::value << endl;
    cout << bit_offset<1, mpeg2_ts_tpl>::value << endl;
    cout << bit_offset<0, mpeg2_ts_tpl>::value << endl;
    cout << byte_offset<1, mpeg2_ts_tpl>::value << endl;
    cout << byte_offset<0, mpeg2_ts_tpl>::value << endl;

    unsigned char b[3];
    
    b[0] = 0x47;
    b[1] = 0xb0;
    b[2] = 0x01;

    cout << std::hex << std::showbase << static_cast<unsigned short>(ts_proto::field_value<mpeg2_ts::sync_byte>(b)) << endl;
    cout << std::hex << std::showbase << ts_proto::field_value<mpeg2_ts::tei>(b) << endl;
    cout << std::hex << std::showbase << ts_proto::field_value<mpeg2_ts::pusi>(b) << endl;
    cout << std::hex << std::showbase << ts_proto::field_value<mpeg2_ts::transport_priority>(b) << endl;
    cout << std::hex << std::showbase << ts_proto::field_value<mpeg2_ts::pid>(b) << endl;
}
