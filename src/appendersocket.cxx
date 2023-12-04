/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-16 14:58:47
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 18:28:02
 * @FilePath: /aplog/src/appendersocket.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "appendsocket.h"
#include "connection.h"
#include "connectionmanager.h"
#include "nlohmann/json.hpp"
#include "utils.h"

namespace aplog
{
SocketAppender::SocketAppender(const string& server, int port, bool needAsync):
    Appender(needAsync),
    serverip_(server),
    serverport_(port)
{
    std::cout << "initialize client!" << std::endl;
    init();
}

SocketAppender::~SocketAppender()
{

}

void SocketAppender::setFile(const string& filename, int filenum, size_t size)
{
    cliImpl_.setFile(filename, filenum, size);
}

void SocketAppender::init()
{
    cliImpl_.initialize(serverip_, serverport_, this);
}

void SocketAppender::doAppend(logeventPtr& evt)
{
    cliImpl_.append(evt);
}

}