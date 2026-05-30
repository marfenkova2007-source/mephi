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
    bool choked_;
    PiecePtr pieceInProgress_;  // часть файла, с которой сейчас работаем
    PieceStorage& pieceStorage_;  // отсюда берем новые части для скачивания
    bool pendingBlock_;  // уже послали запрос на скачивание части файла и ждем ответ
      /*/

PeerConnect::PeerConnect(const Peer& peer, const TorrentFile &tf, std::string selfPeerId, PieceStorage& pieceStorage):
    tf_(tf), socket_(peer.ip, peer.port, std::chrono::milliseconds(2000), std::chrono::milliseconds(2000)), selfPeerId_(selfPeerId), peerId_(""),
    piecesAvailability_(), terminated_(false), choked_(true), pieceInProgress_(nullptr), pieceStorage_(pieceStorage), pendingBlock_(false) {};

void PeerConnect::Run() {
    int p = 0;
    const int max_p = 3;
    while (!terminated_ and p<max_p) {
        if (EstablishConnection()) {
            std::cout << "Connection established to peer" << std::endl;
            MainLoop();
            return;
        } else {
            std::cerr << "Cannot establish connection to peer" << std::endl;
            ++p;
           // Terminate();
        }
    }
    std::cout << "Gave up on this peer" << std::endl;
}

void PeerConnect::PerformHandshake() {
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
        socket_.EstablishConnection();
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

void PeerConnect::RequestPiece() {
    if (pieceInProgress_==nullptr){
        return;
    }
    Block* new_block = pieceInProgress_->FirstMissingBlock();
    if (new_block != nullptr){
        std::string res = IntToBytes(new_block->piece) + 
                          IntToBytes(new_block->offset) + 
                          IntToBytes(new_block->length);
        Message SMS = Message::Init(MessageId::Request, res);
        socket_.SendData(SMS.ToString());
        pendingBlock_ = true;
    }
}

void PeerConnect::Terminate() {
    std::cerr << "Terminate" << std::endl;
    terminated_ = true;
}

void PeerConnect::MainLoop() {
    while (!terminated_) {
        std::string rasm = socket_.ReceiveData();
        
        if (rasm == "ЖИВ") {
            continue; 
        }

        if (rasm.empty()) {
            if (pieceInProgress_ != nullptr) {
                pieceInProgress_->Reset();
                pieceInProgress_ = nullptr;
            }
            Terminate();
            break;
        }

        Message SMS = Message::Parse(rasm);

        if (SMS.id == MessageId::Choke) {
            choked_ = true;
        }
        else if (SMS.id == MessageId::Unchoke) {
            choked_ = false;
        }
        else if (SMS.id == MessageId::Piece and SMS.payload.size() >= 8) {
            int idx = BytesToInt(SMS.payload.substr(0, 4));
            int smesh = BytesToInt(SMS.payload.substr(4, 4));
            std::string binar_data = SMS.payload.substr(8);

            if (pieceInProgress_ != nullptr and pieceInProgress_->GetIndex() == idx) {
                pieceInProgress_->SaveBlock(smesh, binar_data);
                pendingBlock_ = false;

                if (pieceInProgress_->AllBlocksRetrieved()) {
                    pieceStorage_.PieceProcessed(pieceInProgress_);
                    pieceInProgress_ = nullptr;
                }
            }
        }

        if (pieceInProgress_ == nullptr) {
            pieceInProgress_ = pieceStorage_.GetNextPieceToDownload();
        }

        if (!choked_ && !pendingBlock_ && pieceInProgress_ != nullptr) {
            RequestPiece();
            pendingBlock_ = true;
        }
    }
    if (pieceInProgress_ != nullptr) {
        pieceInProgress_->Reset();
        pieceInProgress_ = nullptr;
    }
    terminated_ = true;
}

bool PeerConnect::Failed() const {
    return terminated_ and (pieceInProgress_ != nullptr);
}
