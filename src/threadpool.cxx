/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-28 14:30:40
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-28 17:46:30
 * @FilePath: /aplog/src/threadpool.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "threadpool.h"
#include "boost/bind.hpp"
namespace aplog
{
ThreadPool* ThreadPool::instance_ = new ThreadPool();

ThreadPool::ThreadPool():
    threadCount_(boost::thread::hardware_concurrency() * 2),
    qhead_(0),
    qrear_(-1),
    isShutDown_(true),
    threads_(nullptr),
    appendArray_(nullptr)
{
    poolStart(50);
}

ThreadPool::~ThreadPool()
{
    poolStop();
}

ThreadPool* ThreadPool::Instance()
{
    if (instance_ == nullptr) {
        exit (1);
    }
    return instance_;
}

void ThreadPool::poolStart(int qsize)
{
    if(!isShutDown_)
        return;
    
    isShutDown_ = false;
    qsize_ = qsize;
    appendArray_ = new Asynlog[qsize_];
    if (appendArray_ ==  nullptr) {
        std::cerr << "new fail!"  << std::endl;
        exit(1);
    }
    
    threads_ = new boost::thread[threadCount_];
    if (threads_  == nullptr) {
        std::cerr << "new fail!"  << std::endl;
        exit(1);
    }

    for (int i = 0; i < threadCount_ / 2; i++) {
        threads_[i] = boost::thread(boost::bind(&ThreadPool::appenderWork, this));
    }

    for (int i = threadCount_ / 2; i < threadCount_ ; i++) {
        threads_[i] = boost::thread(boost::bind(&ThreadPool::funcwork, this));
    }
}

void ThreadPool::poolStop()
{
    if(isShutDown_)
        return;
    isShutDown_ = true;
    clearAppenderJob();
    clearFuncJob();

    for (int i = 0; i < threadCount_; i++) {
        if (threads_[i].joinable())
            threads_[i].join();
    }

    if(appendArray_) 
        delete[] appendArray_;
    if(threads_)
        delete[] threads_;
}

void ThreadPool::clearAppenderJob()
{
    apcond_.notify_all();
    std::lock_guard<std::mutex> lc_(apmutex_);
    while (!isAppendQEmpty()) {
        appendArray_[qhead_] = nullptr;
        qhead_ = (qhead_+1) % qsize_;
    }
}

void ThreadPool::clearFuncJob()
{
    funccond_.notify_all();
}

bool ThreadPool::isAppendQEmpty()
{
    return appendArray_[qhead_] == nullptr;
}

void ThreadPool::pushJob(AppedenSharedPtr append, logeventPtr evt)
{
    {
        //std::lock_guard<std::mutex> lc_(apmutex_);
        spinap_.lock();
        qrear_ = (++qrear_) % qsize_;
        appendArray_[qrear_].pAppend = std::move(append);
        appendArray_[qrear_].logevt = std::move(evt);
        spinap_.unlock();
    }
    apcond_.notify_one();
}

Asynlog ThreadPool::popAppender()
{  
    while(!isShutDown_) {
        std::unique_lock<std::mutex> lc_(apmutex_);
        apcond_.wait(lc_, [&] {
            return !isAppendQEmpty() || isShutDown_;
        });

        if(!isAppendQEmpty()) {
            Asynlog log = std::move(appendArray_[qhead_]);
            appendArray_[qhead_] = nullptr;
            qhead_ = (qhead_+1) % qsize_;
            lc_.unlock();
            return log;
        }
    }
    
    return Asynlog();
}

void ThreadPool::appenderWork()
{
    Asynlog asyncact;
    while ((asyncact = std::move(popAppender())) != nullptr) {
        if(asyncact.pAppend->isRun())
            asyncact.pAppend->doAppend(asyncact.logevt);
    }
}

void ThreadPool::funcwork()
{
    while(!isShutDown_) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lc_(funcmutex_);

            funccond_.wait(lc_, [&] {
                return !taskqueue_.empty() || isShutDown_;
            });
            
            if(!taskqueue_.empty()) {
                task = std::move(taskqueue_.front());
                taskqueue_.pop();
            }
        }
        
        if (task)
            task();
    }
}

}