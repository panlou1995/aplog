/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-23 16:55:14
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 19:02:36
 * @FilePath: /aplog/src/server.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "aplogserver.h"
#include <memory>
#include "transporttcp.h"
#include "connection.h"
#include "connectionmanager.h"
#include "nlohmann/json.hpp"
#include "logger.h"
#include "appendsocketserver.h"
#include "threadpool.h"

namespace aplog{
AplogServer::AplogServer(int port, int backlog, string host):
    port_(port),
    backlog_(backlog),
    host_(host),
    poll_(PollManager::Instance()),
    transport_(std::make_shared<TransportTcp>(true))
{

}

AplogServer::~AplogServer()
{
    if(conn_)
        ConnectionManager::dropConnection(conn_);
}

void AplogServer::listen()
{
    transport_->listen(port_, backlog_, host_);
    conn_ = std::make_shared<Connection>(transport_, true);
    /*
    conn_->setLogAppendCreateHandler(boost::bind(&AplogServer::clientAppendCreate, this, _1, _2, _3));
    conn_->setLogWriteHandler(boost::bind(&AplogServer::clientExecLog, this, _1, _2, _3));
    */
    transport_->setAcceptCb(boost::bind(&AplogServer::acceptCb, this, _1));
    ConnectionManager::Instance()->addConnect(conn_);
}

void AplogServer::eventloop()
{
    poll_->startserver();
}

void AplogServer::acceptCb(const TcpTransportPtr& tcp)
{
    auto conn = std::make_shared<Connection>(tcp, false);
    tcp->setReadCb(boost::bind(&Connection::onReadable, conn, _1));

    conn->setLogAppendCreateHandler(boost::bind(&AplogServer::clientAppendCreate, this, conn, _2, _3));
    conn->setLogWriteHandler(boost::bind(&AplogServer::clientExecLog, this, conn, _2, _3));
    conn->read(2, boost::bind(&Connection::lengthRead, conn, conn, _2, _3));

    ConnectionManager::addConnect(conn);
}

void AplogServer::clientAppendCreate(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& readbuffer, uint32_t len)
{
    std::cout << "create append in server!" << std::endl;
    std::string str(reinterpret_cast<char *>(readbuffer.get()), len);

    //std::string strJson = str.substr(2, len);

    nlohmann::json create = nlohmann::json::parse(str);

    std::string name = create["name"];
    std::string filename = create["filename"];
    int size = create["size"];
    int num = create["filenum"];

    if(!Logger::hasLogger(name)) {
        aplog::Logger* log = aplog::Logger::getInstance(name);
        auto appender = std::make_shared<aplog::SocketAppenderServer>(filename, num, size);
        log->setAppender(appender);
        conn->read(2, boost::bind(&Connection::lengthRead, conn, _1, _2, _3));
    } else {
        std::cerr << "already has appender  " << name <<  " in server!" << std::endl;
    }
} 

void AplogServer::clientSetAppend(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& readbuffer, uint32_t len)
{
    /*设定日志等级和layout*/
    /*暂时不用，设置项放在客户端进行处理，服务端只接收最终需要写的数据*/
    return;
}

void AplogServer::clientExecLog(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& readbuffer, uint32_t len)
{
    std::string str(reinterpret_cast<char *>(readbuffer.get()), len);

    //std::string strJson = str.substr(2, len);

    nlohmann::json task = nlohmann::json::parse(str);

    /*发送日志数据*/
    std::string name = task["name"];
    std::string msg = task["msg"];

    stringPtr msgptr = std::make_shared<std::string>(msg);
    auto append = aplog::Logger::getInstance(name)->getAppender();
    if (append) {
        aplog::SocketAppenderServer* sockAppend = dynamic_cast<aplog::SocketAppenderServer*>(append.get());
        sockAppend->execLogWrite(msgptr);
        conn->read(2, boost::bind(&Connection::lengthRead, conn, _1, _2, _3));
    } else {
        std::cerr << "no appender  " << name <<  " create in server!" << std::endl;
    }
}

}