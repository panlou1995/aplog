/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-08 13:27:33
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-23 15:35:06
 * @FilePath: /aplog/include/pollmanager.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include "spinlock.h"
#include <map>
#include <sys/epoll.h>
#include <boost/thread.hpp>

namespace aplog
{
class APLOG_DECLSPEC_PUBLIC PollManager
{
public:
    static PollManager* Instance();
    void startclient();
    void startserver();
    void shutdown();

    bool addSocket(int sock, const socketUpdateFunc& updateCb);
    bool delSocket(int sock);
    bool addEvents(int sock, int events);
    bool delEvents(int sock, int events);
    
private:
    typedef struct sockInfo{
        socketUpdateFunc cbfunc_;
        int fd_;
        int event_;
        sockInfo():cbfunc_(NULL), fd_(-1), event_(-1) {}
    }sockInfo;

    static PollManager* instance_;
    int epollfd_;
    SpinLock spinlock_;
    const static int MAX_EVENT_NUMBER = 10000;
    epoll_event events_[MAX_EVENT_NUMBER];
    std::map<int, sockInfo> sockmap_;
    boost::thread loopthread_;
    std::atomic<bool> isShutDown_;

    PollManager();
    ~PollManager();
    void loop();

    void update(int timeout);
};

}