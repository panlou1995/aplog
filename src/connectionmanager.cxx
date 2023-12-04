/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-15 17:36:38
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-24 13:57:17
 * @FilePath: /aplog/src/connectionmanager.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "connectionmanager.h"
#include <memory>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>         
#include <sys/socket.h>

namespace aplog
{
ConnectionManager* ConnectionManager::instance_ = new ConnectionManager;
std::vector<ConnectionPtr> ConnectionManager::connvec_ = {};
std::mutex connvec_mutex;

ConnectionManager* ConnectionManager::Instance()
{
    assert(instance_ != nullptr);
    return instance_;
}

ConnectionManager::ConnectionManager():
    poll_(PollManager::Instance())
{
    timerStart(60);
}

ConnectionManager::~ConnectionManager()
{
    alarm(0);
}

void ConnectionManager::timerStart(int time)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = &ConnectionManager::timerCallBack;

    sigfillset(&sa.sa_mask);
    assert(sigaction(SIGALRM, &sa, NULL) != -1);

    alarm(time);
}

void ConnectionManager::timerCallBack(int num)
{
    std::lock_guard<std::mutex> lc(connvec_mutex);
    auto it = connvec_.begin();
    for(; it != connvec_.end(); it++) {
        auto conn = *it;
        if (conn->isServer()) {
            continue;
        } else {
            int sock = conn->transport()->getSocket();
            int error;
            socklen_t len = sizeof(error);
            getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len);
            if (error != 0)
                connvec_.erase(it);
        }
    }
}

void ConnectionManager::addConnect(const ConnectionPtr& conn)
{
    std::lock_guard<std::mutex> lc(connvec_mutex);
    connvec_.push_back(conn);
}

void ConnectionManager::dropConnection(const ConnectionPtr& conn)
{
    if(connvec_.empty())
        return;

    std::lock_guard<std::mutex> lc(connvec_mutex);
    auto it = connvec_.begin();
    for(; it != connvec_.end(); it++) {
        connvec_.erase(it);
        break;
    }
}

}