#include "torrent_file.h"
#include "bencode.h"

#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype> 



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