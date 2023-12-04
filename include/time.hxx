/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-03 16:08:45
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-15 15:19:07
 * @FilePath: /aplog/include/time.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sys/time.h>

namespace aplog
{
class APLOG_DECLSPEC_PRIVATE Time
{
public:
    Time() ;
    //friend std::ostream& operator<<(std::ostream& os, const Time& time);

    int year_;
    int mon_;
    int date_;
    int day_;
    int hour_;
    int min_;
    int sec_; 
    int ms_;
    
private:
    std::time_t curTime_;
    std::tm localtime_;   
};
   
}