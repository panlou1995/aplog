/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-11 09:14:41
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-29 15:37:23
 * @FilePath: /aplog/include/transporttcp.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include "pollmanager.h"
#include "threadpool.h"
#include <atomic>
namespace aplog
{
class APLOG_DECLSPEC_PUBLIC TransportTcp: public std::enable_shared_from_this<TransportTcp>
{
public:
    TransportTcp(bool isServer);
    ~TransportTcp();

    bool connect(const string& host, int port);
    bool listen(int port, int backlog, string host = "");
    TcpTransportPtr accept();
    void setSocket(int socket);

    void setReadCb(tcpCallBack cb) {read_cb_ = cb;}
    void setWritCb(tcpCallBack cb) {write_cb_ = cb;}
    void setAcceptCb(tcpCallBack cb) {accept_cb_ = cb;}
    void setConnSuccessCb(clientConnSuccessCallBack cb) {conn_cb_ = cb;}

    int32_t read(uint8_t* buffer, uint32_t size);
    int32_t write(uint8_t* buffer, uint32_t size);
    
    void enableRead();
    void enableWrite();

    bool isClientConnect() const {return is_connected_;}
    uint32_t getDisconnectSec() const  {return disConnectTimeSec_;}
    int getSocket() const {return sock_;}
    
private:
    int sock_;
    bool is_server_;
    std::atomic<bool> is_connected_;
    uint32_t disConnectTimeSec_;

    PollManager* poll_;
    ThreadPool* threadpool_;
    
    tcpCallBack  read_cb_;
    tcpCallBack  write_cb_;
    tcpCallBack  accept_cb_;
    clientConnSuccessCallBack conn_cb_;
    boost::recursive_mutex recur_lock_;

    void socketUpdate(int events);
    void initsocket();
    void setSocketNonBlock();
    void setSocketReuse(bool);
    void setSocketKeepAlive(bool use);
    bool needOperLater();
};
}