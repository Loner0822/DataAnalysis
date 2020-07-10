#include <iostream>
#include "Frame.h"

Frame::Frame() {
    Frame::Sync_Word = "";
    Frame::Frame_Leader = "";
    Frame::Frame_Sub_Leader = "";
    for (int i = 0; i < 3; ++i) {
        Data[i] = Package();
    }
    Frame::Control_Field = "";
}

Frame::Frame(const std::string& frame_string) {
    if (frame_string.size() != FrameLength) {
        std::cout << "Illegal (Frame)String" << std::endl;
        return;
    }
    Frame::Sync_Word = frame_string.substr(0, 8);
    //ͬ�����ж�
    /*
    if ...
    */
    Frame::Frame_Leader = frame_string.substr(8, 12);
    Frame::Frame_Sub_Leader = frame_string.substr(20, 10);
    for (int i = 0; i < 3; ++i) {
        std::string tmp = frame_string.substr(30 + i * (PackageLength + 2), PackageLength);
        Frame::Data[i] = Package(tmp);
    }
    //������ж�
    /*
    if ...
    */
    Frame::Control_Field = frame_string.substr(508, 4);
}

Frame::~Frame() {

}