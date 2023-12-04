/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-19 17:07:26
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 17:54:50
 * @FilePath: /aplog/src/layout.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include  <stdlib.h>
#include "layout.h"
#include <iostream>
#include "aplog.h"
#include "time.hxx"
namespace aplog
{
char LayOut::patdic[] = {'D','F','L','M','Y','c','d','f','h','m','n','q','s'};

LayOut::LayOut(const string pattern):
    patNodeHead_(new patNode)
{
    if(patNodeHead_->next != nullptr)  {
        freePatternList();
    }

    if(checkValid(pattern)) {
        std::cout << "check pattern pass!"  <<  std::endl;
        formPatternList(pattern);
    } else {
        std::cout << "check pattern fail!"  <<  std::endl;
    }
}

LayOut::~LayOut()
{
    std::cout << "layout destruct" << std::endl;
    freePatternList();
    delete patNodeHead_;
}

void LayOut::formPatternList(const string& pattern)
{
    patNode* tail = patNodeHead_;
    for (size_t i = 0; i < pattern.size();) {
        patNode* node = (patNode*)calloc(1, sizeof(patNode));
        node->data = (item*)calloc(1, sizeof(item)); 
        if(!node || !node->data) {
            std::cerr << "malloc fail" << std::endl;
            exit(1);
        }
        if (pattern[i] != '%') {
            node->data->type = CHAR;
            node->data->value = pattern[i];
            i++;
        } else if (pattern[i] == '%' && i != pattern.size()-1) {
            switch(pattern[i+1]) {
                case 89: {//'Y'
                    node->data->type = YEAR;
                    break;
                }

                case 77: {//'M'
                    node->data->type = MONTH;
                    break;
                }

                case 68: {//'D'
                    node->data->type = DATE;
                    break;
                }

                case 100: {//'d'
                    node->data->type = WEEKDAY;
                    break;
                }

                case 104: {//'h'
                    node->data->type = HOUR;
                    break;
                }

                case 109: {//'m'
                    node->data->type = MIN;
                    break;
                }

                case 115: {//'s'
                    node->data->type = SEC;
                    break;
                }

                case 113: {//'q'
                    node->data->type = MSEC;
                    break;
                }

                case 70: {//'F'
                    node->data->type = FILE;
                    break;
                }

                case 102: {//'f'
                    node->data->type = FUNC;
                    break;
                }

                case 76: {//'L'
                    node->data->type = LINE;
                    break;
                }

                case 99: {//'c'
                    node->data->type = CONTENT;
                    break;
                }

                case 110: {//'n'
                    node->data->type = NEWLINE;
                    break;
                }

                default: 
                    break;   
            }
            i+=2;
        }
        tail->next = node;
        node->next = nullptr;
        tail = node;
    }
}

void LayOut::freePatternList()
{
    if(!patNodeHead_->next)
        return;

    for(patNode* node = patNodeHead_->next; node != nullptr;) {
        patNode* tmp = node->next;
        free(node->data);
        free(node);
        node = tmp;
    }
}

bool LayOut::checkValid(const string& str)
{
    bool ret = false;
    
    for (size_t i = 0; i < str.length(); i++) {
        uint8_t headIdx = 0;
        uint8_t tailIdx = sizeof(patdic)/sizeof(char);
        uint8_t midIdx = tailIdx / 2;
        if(str[i] == '%' && i != str.length()-1) {
            char c = str[i+1];
            if (c == '%') {
                /*暂不支持转义*/
                std::cerr << "invalid:%" << c << std::endl;
                return false;
            }

            while(patdic[midIdx] != c && midIdx != headIdx && midIdx != tailIdx) {
                if(c > patdic[midIdx]) {
                    headIdx = midIdx;
                } else {
                    tailIdx = midIdx;
                }
                midIdx = (headIdx + tailIdx) / 2;
            }

            if(patdic[midIdx] != c) {
                std::cerr << "invalid:%" << c << std::endl;
                return false;
            }else {
                ret = true;
            }
        }
    }
    return ret;
}

std::string LayOut::getLogBuffer(logeventPtr evt)
{
    if(!patNodeHead_->next)
        return execDefLayout(evt);
    else
        return execPatLayout(evt);
}

void LayOut::execLocalPatLayoutWrite(std::ostream& os, logeventPtr evt)
{
    std::string buf;
    if(!patNodeHead_->next)
        buf = execDefLayout(evt);
    else
        buf = execPatLayout(evt);
    
    os << buf;
}

std::string LayOut::execDefLayout(logeventPtr evt)
{
    size_t len = 1024 + evt->getMsg().length() + evt->file().length();
    char buf[len];
    memset(buf, 0, len);
    
    if (!evt->needTimeRec()) {
        snprintf(buf, len, "%s[%s:%d] %s",
            level2str(evt->getlevel()).c_str(),
            evt->file().c_str(),
            evt->line(),
            evt->getMsg().c_str());
    } else {
        snprintf(buf, len, "%s[%04d-%02d-%02d %02d:%02d:%02d.%03d][%s:%d] %s", 
            level2str(evt->getlevel()).c_str(),
            evt->year(),
            evt->month(),
            evt->date(),
            evt->hour(),
            evt->min(),
            evt->sec(),
            evt->msec(),
            evt->file().c_str(),
            evt->line(),
            evt->getMsg().c_str()
        );
    }
    
    return buf;
}

std::string LayOut::execPatLayout(logeventPtr evt)
{
    size_t buffsize = 1024 + evt->getMsg().length() + evt->file().length();
    char buf[buffsize];
    memset(buf, 0, buffsize);

    int len = 0;
    len = snprintf(buf, buffsize, "%s", level2str(evt->getlevel()).c_str());

    for (patNode* node = patNodeHead_->next; node != nullptr; node = node->next) {
        switch(node->data->type) {
            case YEAR:
                //os << evt->year();
                len += snprintf(buf+len, buffsize, "%04d", evt->year());
                break;
            case MONTH:
                //os << evt->month();
                len += snprintf(buf+len, buffsize, "%02d", evt->month());
                break;
            case DATE:
                //os << evt->date();
                len += snprintf(buf+len, buffsize, "%02d", evt->date());
                break;
            case WEEKDAY:
                //os << evt->weekday();
                len += snprintf(buf+len, buffsize, "%02d", evt->weekday());
                break;
            case HOUR:
                //os << evt->hour();
                len += snprintf(buf+len, buffsize, "%02d", evt->hour());
                break;
            case MIN:
                //os << evt->min();
                len += snprintf(buf+len, buffsize, "%02d", evt->min());
                break;
            case SEC:   
                //os << evt->sec();
                len += snprintf(buf+len, buffsize, "%02d", evt->sec());
                break;
            case MSEC:
                //os << evt->msec();
                len += snprintf(buf+len, buffsize, "%03d", evt->msec());
                break;
            case FILE:
                //os << evt->file();
                len += snprintf(buf+len, buffsize, "%s", evt->file().c_str());
                break;
            case FUNC:
                //os << evt->func();
                len += snprintf(buf+len, buffsize, "%s", evt->func().c_str());
                break;
            case LINE:  
                //os << evt->line();
                len += snprintf(buf+len, buffsize, "%d", evt->line());
                break;
            case NEWLINE:
                //os << std::endl;
                len += snprintf(buf+len, buffsize, "\n");
                break;
            case CONTENT:
                //os << evt->getMsg();
                len += snprintf(buf+len, buffsize, "%s", evt->getMsg().c_str());
                break;
            case CHAR:
                //os << node->data->value;
                len += snprintf(buf+len, buffsize, "%c", node->data->value);
                break;
            default:
                break;
        }
    }

    return buf;
}

const string LayOut::level2str(LogLevel ll)
{
    switch (ll)
    {
    case TRACE_LOG:
        return "[TRACE]";
    case DEBUG_LOG:
        return "[DEBUG]";
    case INFO_LOG:
        return "[INFO]";
    case WARN_LOG:
        return "[WARN]";
    case ERROR_LOG:
        return "[ERROR]";
    case FATAL_LOG:
        return "[FATAL]";
    default:
        return "[NONE]";
    }
}

}