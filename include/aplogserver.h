/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-23 15:33:05
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-01 17:07:04
 * @FilePath: /aplog/include/server.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include "pollmanager.h"

namespace aplog {
class AplogServer
{
public:
    AplogServer(int port, int backlog, string host = "");
    ~AplogServer();
    
    void listen();
    void eventloop();
    void acceptCb(const TcpTransportPtr& tcp);
    
private:
    int port_;
    int backlog_;
    string host_;
    PollManager* poll_;
    TcpTransportPtr transport_;
    ConnectionPtr conn_;

    void clientAppendCreate(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& readbuffer, uint32_t len);
    void clientSetAppend(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& readbuffer, uint32_t len);
    void clientExecLog(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& readbuffer, uint32_t len);
};
}