#pragma once

#include <string>
#include <chrono>
#include <cstring>
#include <cerrno>    
#include <stdexcept>
#include "byte_tools.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>


/*
 * Обертка над низкоуровневой структурой сокета.
 */
class TcpConnect {
public:
    TcpConnect(std::string ip, int port, std::chrono::milliseconds connectTimeout, std::chrono::milliseconds readTimeout)
    : ip_(std::move(ip)), port_(port), connectTimeout_(connectTimeout), readTimeout_(readTimeout), sock_(-1) {};
    ~TcpConnect(){
        if (sock_ != -1) {
            close(sock_);
            sock_ = -1;
        }
    };

    /*
     * Установить tcp соединение.
     * Если соединение занимает более `connectTimeout` времени, то прервать подключение и выбросить исключение.
     * Полезная информация:
     * - https://man7.org/linux/man-pages/man7/socket.7.html
     * - https://man7.org/linux/man-pages/man2/connect.2.html
     * - https://man7.org/linux/man-pages/man2/fcntl.2.html (чтобы включить неблокирующий режим работы операций)
     * - https://man7.org/linux/man-pages/man2/select.2.html
     * - https://man7.org/linux/man-pages/man2/setsockopt.2.html
     * - https://man7.org/linux/man-pages/man2/close.2.html
     * - https://man7.org/linux/man-pages/man3/errno.3.html
     * - https://man7.org/linux/man-pages/man3/strerror.3.html
     */
    void EstablishConnection(){
        sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_ == -1) return;

        int flags = fcntl(sock_, F_GETFL, 0);
        fcntl(sock_, F_SETFL, flags | O_NONBLOCK);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr);

        connect(sock_, (struct sockaddr*)&addr, sizeof(addr));

        struct pollfd pfd{sock_, POLLOUT, 0};
        int poll_ret = poll(&pfd, 1, connectTimeout_.count());

        if (poll_ret <= 0) { 
            CloseConnection();
            throw std::runtime_error("Таймаут подключения");
        }
        int error;
        socklen_t len = sizeof(error);
        getsockopt(sock_, SOL_SOCKET, SO_ERROR, &error, &len);
        
        if (error != 0) { 
            CloseConnection();
            throw std::runtime_error("Ошибка подключения");
        }

        fcntl(sock_, F_SETFL, flags);
    };

    /*
     * Послать данные в сокет
     * Полезная информация:
     * - https://man7.org/linux/man-pages/man2/send.2.html
     */
    void SendData(const std::string& data) const{
        if (sock_ == -1) {
            throw std::runtime_error("Не подключен");
        }

        const char* l = data.data();     
        size_t kol = 0;   
        const size_t nado_kol = data.size();

        while (kol<nado_kol) {
            size_t otpr = send(sock_, l+kol, nado_kol-kol, 0);
            

            if (otpr==-1 or otpr==0) {
                throw std::runtime_error("Ошибка");
            }
            kol+= otpr;
        }
    };

    /*
     * Прочитать данные из сокета.
     * Если передан `bufferSize`, то прочитать `bufferSize` байт.
     * Если параметр `bufferSize` не передан, то сначала прочитать 4 байта, а затем прочитать количество байт, равное
     * прочитанному значению.
     * Первые 4 байта (в которых хранится длина сообщения) интерпретируются как целое число в формате big endian,
     * см https://wiki.theory.org/BitTorrentSpecification#Data_Types
     * Полезная информация:
     * - https://man7.org/linux/man-pages/man2/poll.2.html
     * - https://man7.org/linux/man-pages/man2/recv.2.html
     */
    std::string ReceiveData(size_t bufferSize = 0) const{
        if (sock_ == -1) {
            throw std::runtime_error("Сокет закрыт");
        }
        size_t nado_kol = bufferSize;

        if (bufferSize == 0) {
            std::string dlinna_str = ReceiveData(4);
            nado_kol = BytesToInt(dlinna_str);     
            if (nado_kol == 0) return "ЖИВ";
        }
        std::string rez(nado_kol, '\0'); // Сразу готовим строку нужного размера
        size_t has = 0;

        while (has < nado_kol) {
            struct pollfd pfd{sock_, POLLIN, 0};
            int p = poll(&pfd, 1, readTimeout_.count());

            if (p == 0) throw std::runtime_error("Таймаут чтения");
            if (p == -1) throw std::runtime_error("Ошибка poll");

            ssize_t poluch = recv(sock_, &rez[has], nado_kol-has, 0);

            if (poluch == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                throw std::runtime_error("Ошибка recv");
            }
            if (poluch == 0) {
                throw std::runtime_error("Пир отключился");
            }

            has += poluch;
        }
        return rez;
    };

    /*
     * Закрыть сокет
     */
    void CloseConnection(){
        if (sock_ != -1) {
            close(sock_);
            sock_ = -1;
        }
    };

    const std::string& GetIp() const;
    int GetPort() const;
private:
    const std::string ip_;
    const int port_;
    std::chrono::milliseconds connectTimeout_, readTimeout_;
    int sock_;
};
