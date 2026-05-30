#include "byte_tools.h"
#include <stdexcept>

int BytesToInt(std::string_view bytes) {
    if (bytes.size() < 4) {
        throw std::runtime_error("Должно быть не меньше 4");
    }
    return (static_cast<unsigned char>(bytes[0])<<24) | (static_cast<unsigned char>(bytes[1])<<16) |
           (static_cast<unsigned char>(bytes[2])<<8) | (static_cast<unsigned char>(bytes[3]));
}
