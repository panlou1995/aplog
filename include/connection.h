/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-11 15:16:57
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 09:36:48
 * @FilePath: /aplog/include/connection.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "config.hxx"
#include "transporttcp.h"
#include <memory>
#include <mutex>
#include <atomic>
namespace aplog
{
class APLOG_DECLSPEC_PUBLIC Connection : public std::enable_shared_from_this<Connection>
{
public:
    Connection(const TcpTransportPtr& tcp, bool isServer);
    ~Connection();
    
    TcpTransportPtr transport() const {return trasporttcp_;}
    bool isServer() {return is_server_;}
    void initialize();
    void onReadable(const TcpTransportPtr&);
    void onWritable(const TcpTransportPtr&);
    void setLogAppendCreateHandler(const readFiniCallBack& cb) {logCreateAppendHandler_ = cb;}
    void setLogOptionHandler(const readFiniCallBack& cb) {logOptionSetHandler_ = cb;}
    void setLogWriteHandler(const readFiniCallBack& cb) {logExecHandler_ = cb;}
    void setWrittenCb(writeFiniCallBack cb) {writefini_cb_ = cb;}
    void read(uint32_t len, const readFiniCallBack& cb);
    void write(const boost::shared_array<uint8_t>& buffer, uint32_t size, const writeFiniCallBack& finished_callback, bool immedate = true);

    void lengthRead(const ConnectionPtr&, boost::shared_array<uint8_t>, uint32_t);

private:
    TcpTransportPtr trasporttcp_;
    bool is_server_;
    bool option_set_;

    uint32_t read_size_;
    uint32_t has_read_;
    boost::shared_array<uint8_t> read_buffer_;
    readFiniCallBack  readfini_cb_;
    std::atomic<bool> read_done_;
    
    uint32_t write_size_;
    uint32_t has_written_;
    std::atomic<bool> write_done_;
    boost::shared_array<uint8_t> write_buffer_;
    writeFiniCallBack  writefini_cb_;
    
    readFiniCallBack logCreateAppendHandler_;
    readFiniCallBack logExecHandler_;
    readFiniCallBack logOptionSetHandler_;
    
    //std::mutex write_mutex_;
    //std::mutex read_mutex_;
    boost::recursive_mutex read_lock_;
    boost::recursive_mutex write_lock_;
    void readTransport();
    void writeTransport();
};
}