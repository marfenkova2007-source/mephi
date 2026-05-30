#include "piece_storage.h"
#include <fcntl.h>    
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <stdexcept>


PieceStorage::PieceStorage(const TorrentFile& tf, const std::filesystem::path& outputDirectory) : tf_(tf) {
        kol = tf.pieceHashes.size();
        std::lock_guard<std::mutex> lock(storage_mutex);
        piece_size_bytes = tf.pieceLength;
        output_file_path = outputDirectory / tf.name;
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
    std::lock_guard<std::mutex> lock(storage_mutex);
    if (remainPieces_.empty()){
        return nullptr;
    }
    PiecePtr next = remainPieces_.front();
    remainPieces_.pop(); 
    return next;
}

void PieceStorage::PieceProcessed(const PiecePtr& piece) {
    SavePieceToDisk(piece);
    std::lock_guard<std::mutex> lock(storage_mutex);
    ++kol_now;
    saved_indices_.push_back(piece->GetIndex());
}

bool PieceStorage::QueueIsEmpty() const {
    std::lock_guard<std::mutex> lock(storage_mutex);
    if (remainPieces_.empty()){
        return true;
    }
    return false;
}

size_t PieceStorage::TotalPiecesCount() const {
    return kol;
}



void PieceStorage::CloseOutputFile() {
}

const std::vector<size_t>& PieceStorage::GetPiecesSavedToDiscIndices() const {
    return saved_indices_;
}

size_t PieceStorage::PiecesInProgressCount() const {
    return 0;
}

void PieceStorage::SavePieceToDisk(PiecePtr piece) {
    int fd = ::open(output_file_path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        throw std::runtime_error("Не можем открыть");
    }

    if (ftruncate(fd, tf_.length) == -1) {
        ::close(fd);
        throw std::runtime_error("Не удалось установить размер файла");
    }

    off_t offset = static_cast<off_t>(piece->GetIndex()) * static_cast<off_t>(piece_size_bytes);
   
    if (::lseek(fd, offset, SEEK_SET) == -1) {
        ::close(fd);
        throw std::runtime_error("Невозможно сместиться в файле");
    }

    const std::string& data = piece->GetData();
    if (::write(fd, data.c_str(), data.size()) == -1) {
        ::close(fd);
        throw std::runtime_error("Ошибка записи данных");
    }

    ::fsync(fd);
    ::close(fd);
}

size_t PieceStorage::PiecesSavedToDiscCount() const {
    std::lock_guard<std::mutex> lock(storage_mutex);
    return kol_now;
}