#include "bencode.h"
#include <cctype>
#include <stdexcept>

namespace Bencode {

std::string ReadWord(const std::string& data, size_t& pos) {
    if (pos >= data.size()) {  
        throw std::runtime_error("Вышли за рамки");
    }
    size_t l = pos;
    while (pos < data.size() && data[pos] != ':'){
        if (!std::isdigit(static_cast<unsigned char>(data[pos]))){
            throw std::runtime_error("Ошибка длины");
        }                 
        ++pos;
    }
    if (pos >= data.size()) {
        throw std::runtime_error("Нет двоеточия");
    }
    std::string value = data.substr(l, pos - l);
    size_t len_value = std::stoull(value);
    ++pos;
    if (pos + len_value > data.size()) {  
        throw std::runtime_error("Строка слишком длинная");
    }
    std::string res = data.substr(pos, len_value);
    pos += len_value;
    return res;
}

void SkipValue(const std::string& data, size_t& pos) {
    if (pos >= data.size()){
        throw std::runtime_error("Вышли за границу");
    }
    if (data[pos] == 'i'){
        ++pos;
        while (pos < data.size() && data[pos] != 'e'){
            ++pos;
        }
        if (pos >= data.size()){
            throw std::runtime_error("Error int");
        }
        ++pos;
    }
    else if (data[pos] == 'l' || data[pos] == 'd'){
        ++pos;
        while (pos < data.size() && data[pos] != 'e'){
            SkipValue(data, pos);
        }
        if (pos >= data.size()){
            throw std::runtime_error("Ошибка при пропуске словаря/списка");
        }
        ++pos;
    }
    else if (std::isdigit(static_cast<unsigned char>(data[pos]))){
        ReadWord(data, pos);
    }
    else{
        throw std::runtime_error("Неизвестный тип данных в bencode");
    }
}

}