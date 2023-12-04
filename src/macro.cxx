/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-04 10:53:08
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-03 17:05:40
 * @FilePath: /aplog/src/macro.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include  "config.hxx"
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

using namespace aplog;

stringPtr funformat(const char *format, ...) 
{
    va_list arg;
    va_start(arg, format);
    stringPtr pbuf(new std::string(512, '\0'));
    int ret;
    while ((ret = vsnprintf((char *)pbuf->c_str(), pbuf->size(), format, arg)) >= pbuf->size()) {
        pbuf->resize(pbuf->size() * 2);
        va_start(arg, format);
    }

    pbuf->resize(ret);
    va_end(arg);
    return pbuf;
}