/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-03 16:12:51
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-15 14:59:01
 * @FilePath: /aplog/src/time.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "time.hxx"

namespace  aplog
{
Time::Time():
    curTime_(time(NULL)),
    localtime_(*localtime(&curTime_)),
    year_(localtime_.tm_year + 1900),
    mon_(localtime_.tm_mon + 1),
    date_(localtime_.tm_mday),
    day_(localtime_.tm_wday),
    hour_(localtime_.tm_hour),
    min_(localtime_.tm_min),
    sec_(localtime_.tm_sec)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    ms_ = (int)(tv.tv_usec/1000);
}

#if 0
std::ostream& operator<<(std::ostream& os, const Time& time)
{
    /*该函数有性能开销*/
    //os << std::put_time(std::localtime(&time.curTime_), "%Y-%m-%d %H:%M:%S") << "." << ms_;
    return os;
}
#endif
}