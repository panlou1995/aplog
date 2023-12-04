/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-06-21 14:08:43
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 18:31:50
 * @FilePath: /aplog/include/appender.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "loggingevent.h"
#include "layout.h"
#include "spinlock.h"
#include <atomic>
#include <iostream>
namespace aplog
{
class APLOG_DECLSPEC_PUBLIC Appender
{
public:
    Appender(bool needAsync = false):isRun_(true),needAsync_(needAsync),layout_(new LayOut()) {};
    virtual ~Appender() {layout_.reset();};
    //Appender& operator=(const Appender&);
    //Appender* operator=(const Appender*);
    void setName(const string& name) {name_ = name;}
    string getName() const {return name_;}
    void setLayOut(LayoutSharedPtr layout);
    
    LayoutSharedPtr getLayOut() const {return layout_;}
    virtual void doAppend(logeventPtr& evt) {};
    void stop() {isRun_ = false;}
    bool isNeedAsync() {return needAsync_;};
    bool isRun() {return isRun_;}

protected:
    string name_;
    std::atomic<bool> isRun_;
    bool needAsync_;
    LayoutSharedPtr layout_;
    SpinLock spinlock_;
private:
    
};

}