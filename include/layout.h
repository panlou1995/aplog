/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-19 16:03:15
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 17:47:12
 * @FilePath: /aplog/include/layout.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "config.hxx"
#include "loggingevent.h"
#include <fstream>
#include <vector>

namespace aplog
{
class APLOG_DECLSPEC_PUBLIC LayOut
{
public:
    LayOut():patNodeHead_(new patNode) {}
    LayOut(const string pattern);
    ~LayOut();
    void execLocalPatLayoutWrite(std::ostream& os, logeventPtr evt);
    std::string getLogBuffer(logeventPtr evt);

private:
    enum tp {
        YEAR,
        MONTH,
        DATE,
        WEEKDAY,
        HOUR,
        MIN,
        SEC,
        MSEC,
        FILE,
        FUNC,
        LINE,
        CONTENT,
        NEWLINE,
        CHAR
    };
    typedef struct {
        char value;
        tp type;
    } item;

    typedef struct patNode{
        item* data;
        patNode* next;
        patNode() {this->next = nullptr;}
    } patNode;
    
    static char patdic[];
    patNode* patNodeHead_;

    bool checkValid(const string& str);
    void formPatternList(const  string& str);
    void freePatternList();
    std::string execPatLayout(logeventPtr evt);
    std::string execDefLayout(logeventPtr evt);
    const string level2str(LogLevel ll);
};

}