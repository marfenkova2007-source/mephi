#include "byte_tools.h"
#include <openssl/sha.h>
#include <vector>
#include <stdexcept>

int BytesToInt(std::string_view bytes) {
    if (bytes.size() < 4) {
        throw std::runtime_error("Должно быть не меньше 4");
    }
    return (static_cast<unsigned char>(bytes[0])<<24) | (static_cast<unsigned char>(bytes[1])<<16) |
           (static_cast<unsigned char>(bytes[2])<<8) | (static_cast<unsigned char>(bytes[3]));
}

std::string IntToBytes(uint32_t n) {
    std::string res;
    res.resize(4);
    res[0] = static_cast<char>((n>>24) & 0xFF);
    res[1] = static_cast<char>((n>>16) & 0xFF);
    res[2] = static_cast<char>((n>>8) & 0xFF);
    res[3] = static_cast<char>(n & 0xFF);
    return res;
}

std::string CalculateSHA1(const std::string& msg) {
    unsigned char hash[20];
    const unsigned char* data = reinterpret_cast<const unsigned char*>(msg.c_str());
    size_t len = msg.size();
    SHA1(data, len, hash);
    std::string result(reinterpret_cast<char*>(hash), 20);

    return result;

}
