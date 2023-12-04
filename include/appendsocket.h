/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-16 17:57:44
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 18:16:43
 * @FilePath: /aplog/include/appendsocket.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include "appender.h"
#include "transporttcp.h"
#include "connection.h"
#include <fstream>
#include <queue>
#include <boost/thread.hpp>
#include <mutex>
#include <condition_variable>
#include "socketclientimpl.h"

namespace  aplog
{
class APLOG_DECLSPEC_PUBLIC SocketAppender:public Appender
{
public:
    SocketAppender(const string& server, int port, bool needAsync);
    virtual ~SocketAppender();

    void doAppend(logeventPtr& evt) override;
    void setFile(const string& filename, int filenum, size_t size);

protected:
    std::ofstream outfile_;
    SocketAppender() {};

private:
    void init();

    string serverip_;
    int serverport_;

    SocketAppenderCliImpl cliImpl_;
};

}