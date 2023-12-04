/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-11 09:37:08
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-05 14:05:31
 * @FilePath: /aplog/src/transporttcp.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "transporttcp.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <boost/function.hpp>
#include <iostream>
#include "connection.h"
#include "connectionmanager.h"
#include "utils.h"

namespace aplog
{
TransportTcp::TransportTcp(bool isServer):
    sock_(-1),
    is_server_(isServer),
    is_connected_(false),
    disConnectTimeSec_(util::getCurrentSec()),
    poll_(PollManager::Instance()),
    threadpool_(ThreadPool::Instance())
{

}

TransportTcp::~TransportTcp()
{
    std::cout << "transporttcp destruct!" << std::endl;
    poll_->delSocket(sock_);
    ::close(sock_);
}

void TransportTcp::enableRead()
{
    poll_->addEvents(sock_, EPOLLIN);
}

void TransportTcp::enableWrite()
{
    poll_->addEvents(sock_, EPOLLOUT);
}

bool TransportTcp::connect(const string& host, int port)
{
    sockaddr_in dest;
    bzero(&dest, sizeof(sockaddr_in));
    
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_ < 0) {
        std::cerr << "new socket fail: " << errno << std::endl;
        return false;
    }

    if (inet_pton(AF_INET, host.c_str(), &dest.sin_addr.s_addr) != 1) {
        std::cerr << "inet pton fail: " << errno << std::endl;
        return false;
    }

    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);

    initsocket();

    int ret = ::connect(sock_, (struct sockaddr *)&dest, sizeof(dest));
    if (ret == 0)
        is_connected_ = true;
    
    if (!needOperLater()) {
        std::cerr << "no need operlater: " << errno << std::endl;
        return false;
    }

    poll_->addSocket(sock_, boost::bind(&TransportTcp::socketUpdate, this, _1));
    poll_->addEvents(sock_, EPOLLIN | EPOLLOUT);
    
    std::cout << "new client: "  << sock_ << std::endl;
    return true;
}

bool TransportTcp::listen(int port, int backlog, string host)
{
    sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(sockaddr_in));

    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    setSocketReuse(true);

    if(sock_ < 0) {
        std::cerr << "new socket fail: " << errno << std::endl;
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(host.empty()) {
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        serv_addr.sin_addr.s_addr = inet_addr(host.c_str());
    }

    if(::bind(sock_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        std::cerr << "bind fail: " << errno << std::endl;
        return false;
    }

    if(::listen(sock_, backlog) == -1) {
        std::cerr << "listen fail: " << errno << std::endl;
        return false;
    }
    
    setSocketNonBlock();
    poll_->addSocket(sock_, boost::bind(&TransportTcp::socketUpdate, this, _1));
    poll_->addEvents(sock_, EPOLLIN);

    return true;
}

TcpTransportPtr TransportTcp::accept()
{
    sockaddr_in cli_addr;
    socklen_t len = sizeof(sockaddr_in);
    bzero(&cli_addr, sizeof(cli_addr));

    int clisocket;

    if ((clisocket = ::accept(sock_, (struct sockaddr *)&cli_addr, &len)) < 0) {
        std::cerr << "accept fail: " << errno << std::endl;
        return TcpTransportPtr();
    }

    std::cout << "client connect, fd:" << clisocket << std::endl;
    
    auto tcp = std::make_shared<TransportTcp>(false);
    tcp->setSocket(clisocket);

    poll_->addSocket(clisocket, boost::bind(&TransportTcp::socketUpdate, tcp, _1));
    poll_->addEvents(clisocket, EPOLLIN | EPOLLRDHUP);

    if(accept_cb_) 
        accept_cb_(tcp);

    return tcp;
}

void TransportTcp::socketUpdate(int events)
{
    boost::recursive_mutex::scoped_lock lc(recur_lock_);
    std::cout << "sock update: " << sock_ << " events: "<< events << std::endl;
    if (is_server_ && (events & EPOLLIN)) {
        /*this is connect request from client*/
        std::cout << "1\n";
        auto func = boost::bind(&TransportTcp::accept, this);
        threadpool_->pushJob(func);
        return;
    } else if ((events & EPOLLOUT) && !is_connected_) {
        std::cout << "2\n";
        int error;
        socklen_t len = sizeof(error);

        if (getsockopt(sock_, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            std::cerr << "getsockopt failed" << std::endl;
            exit(1);
        }
        if (error == 0) {
            std::cout << "connect to server!" << std::endl;
            is_connected_ = true;
            if(conn_cb_)
                conn_cb_();
        }
    } else if (events & (EPOLLHUP | EPOLLRDHUP)) {
        std::cout << "5\n";
        /*TODO:disconnect需要处理*/
        is_connected_ = false;
        disConnectTimeSec_ = util::getCurrentSec();
        poll_->delSocket(sock_);
    } else if (events & EPOLLIN) {
        std::cout << "3\n";
        /*
        char buf[1024] = {0};
        
        int ret = recv(sock_, buf, 1024, 0);
        std::cout << "*******" << std::endl;
        std::cout << "read:" << buf << std::endl;
        std::cout << "*******" << std::endl;
        std::cout << "len:" << ret <<"  errno:" <<errno<< std::endl;
        std::cout << "*******" << std::endl;
        */
        if(!read_cb_.empty()) {
            std::cout << "call back  read!" << std::endl;
            auto func = boost::bind(read_cb_, _1);
            threadpool_->pushJob(func, shared_from_this());
        }else {
            std::cout << "read call back empty!" << std::endl;
        }
    } else if (events & EPOLLOUT) {
        std::cout << "4\n";
        if(!write_cb_.empty()) {
            std::cout << "call back  write!" << std::endl;
            auto func = boost::bind(write_cb_, shared_from_this());
            threadpool_->pushJob(func);
        } else {
            std::cout << "write call back empty!" << std::endl;
        }
    }
}

/*读写的调度由Connection控制，buffer也在Connection维护*/
int32_t TransportTcp::read(uint8_t* buffer, uint32_t size)
{
    assert(size > 0);
    int read_num = recv(sock_, reinterpret_cast<char*>(buffer), size, 0);
    
    if(read_num < 0) {
        if (needOperLater()) {
            return 0;
        } else {
            /*TODO:发生错误，关闭套接字*/
            return -1;
        }
    }else if (read_num == 0) {
        /*TODO:连接断开，关闭套接字*/
        return -1;
    }

    return read_num;
}

int32_t TransportTcp::write(uint8_t* buffer, uint32_t size)
{
    assert(size > 0);
    int write_num = send(sock_, reinterpret_cast<char*>(buffer), size, 0);
    
    if(write_num < 0) {
        if (needOperLater()) {
            return 0;
        } else {
            /*TODO:发生错误，关闭套接字*/
            return -1;
        }
    }

    return write_num;
}

void TransportTcp::setSocket(int socket)
{
    sock_ = socket;
    initsocket();
}

void TransportTcp::initsocket()
{
    setSocketNonBlock();
    setSocketKeepAlive(true);
}

void TransportTcp::setSocketNonBlock()
{
    int flags = fcntl(sock_, F_GETFL, 0);
    if (flags < 0) {
        std::cerr << "fcntl F_GETFL failed" << std::endl;
        exit(1);
    }

    flags |= O_NONBLOCK;
    if (fcntl(sock_, F_SETFL, flags) < 0) {
        std::cerr << "fcntl F_SETFL failed" << std::endl;
        exit(1);
    }
}

void TransportTcp::setSocketReuse(bool opt)
{
    int optval = opt;
    setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void TransportTcp::setSocketKeepAlive(bool use)
{
    if (use)
    {
    int val = 1;
    setsockopt(sock_, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char*>(&val), sizeof(val));

    }
    else
    {
    int val = 0;
    setsockopt(sock_, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char*>(&val), sizeof(val));
    }
}

bool TransportTcp::needOperLater()
{
    if ( ( errno == EAGAIN  ) || ( errno == EWOULDBLOCK ) || (errno == EINPROGRESS)) {
		return true;
	} else {
		return false;
	}
}

}