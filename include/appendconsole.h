/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-07 15:49:48
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-07-25 14:59:24
 * @FilePath: /aplog/include/appendconsole.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include "appender.h"
#include <iostream>
#include <mutex>

namespace aplog
{
class APLOG_DECLSPEC_PUBLIC ConsoleAppender:public Appender
{
public:
    ConsoleAppender():Appender(false) {};
    ~ConsoleAppender() {};
    void doAppend(logeventPtr& evt) override;

private:
    std::mutex mutex_;   
};

}