/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-22 16:50:45
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-06 13:44:11
 * @FilePath: /aplog/src/socketclientimpl.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "socketclientimpl.h"
#include "connectionmanager.h"
#include "appendsocket.h"
#include "nlohmann/json.hpp"
#include "layout.h"
#include "utils.h"

namespace aplog
{
SocketAppenderCliImpl::SocketAppenderCliImpl():
    isRun_(true),
    serverHasAppender_(false),
    queuesize_(50),
    reconinterval_(1),
    threadpool_(ThreadPool::Instance())
{

}

SocketAppenderCliImpl::~SocketAppenderCliImpl()
{

}

void SocketAppenderCliImpl::initialize(const string& serverip, int port, SocketAppender* ptr)
{
    serverip_ = serverip;
    serverport_ = port;
    sockappendPtr_  = ptr;

    newClient();
    writeQueueThread_ = boost::thread(&SocketAppenderCliImpl::dealWriteQueue, this);
}

void SocketAppenderCliImpl::setFile(const string& filename, int filenum, size_t size)
{
    logfile_= filename;
    filenum_ = filenum;
    maxsize_ = size;

    writeAppender();
}

bool SocketAppenderCliImpl::newClient()
{
    if(conn_)
        ConnectionManager::dropConnection(conn_);

    transport_ = std::make_shared<TransportTcp>(false);
    assert(transport_);
    transport_->setConnSuccessCb(boost::bind(&SocketAppenderCliImpl::onConnect, this));

    if (!transport_->connect(serverip_, serverport_)) {
        return false;
    }
    
    conn_ = std::make_shared<Connection>(transport_, false);
    
    if(serverHasAppender_) {
        conn_->setWrittenCb(boost::bind(&SocketAppenderCliImpl::onLogWritten, this));
    }

    transport_->setWritCb(boost::bind(&Connection::onWritable, conn_, _1));
    ConnectionManager::addConnect(conn_);
    
    return true;
}

void SocketAppenderCliImpl::append(logeventPtr& evt)
{
    if(!serverHasAppender_) {
        pushFailMsg(evt);
        return;
    }

    if(!transport_->isClientConnect()) {
        pushFailMsg(evt);
        if(util::getCurrentSec() - transport_->getDisconnectSec() <= reconinterval_) {            
            reconinterval_  > 1800 ? (reconinterval_ = 1800) : reconinterval_ += 2;
        } else {
            newClient();
        }

        return;
    }
    
    pushWriteQueue(evt);
}

void SocketAppenderCliImpl::socketWrite(const logeventPtr& log)
{
    assert(sockappendPtr_);
    std::cout << "socket write: " << log->getMsg() << std::endl;
    auto logbuffer = sockappendPtr_->getLayOut()->getLogBuffer(log);
    nlohmann::json data = nlohmann::json::object();

    data["name"] = sockappendPtr_->getName();
    data["msg"] = logbuffer;

    std::string datastr = data.dump();
    
    size_t sendlen = datastr.length() + 3;
    boost::shared_array<uint8_t> sendbuffer(new uint8_t[sendlen]);
    std::memset(sendbuffer.get(), 0, sendlen);

    sendbuffer[0] = NORMAL_WRITE;
    sendbuffer[1] = datastr.length() + 1;
    memcpy(sendbuffer.get()+2, datastr.data(), datastr.length());
    
    if(transport_->isClientConnect())
        conn_->write(sendbuffer, sendlen, boost::bind(&SocketAppenderCliImpl::onLogWritten, this));
    else 
        pushFailMsg(log);
}

void SocketAppenderCliImpl::onLogWritten()
{
    /*写成功后，原来的队尾删除，更新当前处理的事件*/
    std::unique_lock<std::mutex> lock(mutex_);
    currDealLog_ = writequque_.front();
    writequque_.pop();
    condition_.notify_one();
}

void SocketAppenderCliImpl::onConnect()
{
    reconinterval_ = 1;
    std::cout << "connect  callback!" << std::endl;
    writeFailMsg();
}

void SocketAppenderCliImpl::dealWriteQueue()
{
    logeventPtr logevent;
    while (isRun_) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [&]{return !writequque_.empty();});
            logevent = writequque_.front();
        }

        if(currDealLog_ != logevent) {
            //auto func = boost::bind(&SocketAppenderCliImpl::socketWrite, this, logevent);
            //threadpool_->pushJob(func);
            socketWrite(logevent);
        }
    }
}

void SocketAppenderCliImpl::writeFailMsg()
{
    if(failqueue_.empty())
        return;
    
    while(!failqueue_.empty()) {
        pushWriteQueue(failqueue_.back());
        failqueue_.pop();
    }
}

void SocketAppenderCliImpl::pushWriteQueue(const logeventPtr& log)
{
    std::unique_lock<std::mutex> lock(mutex_);
    writequque_.emplace(log);
    condition_.notify_one();
}

void SocketAppenderCliImpl::pushFailMsg(const logeventPtr& log)
{
    if(failqueue_.size() >= queuesize_)  {
        failqueue_.pop();
    }

    failqueue_.emplace(log);
}

void SocketAppenderCliImpl::writeAppender()
{
    if (!transport_ || !conn_ || !sockappendPtr_)
        return;

    nlohmann::json data =  nlohmann::json::object();
    data["name"] = sockappendPtr_->getName();
    data["filename"] = logfile_;
    data["size"] = maxsize_;
    data["filenum"] = filenum_;

    std::string datastr(data.dump());
    int len = datastr.length()+3;
    boost::shared_array<uint8_t> writebuf = boost::shared_array<uint8_t>(new uint8_t[len]);
    std::memset(writebuf.get(), 0, len);
    writebuf[0] = static_cast<uint8_t>(CREATE_APPENDER);
    writebuf[1] = static_cast<uint8_t>(datastr.length() + 1);
    memcpy(writebuf.get()+2, datastr.data(), datastr.length());

    conn_->write(writebuf, len, boost::bind(&SocketAppenderCliImpl::createAppenderDone, this));
}

void SocketAppenderCliImpl::createAppenderDone()
{
    serverHasAppender_ = true;
}

}