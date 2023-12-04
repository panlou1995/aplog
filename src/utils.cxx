/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-23 16:16:02
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-23 16:18:27
 * @FilePath: /aplog/src/utils.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "utils.h"

namespace aplog{

uint32_t util::getCurrentSec()
{
    auto now = std::chrono::system_clock::now();

    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm* local_time = std::localtime(&now_time_t);

    return local_time->tm_sec;
}

}