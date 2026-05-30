#pragma once

#include "peer.h"
#include "torrent_file.h"
#include <string>
#include <vector>
#include <openssl/sha.h>
#include <fstream>
#include <variant>
#include <list>
#include <map>
#include <sstream>
#include <cpr/cpr.h>
#include <stdexcept>
#include <cctype> 

std:: string Read_the_word(const std::string& data, size_t& pos){
    if (pos >= data.size()) {  
        throw std::runtime_error("Вышли за рамки");
    }
    size_t l = pos;
    while (pos < data.size() and data[pos] != ':'){
        if (!std::isdigit(static_cast<unsigned char>(data[pos]))){
            throw std::runtime_error("Ошибка длины");
        }                 
        ++pos;
    }
    if (pos >= data.size()) {
        throw std::runtime_error("Нет двоеточия");
    }
    std::string value = data.substr(l, pos-l);
    size_t len_value = std::stoull(value);
    ++pos;
    if (pos + len_value > data.size()) {  
        throw std::runtime_error("Строка слишком длинная");
    }
    std::string res = data.substr(pos, len_value);
    pos+=len_value;
    return res;

}

void Skip(const std::string& f, size_t& i){
    if(i>=f.size()){
        throw std::runtime_error("Вышли за границу");
    }
    if (f[i]=='i'){
        ++i;
        while (i<f.size() and f[i]!='e'){
            ++i;
        }
        if (i>=f.size()){
            throw std::runtime_error("Error int");
        }
        ++i;
    }
    else if( f[i]=='l' or f[i]=='d'){
        ++i;
        while (i<f.size() and f[i]!='e'){
            Skip(f, i);
        }
        if (i>=f.size()){
            throw std::runtime_error("Ошибка 105");
        }
        ++i;
    }
    else if(std::isdigit(static_cast<unsigned char>(f[i]))){
        Read_the_word(f, i);
    }
    else{
        throw std::runtime_error("Не понятно как попали сюда");
    }

}



class TorrentTracker {
public:
    /*
     * url - адрес трекера, берется из поля announce в .torrent-файле
     */
    TorrentTracker(const std::string& url): url_(url){};

    /*
     * Получить список пиров у трекера и сохранить его для дальнейшей работы.
     * Запрос пиров происходит посредством HTTP GET запроса, данные передаются в формате bencode.
     * Такой же формат использовался в .torrent файле.
     * Подсказка: посмотрите, что было написано в main.cpp в домашнем задании torrent-file
     *
     * tf: структура с разобранными данными из .torrent файла из предыдущего домашнего задания.
     * peerId: id, под которым представляется наш клиент.
     * port: порт, на котором наш клиент будет слушать входящие соединения (пока что мы не слушаем и на этот порт никто
     *  не сможет подключиться).
     */

    std::string perevod(const std::string& stroka){
        std::string help ="0123456789ABCDEF";
        std::string safe = "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM-._~";
        std::string new_stroka = "";
        for (unsigned char i: stroka){
            if (safe.find(i) != std::string::npos){
                new_stroka+=i;
            }
            else{
                new_stroka += '%';
                new_stroka+=help[i / 16];
                new_stroka+=help[i % 16];
            }
        }
        return new_stroka;
    }


    void UpdatePeers(const TorrentFile& tf, std::string peerId, int port){
        std::string s = url_ + "?info_hash=" + perevod(tf.infoHash) +
             "&peer_id=" + perevod(peerId) +
             "&port=" + std::to_string(port) +
             "&uploaded=0" + "&downloaded=0" +
             "&left=" + std::to_string(tf.length) +
             "&compact=1";

        cpr::Response ans = cpr::Get(cpr::Url{s});
        if (ans.status_code!=200) {
            throw std::runtime_error("Код ошибки");
        }
        std::string peers_data;
        size_t pos = 0;
        
        if (ans.text[pos] == 'd') {
            pos++;
            while (pos<ans.text.size() and ans.text[pos] != 'e') {
                std::string key = Read_the_word(ans.text, pos); 
                
                if (key == "peers") {
                    peers_data = Read_the_word(ans.text, pos);
                }
                else {
                    Skip(ans.text, pos);
                }
            }
        }
        if (peers_data.empty()) {
            return;
        }
        peers_.clear();

        for (size_t i=0; i+6<=peers_data.size(); i+=6) {
            Peer par;
            par.ip = std::to_string((unsigned char)peers_data[i]) + "." +
                    std::to_string((unsigned char)peers_data[i+1]) + "." +
                    std::to_string((unsigned char)peers_data[i+2]) + "." +
                    std::to_string((unsigned char)peers_data[i+3]);
            par.port = ((unsigned char)peers_data[i+4]<<8)|(unsigned char)peers_data[i+5];
            peers_.push_back(par);
        }
    };

    /*
     * Отдает полученный ранее список пиров
     */
    const std::vector<Peer>& GetPeers() const{
        return peers_;
    };

private:
    std::string url_;
    std::vector<Peer> peers_;
};

TorrentFile LoadTorrentFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Файл не открылся");
    }
    std::stringstream information;
    information<<file.rdbuf();
    std::string f = information.str();

    TorrentFile torrent;
    size_t i = 0;

    if (f[i]!='d'){
        throw std::runtime_error("Проблема чтения файла, должен начинаться с d");
    }
    ++i;
    while (i < f.size() and f[i]!='e'){
        std::string key = Read_the_word(f, i);
        if (key=="announce"){
            torrent.announce = Read_the_word(f, i);
        }
        else if (key=="comment"){
            torrent.comment = Read_the_word(f, i);
        }
        else if (key=="info"){
            if (f[i]!='d'){
                throw std::runtime_error("Словарь должен начинаться с d и заканчиваться e");
            }
            size_t start = i;
            ++i;
            while (i < f.size() and f[i]!='e'){
                std::string key_for_info = Read_the_word(f, i);
                if (key_for_info == "name") {
                    torrent.name = Read_the_word(f, i);
                }
                else if (key_for_info == "piece length" or key_for_info=="length") {
                    if (f[i]!='i'){
                        throw std::runtime_error("Для численого значения должно быть i");
                    }
                    ++i;
                    size_t l = i;
                    while (i < f.size() and f[i]!='e'){
                        ++i;
                    }
                    std::string n = f.substr(l, i-l);
                    size_t nn = std::stoull(n);
                    if (key_for_info=="piece length"){
                        torrent.pieceLength = nn;
                    }
                    else{
                        torrent.length = nn;
                    }
                    
                    ++i;
                }
                else if (key_for_info == "pieces") {
                    std::string pieces = Read_the_word(f, i);
                    for (size_t j = 0; j < pieces.size(); j+=20) {
                        torrent.pieceHashes.push_back(pieces.substr(j, 20));
                    }
                }
                else{
                    Skip(f, i);
                }
            }
            ++i;
            size_t end = i;
            std::string info = f.substr(start, end-start);
            unsigned char hash[20];
            SHA1(reinterpret_cast<const unsigned char*>(info.c_str()), info.size(), hash);
            torrent.infoHash = std::string(reinterpret_cast<char*>(hash), 20);

        }
        else{
            Skip(f, i);

        }
    }
    return torrent;

}