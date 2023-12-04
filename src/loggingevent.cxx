/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-06-28 16:33:09
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-15 15:06:46
 * @FilePath: /aplog/src/logevt.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "loggingevent.h"
#include <iostream>
namespace aplog
{
LogEvent::LogEvent(stringPtr message, LogLevel loglevel, const char* filename, const char* func, int line, bool timerec):
    msg_(message),
    loglevel_(loglevel),
    file_(filename),
    func_(func),
    line_(line),
    tmrecflag_(timerec),
    ptime_(new Time)
{
    
}

LogEvent::~LogEvent()
{
    //std::cout << "delete msg: " <<  *msg_ << std::endl;
    if (ptime_ != nullptr) 
        ptime_.reset();
    
    msg_.reset();
}

/*
void LogEvent::recTime()
{
    auto now = std::chrono::system_clock::now();
    
    ms_ = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    curTime_ = std::chrono::system_clock::to_time_t(now);

    std::cout << "Current time: " << std::put_time(std::localtime(&curTime_), "%Y-%m-%d %H:%M:%S");

    std::cout << "." << std::setfill('0') << std::setw(3) << ms.count() << std::endl;

}
*/


}