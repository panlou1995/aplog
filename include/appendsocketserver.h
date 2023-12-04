/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-22 16:05:45
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-25 11:40:51
 * @FilePath: /aplog/include/socketclientimpl.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma  once

#include "config.hxx"
#include "appendsocket.h"
#include "connection.h"
#include "transporttcp.h"
#include "boost/thread.hpp"
#include <queue>
#include <condition_variable>
#include <mutex>


namespace aplog {
class SocketAppenderServer:public SocketAppender
{
public:
    SocketAppenderServer(const string& filename, int filenum, size_t size);
    ~SocketAppenderServer();

    void execLogWrite(const stringPtr& msg);

private:
    bool isRun_;
    string serverip_;
    int serverport_;
    bool serverHasAppender_;

    int reconinterval_;
    string logfile_;
    int filenum_;
    string name_;
    size_t maxsize_;

    ConnectionPtr conn_;
    TcpTransportPtr transport_;
    
    void init();
    void rollover();
};

}