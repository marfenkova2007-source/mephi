#pragma once

#include <string>
#include <vector>
#include "torrent_file.h"
#include "peer.h"
#include <cpr/cpr.h> 

std::string Read_the_word(const std::string& data, size_t& pos);
void Skip(const std::string& f, size_t& i);


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
    const std::vector<Peer>& GetPeers() const;

private:
    std::string url_;
    std::vector<Peer> peers_;
};
