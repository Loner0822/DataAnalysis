#pragma once
#include <vector>
#include "Package.h"

const int FrameLength = 512;
class Frame {
public:
	std::string SID;
	std::string DID;
	std::string BID;
	std::string Time_Scale;
	std::string Sync_Word;
	std::string Frame_Leader;
	std::string Frame_Sub_Leader;
	Package Data[3];
	std::string Control_Field;

	Frame();
	Frame(const std::string& frame_string);
	~Frame();
};
