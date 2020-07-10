#include "Package.h"

const int FrameLength = 512;
class Frame {
public:
    std::string Sync_Word;
    std::string Frame_Leader;
    std::string Frame_Sub_Leader;
    Package Data[3];
    std::string Control_Field;

    Frame();
    Frame(const std::string& frame_string);
    ~Frame();
};