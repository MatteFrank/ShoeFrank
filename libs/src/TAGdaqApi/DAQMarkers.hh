#ifndef DAQMARKERS_HH
#define DAQMARKERS_HH

#include <map>
#include <string>

// Standard markers
const unsigned int FileHeader  = 0x1234aaaa;
const unsigned int hFirst      = 0x1234aabb;
const unsigned int hSecond     = 0x1234aabc;
const unsigned int FileHeader2 = 0x1234bbbb;
const unsigned int EventMarker = 0x1234cccc;
const unsigned int EventHeader = 0xaa1234aa;
const unsigned int ROSHeader1  = 0xdd1234dd;
const unsigned int ROSHeader2  = 0xee1234ee;
const unsigned int EndOfFile   = 0x1234eeee;

// Fragment ID
const unsigned int EventHeaderID = 0x00460000;
// Trigger
const unsigned int dataV2495 = 0x00463000;
//TDC
const unsigned int dataV1190 = 0x00463100;
// DE0/DE10
const unsigned int dataVTX   = 0x00463300;
// MSD
const unsigned int dataMSD   = 0x00463400;
//Remote board with PC
const unsigned int dataWD    = 0x00463200;
// FADC
const unsigned int dataV1720 = 0x00463500;
// Generic
const unsigned int dataEmpty = 0x00463800;

const unsigned int FRAGTYPES = 8;


#endif
