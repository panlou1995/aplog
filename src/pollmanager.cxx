/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-08 14:18:01
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 10:42:09
 * @FilePath: /aplog/src/pollmanager.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "pollmanager.h"
#include <iostream>

namespace aplog
{
PollManager* PollManager::instance_ = new PollManager();

PollManager* PollManager::Instance()
{
    assert(instance_ != nullptr);
    return instance_;
}

PollManager::PollManager():
    epollfd_(epoll_create(5)),
    isShutDown_(false)
{
    
}

PollManager::~PollManager()
{
    shutdown();
}

void PollManager::startclient()
{
    isShutDown_ = false;
    loopthread_ = boost::thread(boost::bind(&PollManager::loop, this));
}

void PollManager::startserver()
{
    while(!isShutDown_) {
        update(-1);
    }
}

void PollManager::shutdown()
{
    isShutDown_ = true;
    if(loopthread_.joinable())
        loopthread_.join();
    
    spinlock_.lock();
    auto it = sockmap_.begin();
    for (; it != sockmap_.end();  it++) {
        epoll_ctl(epollfd_, EPOLL_CTL_DEL, it->first, NULL);
    }
    sockmap_.clear();
    spinlock_.unlock();
}

void PollManager::loop()
{
    while (!isShutDown_) {
        spinlock_.lock();
        update(100);
        spinlock_.unlock();
    }
}

void PollManager::update(int timeout)
{
    socketUpdateFunc func;
    int cnt = epoll_wait(epollfd_, events_, MAX_EVENT_NUMBER, timeout);
    if (cnt == -1)
        return;
    for (int i = 0; i < cnt; i++) {
        auto it = sockmap_.find(events_[i].data.fd);
        std::cout <<  "epoll has fd:" << events_[i].data.fd  << " event:0x" << std::hex << events_[i].events << std::endl;
        if (it != sockmap_.end()) {
            func = it->second.cbfunc_;
            if (func) {
                func(events_[i].events);
            }
        }
    }
}

bool PollManager::addSocket(int sock, const socketUpdateFunc& updateCb)
{
    epoll_event event;
    event.data.fd = sock;
    event.events = 0;
    std::cout  << "epoll add: " << sock << std::endl;
    sockInfo sockinfo;
    sockinfo.cbfunc_ = updateCb;
    sockinfo.fd_ = sock;
    sockinfo.event_ = 0;
    
    spinlock_.lock();
    auto it = sockmap_.find(sock);
    if ( it != sockmap_.end()) {
        spinlock_.unlock();
        std::cerr << "socket exist:" << sock << std::endl;
        return true;
    }

    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, sock, &event) != 0) {
        spinlock_.unlock();
        return false;
    }
    
    sockmap_.insert(std::make_pair(sock, sockinfo));
    spinlock_.unlock();
    
    return true;
}

bool PollManager::delSocket(int sock)
{
    if (sock < 0)
        return false;

    spinlock_.lock();
    auto it = sockmap_.find(sock);
    if ( it == sockmap_.end()) {
        spinlock_.unlock();
        return true;
    }
    
    sockmap_.erase(it);
    spinlock_.unlock();
    epoll_ctl(epollfd_, EPOLL_CTL_DEL, sock, NULL);
    
    return true; 
}

bool PollManager::addEvents(int sock, int events)
{
    if (sock < 0)
        return false;

    epoll_event event;
    spinlock_.lock();
    auto it = sockmap_.find(sock);

    if (it == sockmap_.end()) {
        spinlock_.unlock();
        return true;
    }

    it->second.event_ |= events;
    event.data.fd = sock;
    event.events = it->second.event_;
    spinlock_.unlock();
    epoll_ctl(epollfd_, EPOLL_CTL_MOD, sock, &event);
    
    return true;    
}

bool PollManager::delEvents(int sock, int events)
{
    if (sock < 0)
        return false;

    epoll_event event;
    spinlock_.lock();
    auto it = sockmap_.find(sock);

    if (it == sockmap_.end()) {
        spinlock_.unlock();
        return true;
    }

    it->second.event_ &= ~events;
    event.data.fd = sock;
    event.events = it->second.event_;
    spinlock_.unlock();
    epoll_ctl(epollfd_, EPOLL_CTL_MOD, sock, &event);
    
    return true;    
}

}