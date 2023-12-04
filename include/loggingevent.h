/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-06-21 14:12:57
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-15 13:49:06
 * @FilePath: /aplog/include/loggingevent.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include "time.hxx"
#include "boost/filesystem.hpp"
namespace aplog
{
class APLOG_DECLSPEC_PUBLIC LogEvent
{
public:
    LogEvent() {};
    LogEvent(stringPtr message, LogLevel loglevel, const char* filename, const char* func, int line, bool timerec);
      
    virtual ~LogEvent();

    LogLevel getlevel() const {return loglevel_;}
    string getMsg() const {return *msg_;}
    bool needTimeRec() const {return tmrecflag_;}
    int year() const {return ptime_->year_;}
    int month() const {return ptime_->mon_;}
    int date() const {return ptime_->date_;}
    int weekday() const {return ptime_->day_;}
    int hour() const {return ptime_->hour_;}
    int min() const {return ptime_->min_;}
    int sec() const  {return ptime_->sec_;}
    int msec() const {return ptime_->ms_;}
    string file() const {return boost::filesystem::path(file_).filename().string();}
    string func() const {return func_;}
    int line() const {return line_;}
    string content() const {return *msg_;}
    
    TimeSharedPtr ptime_;
private:
    stringPtr msg_;
    LogLevel loglevel_;
    string file_;
    string func_;
    int line_;
    bool tmrecflag_;
};
}