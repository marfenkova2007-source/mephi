#include "torrent_tracker.h"
#include "bencode.h"
#include "byte_tools.h"
#include <cpr/cpr.h>

/*
std::string Read_the_word(const std::string& data, size_t& pos) {
    return Bencode::ReadWord(data, pos);
}

void Skip(const std::string& f, size_t& i) {
    Bencode::SkipValue(f, i);
}*/

const std::vector<Peer> &TorrentTracker::GetPeers() const {
    return peers_;
}

