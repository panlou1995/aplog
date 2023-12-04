/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-06-20 16:24:52
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 17:04:08
 * @FilePath: /aplog/include/logimpl.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include "appender.h"
#include <iostream>
#include <map>

namespace aplog
{
class Logger;
class LogEvent;

typedef std::map<string, Logger*> LogMap;

class APLOG_DECLSPEC_PUBLIC Logger
{
public:
    Logger& operator=(const Logger&) = delete;
    static Logger* getInstance(string name);
    AppedenSharedPtr getAppender() const {return pAppender_;}

    static void deleteLog(string name);

    void setLogLevel(LogLevel level);

    void setAppender(AppedenSharedPtr append);
    
    void write2Log(logeventPtr& evt);

    inline LogLevel getLogLevel() {return level_;};

    static bool hasLogger(string name);

private:
    Logger(const string name);
    ~Logger();
    
    string name_;

    static Logger* instance_;    
    
    static LogMap  logmap_;
    
    static void aplogStart();
    LogLevel level_;
    AppedenSharedPtr pAppender_;
    
    void asyncDeleteAppender();
    /*
    Layout layout;
    Filter filter
    */
};   
}
