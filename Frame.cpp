#include <iostream>
#include "Frame.h"

Frame::Frame() {
	Sync_Word = "";
	Frame_Leader = "";
	Frame_Sub_Leader = "";
	for (int i = 0; i < 3; ++i) {
		Data[i] = Package();
	}
	Frame::Control_Field = "";
}

Frame::Frame(const std::string& frame_string) {
	/*if (frame_string.size() != FrameLength) {
		std::cout << "Illegal (Frame)String" << std::endl;
		return;
	}*/
	SID = frame_string.substr(13, 4);
	DID = frame_string.substr(17, 4);
	BID = frame_string.substr(21, 4);
	Time_Scale = frame_string.substr(43, 4);
	Sync_Word = frame_string.substr(0 + 47, 8);
	Frame_Leader = frame_string.substr(8 + 47, 12);
	Frame_Sub_Leader = frame_string.substr(20 + 47, 10);
	for (int i = 0; i < 3; ++i) {
		std::string tmp = frame_string.substr(30 + 47 + i * (PackageLength + 2), PackageLength);
		Data[i] = Package(tmp);
	}
	//包间隔判断
	/*
	if ...
	*/
	Control_Field = frame_string.substr(508, 4);
}

Frame::~Frame() {

}
