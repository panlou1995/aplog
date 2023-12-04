/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-07 17:16:59
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-23 15:46:56
 * @FilePath: /aplog/src/appendconsole.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "appendconsole.h"
#include "loggingevent.h"
#include "time.hxx"
#include <iostream>

namespace aplog
{
void ConsoleAppender::doAppend(logeventPtr& evt)
{
    std::lock_guard<std::mutex> l_(mutex_);
    layout_->execLocalPatLayoutWrite(std::cout, evt);
}

}