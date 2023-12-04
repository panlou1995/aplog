/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-06-20 16:05:07
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 18:35:15
 * @FilePath: /aplog/src/logger.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "logger.h"
#include "config.hxx"
#include "threadpool.h"
#include "pollmanager.h"
#include <future>
#include <thread>
#include <chrono>

namespace aplog
{
Logger* Logger::instance_ = nullptr;
std::map<string, Logger*> Logger::logmap_ = std::map<string, Logger*>();

Logger*
Logger::getInstance(string name) {
    if (instance_ == nullptr) {
        instance_ = new Logger("__root__");
        aplogStart();
    }
    auto it = logmap_.find(name);
    if (it != logmap_.end()) {
        std::cout << "logger exist!" << std::endl;
        return it->second;
    } else {
        Logger* plog = new Logger(name);
        if (plog == nullptr) {
            //throw error...
            exit (1);
        }
        logmap_.insert(std::make_pair(name, plog));
        return plog;
    }
}

void Logger::deleteLog(string name)
{
    auto it = logmap_.find(name);
    if (it != logmap_.end()) {
        std::cout << "in log erase!" << std::endl;
        if (it->second)
            delete it->second;
        logmap_.erase(it);
    } else
        std::cout << "not found:" << name << std::endl;
}

Logger::Logger(const string name):
    name_(name),
    level_(-1),
    pAppender_(nullptr)
{
    
}

bool Logger::hasLogger(string name)
{
    auto it = logmap_.find(name);
    if (it != logmap_.end()) {
        return true;
    } else {
        return false;
    }
}

void Logger::aplogStart()
{
    //ThreadPool::Instance()->poolStart(1000);
    PollManager::Instance()->startclient();
}

void Logger::asyncDeleteAppender()
{
    std::cout << "delete async appender!  " << __func__ << ":"<<  __LINE__ << std::endl;
    pAppender_->stop();
    pAppender_.reset();
}

Logger::~Logger()
{
    std::cout << "in logger delete!" << std::endl;
    if (pAppender_ != nullptr) {
        if (pAppender_->isNeedAsync()) {
            std::async(std::launch::async, boost::bind(&Logger::asyncDeleteAppender, this));
        }
        else {
            /*同步，大胆删除*/
            pAppender_.reset();
            std::cout << "reset async sharptr!" << __func__ << ":"<<  __LINE__ << std::endl;
        }
    }
}

void Logger::setLogLevel(LogLevel level)
{
    level_ = level;
}

void Logger::setAppender(AppedenSharedPtr append)
{
    if (append) {
        if (pAppender_ != nullptr) {
            pAppender_.reset();
        }
        pAppender_ = append;

        assert(name_  != "__root__");
        pAppender_->setName(name_);
    } 
    else
        std::cerr << "append is null" << std::endl;
}

void Logger::write2Log(logeventPtr& evt)
{   
    if (pAppender_ == nullptr)
        return;
        
    if (pAppender_->isNeedAsync()) {
        ThreadPool::Instance()->pushJob(pAppender_, evt);
    } else {
        pAppender_->doAppend(evt);
    }
}

#if 0
Logger::Logger():
    logDefault_(std::make_shared<aplog::LogImpl>())
{

}

Logger::~Logger()
{
    //do something
}

LogSharedPtr
Logger::getLogImpl(const string& name)
{
    if (getInstance() == nullptr) {
        std::cout << "malloc error" <<std::endl;
        return nullptr;
    }

    auto it = logMap_.find(name);
    if (it != logMap_.end()) {
        std::cout << "logger exist!" << std::endl;
        return it->second;
    } else {
        LogSharedPtr implPtr = std::make_shared<LogImpl>();
        if (implPtr == nullptr) {
            //throw error...
        }
        logMap_.insert(std::make_pair(name, implPtr));
        return implPtr;
    }
}

LogSharedPtr
Logger::getImplDefault()
{
    if (getInstance() == nullptr) {
        std::cout << "malloc error" <<std::endl;
        return nullptr;
    }
    
    return logDefault_;
}
#endif

}