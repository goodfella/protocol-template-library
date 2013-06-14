#include <iostream>
#include <tuple>
#include <cstring>
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

int main()
{
    unsigned char buf[11];

    memset(buf, 0, sizeof(buf));

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

    cout << "field 0: " << std::hex << std::showbase << test_proto::get_field<0>(buf) << endl;
    cout << "field 1: " << std::hex << std::showbase << test_proto::get_field<1>(buf) << endl;
    cout << "field 2: " << std::hex << std::showbase << test_proto::get_field<2>(buf) << endl;
    cout << "field 3: " << std::hex << std::showbase << test_proto::get_field<3>(buf) << endl;
    cout << "field 4: " << std::hex << std::showbase << test_proto::get_field<4>(buf) << endl;
    cout << "field 5: " << std::hex << std::showbase << test_proto::get_field<5>(buf) << endl;
    cout << "field 6: " << std::hex << std::showbase << test_proto::get_field<6>(buf) << endl;
    cout << "field 7: " << std::hex << std::showbase << test_proto::get_field<7>(buf) << endl;
    cout << "field 8: " << std::hex << std::showbase << test_proto::get_field<8>(buf) << endl;
    cout << "field 9: " << std::hex << std::showbase << test_proto::get_field<9>(buf) << endl;
    cout << "field 10: " << std::hex << std::showbase << test_proto::get_field<10>(buf) << endl;
}
