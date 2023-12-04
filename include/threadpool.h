/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-28 10:55:59
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-28 15:42:22
 * @FilePath: /aplog/include/threadpool.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AEcon
 */
#pragma once

#include "config.hxx"
#include <boost/thread.hpp>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <array>
#include "loggingevent.h"
#include "appender.h"
#include "spinlock.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <queue>
namespace aplog
{
class APLOG_DECLSPEC_PUBLIC Asynlog {
public:
    Asynlog():pAppend(nullptr),logevt(nullptr) {};
    Asynlog(const Asynlog& other) {
        this->pAppend = std::move(other.pAppend);
        this->logevt = std::move(other.logevt);
    }

    Asynlog& operator=(void* ptr) {
        this->pAppend = AppedenSharedPtr((Appender*)ptr);
        this->logevt = logeventPtr((LogEvent*)ptr);
        return *this;
    }

    bool operator==(void* ptr) {
        return (this->logevt.get() == ptr && pAppend.get() == ptr);
    }

    bool operator!=(void* ptr) {
        return (this->logevt.get() != ptr || pAppend.get() != ptr);
    }
    AppedenSharedPtr pAppend;
    logeventPtr logevt;
};

class APLOG_DECLSPEC_PUBLIC ThreadPool
{
public:
    static ThreadPool* Instance();
    ThreadPool();
    ~ThreadPool();

    template <typename F, typename... Args>
    void pushJob(F &&f, Args &&... args) {
        {
            std::lock_guard<std::mutex> lc_(funcmutex_);
            taskqueue_.emplace(boost::bind(std::forward<F>(f), std::forward<Args>(args)...));
        }
        funccond_.notify_one();
    }
    
    void pushJob(AppedenSharedPtr append, logeventPtr evt);
    //void poolStart(int qsize);
    void poolStop();

private:
    static ThreadPool* instance_;
    int threadCount_;
    int qhead_;
    int qrear_;
    std::atomic<bool> isShutDown_;
    std::condition_variable apcond_;
    std::mutex apmutex_;
    SpinLock spinap_;
    std::condition_variable funccond_;
    std::mutex funcmutex_;
    SpinLock spinfunc_;
    size_t qsize_;
    boost::thread* threads_;
    Asynlog* appendArray_;
    std::queue<boost::function<void()>> taskqueue_;
    void appenderWork();
    void funcwork();
    Asynlog popAppender();
    bool isAppendQEmpty();
    void poolStart(int qsize);
    
    void clearAppenderJob();
    void clearFuncJob();   
};

}