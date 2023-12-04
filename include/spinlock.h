/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-10 09:30:46
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-23 15:33:37
 * @FilePath: /aplog/include/spinlock.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include <atomic>

namespace aplog
{
class SpinLock {
public:
    SpinLock():flag_(ATOMIC_FLAG_INIT) {}
    ~SpinLock() {unlock();}
    void lock() {
        while(flag_.test_and_set(std::memory_order_acquire)) {
        }
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
private:
    std::atomic_flag flag_;
};    
} 