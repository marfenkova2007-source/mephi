#include "byte_tools.h"
#include "peer_connect.h"
#include "message.h"
#include <iostream>
#include <sstream>
#include <utility>
#include <cassert>

using namespace std::chrono_literals;

/*const TorrentFile& tf_;
    TcpConnect socket_;  // tcp-соединение с пиром
    const std::string selfPeerId_;  // наш id, которым представляется наш клиент
    std::string peerId_;  // id пира, с которым мы общаемся в текущем соединении
    PeerPiecesAvailability piecesAvailability_;
    bool terminated_;  // флаг, необходимый для завершения цикла общения с пиром
    bool choked_;  /*/

PeerConnect::PeerConnect(const Peer& peer, const TorrentFile &tf, std::string selfPeerId):
    tf_(tf), socket_(peer.ip, peer.port, std::chrono::milliseconds(2000), std::chrono::milliseconds(2000)), selfPeerId_(selfPeerId), peerId_(""), piecesAvailability_(), terminated_(false), choked_(true)  {}

void PeerConnect::Run() {
    while (!terminated_) {
        if (EstablishConnection()) {
            std::cout << "Connection established to peer" << std::endl;
            MainLoop();
        } else {
            std::cerr << "Cannot establish connection to peer" << std::endl;
            Terminate();
        }
    }
}

void PeerConnect::PerformHandshake() {
    socket_.EstablishConnection();
    std::string handshake_str;
    
    handshake_str.push_back((char)19);
    handshake_str += "BitTorrent protocol";

    for (int i = 0; i < 8; ++i) {
        handshake_str.push_back('\0');
    }
    handshake_str += tf_.infoHash;
    handshake_str += selfPeerId_;

    socket_.SendData(handshake_str);
    std::string otvet = socket_.ReceiveData(68);

    if (otvet.size() < 68) {
        throw std::runtime_error("Короткий ответ пира");
    }

    std::string peer_hash = otvet.substr(28, 20);
    if (peer_hash != tf_.infoHash) {
        throw std::runtime_error("Не совпадает");
    }

    peerId_ = otvet.substr(48, 20);

}

bool PeerConnect::EstablishConnection() {
    try {
        PerformHandshake();
        ReceiveBitfield();
        SendInterested();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to establish connection with peer " << socket_.GetIp() << ":" <<
            socket_.GetPort() << " -- " << e.what() << std::endl;
        return false;
    }
}

void PeerConnect::ReceiveBitfield() {
    std::string bait_of_dlin4= socket_.ReceiveData();
    if (bait_of_dlin4.empty()){
        return;
    }
    Message SMS = Message::Parse(bait_of_dlin4);
    if (SMS.id == MessageId::BitField) {
        piecesAvailability_ = PeerPiecesAvailability(SMS.payload);
    } 
    else if (SMS.id == MessageId::Unchoke) {
        choked_ = false;
    }

}

void PeerConnect::SendInterested() {
    Message SMS = Message::Init(MessageId::Interested, "");
    socket_.SendData(SMS.ToString());
}

void PeerConnect::Terminate() {
    std::cerr << "Terminate" << std::endl;
    terminated_ = true;
}

void PeerConnect::MainLoop() {
    /*
     * При проверке вашего решения на сервере этот метод будет переопределен.
     * Если вы провели handshake верно, то в этой функции будет работать обмен данными с пиром
     */
    std::cout << "Dummy main loop" << std::endl;
    Terminate();
}
