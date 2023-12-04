/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-06-20 16:12:17
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 18:28:17
 * @FilePath: /aplog/include/connfig.hxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include <string>
#include <utility>
#include <memory>
#include <cstdint>
#include <boost/function.hpp>
#include <boost/shared_array.hpp>

#define APLOG_DECLSPEC_PUBLIC __attribute__ ((visibility("default")))
#define APLOG_DECLSPEC_PRIVATE __attribute__ ((visibility("hidden")))


namespace aplog{
enum {
    CREATE_APPENDER,
    NORMAL_WRITE,
    DESTROY_APPENDER
};

class LogImpl;
class Time;
class Appender;
class LogEvent;
class LayOut;
class TransportTcp;
class Connection;
class SocketAppender;

typedef std::string 
string;

typedef int8_t
LogLevel;

typedef std::shared_ptr<LogImpl>                                                
LogSharedPtr; 

typedef std::shared_ptr<Appender>                                               
AppedenSharedPtr;

typedef std::shared_ptr<LayOut>                                                 
LayoutSharedPtr;

typedef std::shared_ptr<std::string>                                            
stringPtr;

typedef std::shared_ptr<LogEvent>                                               
logeventPtr;

typedef std::shared_ptr<Time>                                                   
TimeSharedPtr;

typedef std::shared_ptr<TransportTcp>                                           
TcpTransportPtr;

typedef std::shared_ptr<Connection>                                             
ConnectionPtr;

typedef boost::function<void(int)>                                              
socketUpdateFunc; 

typedef boost::function<void(const TcpTransportPtr&)>                           
tcpCallBack;

typedef boost::function<void(const ConnectionPtr&, const boost::shared_array<uint8_t>&, uint32_t)> 
readFiniCallBack;

typedef boost::function<void()> 
writeFiniCallBack;

typedef boost::function<void(const ConnectionPtr&)> 
dropConnectionCallBack;

typedef boost::function<void()> 
clientConnSuccessCallBack;
}

