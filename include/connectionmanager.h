/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-17 15:27:32
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-24 13:47:35
 * @FilePath: /aplog/include/connectionmanager.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include "connection.h"
#include "spinlock.h"
#include "pollmanager.h"
#include "transporttcp.h"
#include <vector>
#include <mutex>
#include <boost/signals2/signal.hpp>

namespace aplog
{
class APLOG_DECLSPEC_PUBLIC ConnectionManager
{
public:
    static ConnectionManager* Instance();
    static void addConnect(const ConnectionPtr& conn);
    static void dropConnection(const ConnectionPtr& conn);

private:
    ConnectionManager();
    ~ConnectionManager();
    
    static ConnectionManager* instance_; 
    PollManager* poll_;
    
    static std::vector<ConnectionPtr> connvec_;

    void timerStart(int);
    static void timerCallBack(int);
};

}

