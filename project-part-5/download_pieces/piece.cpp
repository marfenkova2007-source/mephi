#include "byte_tools.h"
#include "piece.h"
#include <iostream>
#include <algorithm>
#include <openssl/sha.h>

namespace {
constexpr size_t BLOCK_SIZE = 1 << 14;
}

Piece::Piece(size_t index, size_t length, std::string hash): index_(index), length_(length), hash_(hash){
    size_t k = 0;
    while (k<length_){
        Block new_block;
        new_block.piece = index_;
        new_block.offset = k;
        new_block.status = Block::Missing;
        new_block.data = "";
        if (length_ - k < BLOCK_SIZE) {
            new_block.length = length_ -k;
        }
        else {
            new_block.length = BLOCK_SIZE;
        }
        blocks_.push_back(new_block);
        k+=new_block.length;
    }
} ;
    /*
     * Дать указатель на отсутствующий (еще не скачанный и не запрошенный) блок
     */
Block* Piece::FirstMissingBlock(){
    for (size_t i=0; i<blocks_.size(); ++i){
        if (blocks_[i].status== Block::Missing){
            blocks_[i].status = Block::Pending;
            return &blocks_[i];
        }
    }
    return nullptr;
};

    /*
     * Получить порядковый номер части файла
     */
size_t Piece::GetIndex() const{
    return index_;
};

    /*
     * Сохранить скачанные данные для какого-то блока
     */
void Piece::SaveBlock(size_t blockOffset, std::string data){
    for (size_t i=0; i<blocks_.size(); ++i){
        if (blocks_[i].offset==blockOffset){
            blocks_[i].status = Block::Retrieved;
            ++kol_skach;
            blocks_[i].data = data;
            break;
        }
    }
};

    /*
     * Скачали ли уже все блоки
     */
bool Piece::AllBlocksRetrieved() const{
    if (kol_skach==blocks_.size()){
        return true;
    }
    return false;

};

    /*
     * Получить скачанные данные для части файла
     */
std::string Piece::GetData() const{
    std::string skach = "";
    for (size_t i=0; i<blocks_.size(); ++i){
        skach+= blocks_[i].data;
    }
    return skach;

};

    /*
     * Посчитать хеш по скачанным данным
     */
std::string Piece::GetDataHash() const{
    std::string skach = GetData();
    return CalculateSHA1(skach);

};

    /*
     * Получить хеш для части из .torrent файла
     */
const std::string& Piece::GetHash() const{
    return hash_;
};

/*
     * Совпадает ли хеш скачанных данных с ожидаемым
     */
bool Piece::HashMatches() const{
    if (GetDataHash()==hash_){
        return true;
    }
    return false;
};

    /*
     * Удалить все скачанные данные и отметить все блоки как Missing
     */
void Piece::Reset(){
    kol_skach = 0;
    for (size_t i=0; i<blocks_.size(); ++i){
        blocks_[i].data = "";
        blocks_[i].status = Block::Missing;
    }
};