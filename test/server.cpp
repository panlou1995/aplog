/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-04 16:33:59
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-28 11:15:57
 * @FilePath: /aplog/test/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <iostream>
#include <memory>
#include "logger.h"
#include "appendfile.h"
#include "appendconsole.h"
#include "threadpool.h"
#include "aplog.h"
#include "unistd.h"
#include "layout.h"
#include <thread>
#include "aplogserver.h"

void run()
{
    auto log = aplog::Logger::getInstance("panlou");
    auto logconsole = aplog::Logger::getInstance("console");
    int cnt = -20;
    while(++cnt < 0) {
        APLOG_PRIN_DEBUG(log, "This is %s! from  thread, cnt=%d, pid=%d\n", "me", cnt, std::this_thread::get_id());

        APLOG_TIME_PRIN_DEBUG(log, "hello world! from thread, cnt=%d, pid=%d\n", cnt, std::this_thread::get_id());

        //APLOG_PRIN_DEBUG(logconsole, "This is %s! from  thread\n", "me");

        //APLOG_TIME_PRIN_DEBUG(logconsole, "hello world! from thread\n");
        //sleep(1);
    }
}

int main()
{
#if 0
    aplog::Logger* log = aplog::Logger::getInstance("panlou");
    //aplog::Logger* logconsole = aplog::Logger::getInstance("console");
 
    auto appender = std::make_shared<aplog::RollingFileAppender>("test_pan.txt", 10, 1024*1024*2, true);
    log->setAppender(appender);

    auto layout = std::make_shared<aplog::LayOut>("%Y-%M-%D:%h.%m.%s.%q %c");
    appender->setLayOut(layout);

    //auto appendercon = std::make_shared<aplog::ConsoleAppender>();
    //logconsole->setAppender(appendercon);
 
    //std::thread  t1(run);
    int cnt = 50000;
    while(cnt --) {
        APLOG_PRIN_DEBUG(log, "hello world! cnt=%d, pid=%d\n", cnt, std::this_thread::get_id());
        APLOG_PRIN_DEBUG(log, "This is apLog! cnt=%d, pid=%d\n", cnt, std::this_thread::get_id());
        APLOG_PRIN_DEBUG(log, "This is %s! cnt=%d, pid=%d\n", "me", cnt, std::this_thread::get_id());

        APLOG_TIME_PRIN_DEBUG(log, "hello world! cnt=%d\n", cnt);
        APLOG_TIME_PRIN_DEBUG(log, "This is apLog! cnt=%d\n", cnt);
        APLOG_TIME_PRIN_DEBUG(log, "This is %s! cnt=%d\n", "me", cnt);

        //APLOG_PRIN_DEBUG(logconsole, "hello world!\n");
        //APLOG_PRIN_DEBUG(logconsole, "This is apLog!\n");
        //APLOG_PRIN_DEBUG(logconsole, "This is %s!\n", "me");

        //APLOG_TIME_PRIN_DEBUG(logconsole, "hello world!\n");
        //APLOG_TIME_PRIN_DEBUG(logconsole, "This is apLog!\n");
        //APLOG_TIME_PRIN_DEBUG(logconsole, "This is %s!\n", "me");
    }

    //t1.detach();    
    aplog::Logger::deleteLog("panlou");
    aplog::Logger::deleteLog("console");
#endif


    aplog::AplogServer server(8888, 5);
    server.listen();
    server.eventloop();
    return 0;
}
