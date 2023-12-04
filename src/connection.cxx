/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-08-11 16:20:41
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-09-04 18:44:48
 * @FilePath: /aplog/src/connection.cxx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%A
 */
#include "connection.h"

namespace aplog
{
Connection::Connection(const TcpTransportPtr& tcp, bool isServer):
    trasporttcp_(tcp),
    is_server_(isServer),
    option_set_(false),
    read_size_(0),
    has_read_(0),
    read_done_(true),
    write_size_(0),
    has_written_(0),
    write_done_(true)
{

}

Connection::~Connection()
{

}

void Connection::initialize()
{
#if 0
    this->setLogAppendCreateHandler(boost::bind(&clientAppendCreate, _1, _2, _3));
    this->setLogOptionHandler(boost::bind(&clientSetAppend, _1, _2, _3));
    this->setLogWriteHandler(boost::bind(&clientExecLog, _1, _2, _3));
#endif
}

void Connection::onReadable(const TcpTransportPtr& trasporttcp)
{
    assert(trasporttcp == trasporttcp_);
    std::cout << "on read! read_size_:" << read_size_ << std::endl;

    if(!read_done_)
        readTransport();
}

void Connection::onWritable(const TcpTransportPtr& trasporttcp)
{   
    std::cout << "onWrite!" << std::endl;  
    assert(trasporttcp == trasporttcp_);
    std::cout << "write_size:" << write_size_ <<std::endl;  
    if(!write_done_)
        writeTransport();
}

void Connection::read(uint32_t len, const readFiniCallBack& cb)
{
    boost::recursive_mutex::scoped_lock lc(read_lock_);
    //std::lock_guard<std::mutex> lc(read_mutex_);
    read_size_ = len;
    has_read_ = 0;
    readfini_cb_ = cb;
    read_done_ = false;
    read_buffer_ = boost::shared_array<uint8_t>(new uint8_t[len]);
    std::memset(read_buffer_.get(), 0, len);
    //TODO:socket建立时应该就确定了读写属性
    trasporttcp_->enableRead();
    readTransport();
}

void Connection::readTransport()
{
    boost::recursive_mutex::scoped_lock lc(read_lock_);
    //std::lock_guard<std::mutex> lc(read_mutex_);
    
    if(read_done_)
        return;

    uint32_t to_read = read_size_ - has_read_;
    int32_t read_byte = trasporttcp_->read(read_buffer_.get() + has_read_, to_read);
    if (read_byte < 0) {
        //connect dropped
        return;
    }

    has_read_ += read_byte;
    
    if(has_read_ == read_size_ && readfini_cb_) {
        readfini_cb_(shared_from_this(), read_buffer_, read_size_);
        read_size_ = 0;
        has_read_ = 0;
        read_done_ = true;
    }
}

void Connection::write(const boost::shared_array<uint8_t>& buffer, uint32_t len, const writeFiniCallBack& cb, bool immedate)
{
    boost::recursive_mutex::scoped_lock lc(write_lock_);
    //std::lock_guard<std::mutex> lc(write_mutex_);
    write_size_ = len;
    has_written_ = 0;
    writefini_cb_ = cb;
    write_buffer_ = buffer;
    write_done_ = false;
    
    trasporttcp_->enableWrite();
    writeTransport();
}

void Connection::writeTransport()
{
    boost::recursive_mutex::scoped_lock lc(write_lock_);
    //std::lock_guard<std::mutex> lc(write_mutex_);
    
    if (write_done_)
        return;
    
    uint32_t to_write = write_size_ - has_written_;
    std::cout << "write transport!" << std::endl;
    std::string str(reinterpret_cast<char*>(write_buffer_.get()), write_size_);
    std::cout <<  "[write]>>>>>>>>>" << str << std::endl;
    int32_t write_byte = trasporttcp_->write(write_buffer_.get() + has_written_, to_write);
    
    if (write_byte < 0) {
        //connect dropped
        return;
    }

    has_written_ += write_byte;
    if (has_written_ == write_size_) {
        write_size_ = 0;
        has_written_ = 0;
        write_done_ = true;
        if(writefini_cb_)
            writefini_cb_();
    }
}

void Connection::lengthRead(const ConnectionPtr& conn, boost::shared_array<uint8_t> readbuffer, uint32_t length)
{
    //assert(strlen(reinterpret_cast<char *>(readbuffer.get())) >= 2);
    std::cout << "length read" << std::endl;
    uint8_t type = readbuffer[0];
    uint8_t len = readbuffer[1];

    //TODO:是否需要判断大小？
    if(type == CREATE_APPENDER)
        read(len, logCreateAppendHandler_);
    else if (type == NORMAL_WRITE)
        read(len, logExecHandler_);
    else if (type == 1)
        read(len, logOptionSetHandler_);
}

}