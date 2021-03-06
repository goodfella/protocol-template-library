#include <tuple>
#include <iostream>
#include "ptl.hpp"
using namespace std;
using namespace ptl;

// Partial MPEG 2 transport stream header
using mpeg2_ts_tpl = tuple<field<8, unsigned char>,  // sync byte
			   field<1, bool>,           // transport error indicator
			   field<1, bool>,           // payload unit start indicator
			   field<1, bool>,           // transport priority
			   field<13, unsigned short> // PID
			   >;

using ts_proto = protocol<mpeg2_ts_tpl>;

struct mpeg2_ts
{
	enum : unsigned int {
		sync_byte = 0,
			tei,
			pusi,
			transport_priority,
			pid,
			};
};

int main()
{
	cout << tuple_element<0, mpeg2_ts_tpl>::type::bits << endl;
	cout << tuple_element<1, mpeg2_ts_tpl>::type::bits << endl;
	cout << field_bit_offset<4, mpeg2_ts_tpl>::value << endl;
	cout << field_bit_offset<1, mpeg2_ts_tpl>::value << endl;
	cout << field_bit_offset<0, mpeg2_ts_tpl>::value << endl;
	cout << field_first_byte<1, mpeg2_ts_tpl>::value << endl;
	cout << field_first_byte<0, mpeg2_ts_tpl>::value << endl;

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
