#include "piece_storage.h"
#include <iostream>

PieceStorage::PieceStorage(const TorrentFile& tf) {
        kol = tf.pieceHashes.size();
        for (size_t i=0; i<kol; ++i){
            size_t dlin = 0;
            if (i==kol-1){
                dlin = tf.length - i*tf.pieceLength;
            }
            else{
                dlin = tf.pieceLength;
            }
            PiecePtr new_kysok = std::make_shared<Piece>(i, dlin, tf.pieceHashes[i]);
            remainPieces_.push(new_kysok);
        }
    }

PiecePtr PieceStorage::GetNextPieceToDownload() {
    if (remainPieces_.empty()){
        return nullptr;
    }
    return remainPieces_.front();
}

void PieceStorage::PieceProcessed(const PiecePtr& piece) {
    SavePieceToDisk(piece);
}

bool PieceStorage::QueueIsEmpty() const {
    if (remainPieces_.empty()){
        return true;
    }
    return false;
}

size_t PieceStorage::TotalPiecesCount() const {
    return kol;
}

void PieceStorage::SavePieceToDisk(PiecePtr piece) {
    // Эта функция будет переопределена при запуске вашего решения в проверяющей системе
    // Вместо сохранения на диск там пока что будет проверка, что часть файла скачалась правильно
    std::cout << "Downloaded piece " << piece->GetIndex() << std::endl;
    std::cerr << "Clear pieces list, don't want to download all of them" << std::endl;
    while (!remainPieces_.empty()) {
        remainPieces_.pop();
    }
}
