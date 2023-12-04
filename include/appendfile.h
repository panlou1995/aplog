/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-06-21 14:48:09
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-03 17:09:45
 * @FilePath: /aplog/include/appendfile.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AEinco
 */
#pragma once

#include "config.hxx"
#include "appender.h"
#include <iostream>
#include <fstream>
#include <mutex>

namespace aplog
{
class APLOG_DECLSPEC_PUBLIC FileAppender:public Appender
{
public:
    FileAppender(const string& filename, std::ios_base::openmode mode, bool needAsync, bool flush);
    virtual ~FileAppender() {std::cout << "fileappender destruct" << std::endl;};

protected:
    virtual void init() = 0;
    std::ofstream outfile_;
    string file_;
    std::ios_base::openmode openmode_;
    bool flushImmedia_;
    
};

class APLOG_DECLSPEC_PUBLIC RollingFileAppender:public FileAppender
{
public:
    RollingFileAppender(const string& filename, int filenum, size_t size, bool needAsync = false, bool flush = true);
    ~RollingFileAppender();
    void doAppend(logeventPtr& evt) override;
private:
    void init();
    void rollover();
    string logfile_;
    int filenum_;
    size_t maxsize_;
};

class APLOG_DECLSPEC_PUBLIC CommonFileAppender:public FileAppender
{
public:
    CommonFileAppender(const string& filename, size_t maxsize = 65536, bool needAsync = false, bool flush = true);
    void doAppend(logeventPtr& evt) override;
    ~CommonFileAppender();
private:
    void init();
    void rollover();
    string logfile_;
    size_t maxsize_;
};

}