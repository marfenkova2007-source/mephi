#include "message.h"
#include "byte_tools.h"

Message Message::Parse(const std::string& messageString){
    if (messageString.empty()){
        Message SMS;
        SMS.id = MessageId::KeepAlive;
        SMS.payload = "";
        SMS.messageLength = 0;
        return SMS;
    }
    Message SMS;
    SMS.id = static_cast<MessageId>(static_cast<unsigned char>(messageString[0]));
    SMS.messageLength = messageString.size();
    if (messageString.size()==1) {
        SMS.payload = "";
    }
    else{
        SMS.payload = messageString.substr(1);
    }
    return SMS;
};

Message Message::Init(MessageId id, const std::string& payload){
    Message SMS;
    SMS.id = id;
    SMS.payload = payload;
    SMS.messageLength = payload.size() +1;
    return SMS;
};

std::string Message::ToString() const{
    std::string res = IntToBytes(static_cast<uint32_t>(messageLength));

    if (id != MessageId::KeepAlive) {
        res.push_back(static_cast<char>(id));
        res += payload;
    }

    return res;

};