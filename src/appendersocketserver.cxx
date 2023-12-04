/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-16 14:58:47
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-25 11:25:39
 * @FilePath: /aplog/src/appendersocket.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "appendsocketserver.h"
#include "connection.h"
#include "connectionmanager.h"
#include "nlohmann/json.hpp"
#include "utils.h"

namespace aplog
{
SocketAppenderServer::SocketAppenderServer(const string& filename, int filenum, size_t size):
    SocketAppender(),
    logfile_(filename),
    filenum_(filenum),
    maxsize_(size)
{
    init();
}

SocketAppenderServer::~SocketAppenderServer()
{

}

void SocketAppenderServer::init()
{
    outfile_.open(logfile_, std::ios_base::out | std::ios_base::app);
    if (!outfile_.is_open()) {
        std::cerr  <<  "open " 
                << logfile_ 
                << " fail!" 
                << std::endl;
    }
}

void SocketAppenderServer::execLogWrite(const stringPtr& msg)
{
    /*TODO:是否要用锁*/
    if (outfile_.tellp() > maxsize_) {
        rollover();
    }

    outfile_ << *msg;
    outfile_.flush();
}   

void SocketAppenderServer::rollover()
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

}