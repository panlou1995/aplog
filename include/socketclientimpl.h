/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-22 16:05:45
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-06 10:24:15
 * @FilePath: /aplog/include/socketclientimpl.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma  once

#include "config.hxx"
#include "connection.h"
#include "threadpool.h"
#include "transporttcp.h"
#include "boost/thread.hpp"
#include <queue>
#include <condition_variable>
#include <mutex>


namespace aplog {
class SocketAppender;

class SocketAppenderCliImpl
{
public:
    SocketAppenderCliImpl();
    ~SocketAppenderCliImpl();

    void setFile(const string& filename, int filenum, size_t size);
    void pushWriteQueue(const logeventPtr&);
    void pushFailMsg(const logeventPtr&);
    void initialize(const string& serverip, int port, SocketAppender* ptr);
    bool newClient();
    void onConnect();
    void onLogWritten();
    void createAppenderDone();
    void append(logeventPtr& evt);

private:
    bool isRun_;
    string serverip_;
    int serverport_;
    bool serverHasAppender_;
    int queuesize_;
    int reconinterval_;
    string logfile_;
    int filenum_;
    //string name_;
    size_t maxsize_;
    boost::thread writeQueueThread_;
    std::condition_variable condition_;
    std::mutex mutex_;
    ConnectionPtr conn_;
    TcpTransportPtr transport_;
    std::queue<logeventPtr> failqueue_;
    std::queue<logeventPtr> writequque_;
    logeventPtr currDealLog_;
    SocketAppender* sockappendPtr_;
    ThreadPool* threadpool_;

    void writeAppender();
    void dealWriteQueue();
    void writeFailMsg();
    void socketWrite(const logeventPtr&);
};

}