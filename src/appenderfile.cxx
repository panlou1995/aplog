/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-06-26 18:13:04
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-22 18:48:22
 * @FilePath: /aplog/src/appenderfile.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "appendfile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "time.hxx"

namespace aplog
{
FileAppender::FileAppender(const string& filename, std::ios_base::openmode mode, bool needAsync, bool flush):
    Appender(needAsync),
    file_(filename),
    openmode_(mode),
    flushImmedia_(flush)
{

}

#if 0
string FileAppender::logNameGen(string name, uint8_t suffix)
{
    if (suffix != 255)
        return name + "." + std::to_string(suffix);
    return name;
}

bool FileAppender::isFileExceedSize(string file, size_t size)
{
    struct stat fst;
    if (stat(file.c_str(), &fst)) {
        std::cerr << "stat " << file << " fail" << std::endl;
        return true;
    }

    return fst.st_size >= size;
}
#endif

RollingFileAppender::RollingFileAppender(const string& filename, int filenum, size_t size, bool needAsync, bool flush) 
    :FileAppender(filename, std::ios_base::out | std::ios_base::app, needAsync, flush),
    logfile_(filename),
    maxsize_(size),
    filenum_(filenum)
{
    init();
}

RollingFileAppender::~RollingFileAppender()
{
    std::cout << "rollingfile destruct" << std::endl;
    if (outfile_.is_open()) {
        outfile_.clear();
        outfile_.close();
        std::cout << "close file!" << std::endl;
    }
}

void RollingFileAppender::init()
{
    outfile_.open(logfile_, openmode_);
    if (!outfile_.is_open()) {
        std::cerr  <<  "open " 
                   << logfile_ 
                   << " fail!" 
                   << std::endl;
    }
}

void RollingFileAppender::doAppend(logeventPtr& evt)
{
    spinlock_.lock();
    if (outfile_.tellp() > maxsize_) {
        rollover();
    }

    layout_->execLocalPatLayoutWrite(outfile_, evt);

    if (flushImmedia_)
        outfile_.flush();
    spinlock_.unlock();
}

void RollingFileAppender::rollover()
{
    if (!outfile_.good())
        return;
    
    outfile_.clear();
    outfile_.close();
    
    if (filenum_ > 1) {
        string fileMax(logfile_ + "." + std::to_string(filenum_));
        std::remove(fileMax.c_str());
        for (int i = filenum_-1; i > 0;) {
            string target, source;
            --i;
            if (i == 0) {
                source = string(logfile_);
                target = string(logfile_ + ".1");
            } else {
                source = string(logfile_ + "." + std::to_string(i));
                target = string(logfile_ + "." + std::to_string(i+1));
            }

            std::rename(source.c_str(), target.c_str());
        }

        string newfile(logfile_ + ".1");
        std::rename(logfile_.c_str(), newfile.c_str());
    }

    outfile_.open(logfile_, std::ios_base::out |std::ios_base::trunc);
    if (!outfile_.is_open()) {
        std::cerr  <<  "rollover " 
                    << logfile_ 
                    << " fail!" 
                    << std::endl;
    }
}

CommonFileAppender::CommonFileAppender(const string& filename, size_t size, bool needAsync, bool flush)
    : FileAppender(filename, std::ios_base::out | std::ios_base::app, needAsync, flush),
    maxsize_(size)
{
    init();
}

CommonFileAppender::~CommonFileAppender()
{
    std::cout << "commonfile destruct" << std::endl;
    if (outfile_.is_open()) {
        outfile_.clear();
        outfile_.close();
        std::cout << "close file!" << std::endl;
    }
}

void CommonFileAppender::init()
{
    outfile_.open(logfile_, openmode_);
    if (!outfile_.is_open()) {
        std::cerr  <<  "open " 
                   << logfile_ 
                   << " fail!" 
                   << std::endl;
    }
}

void CommonFileAppender::doAppend(logeventPtr& evt)
{
    spinlock_.lock();

    if (!outfile_.good()) {
        std::cerr << logfile_ << "not good" << std::endl;
        return;
    }
    
    if (outfile_.tellp() > maxsize_) {
        rollover();
    }
    
    layout_->execLocalPatLayoutWrite(outfile_, evt);
    
    if (flushImmedia_)
        outfile_.flush();
        
    spinlock_.unlock();
}

void CommonFileAppender::rollover()
{
    if (!outfile_.good())
        return;
    
    outfile_.clear();
    outfile_.close();
    
    outfile_.open(logfile_, std::ios_base::out |std::ios_base::trunc);
    if (!outfile_.is_open()) {
        std::cerr  <<  "rollover " 
                   << logfile_ 
                   << " fail!" 
                   << std::endl;
    }
}

}