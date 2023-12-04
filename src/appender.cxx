/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-17 11:29:59
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-07-28 17:24:15
 * @FilePath: /aplog/src/appender.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "appender.h"

namespace aplog
{
#if 0
Appender& Appender::operator=(const Appender&  other)
{
    std::cout <<  "*********operator class!******" << std::endl;
    this->needAsync_ = other.needAsync_;
    this->decreAsynUseCount();
}

Appender* Appender::operator=(const Appender* other)
{
    std::cout <<  "*********operator pointer!******" << std::endl;
    this->needAsync_ = other->needAsync_;
    this->decreAsynUseCount();
}
#endif

void Appender::setLayOut(LayoutSharedPtr layout)
{
    if (layout) {
        if (layout_ != nullptr) {
            layout_.reset();
        }
        layout_ = layout;
    } 
    else
        std::cerr << "layout is null" << std::endl;
}


}