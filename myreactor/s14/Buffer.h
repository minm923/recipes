#ifndef MUDUO_NET_BUFFER_H
#define MUDUO_NET_BUFFER_H

#include "datetime/copyable.h"
#include <vector>
#include <assert.h>
#include <algorithm>
#include <string>

namespace muduo
{


class Buffer : public muduo::copyable
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize  = 1024;

    Buffer()
      : buffer_(kInitialSize),
        readerIndex_(kCheapPrepend),
        writerIndex_(kCheapPrepend)
    {

    }

    // default copy-ctor, dtor and assignment are fine
    
    void swap(Buffer& rhs)
    {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_, rhs.readerIndex_);
        std::swap(writerIndex_, rhs.writerIndex_);
    }

    void readFd(int fd, int* savedErrno);


    size_t writableBytes()
    {
        return buffer_.size() - writerIndex_;        
    }

    size_t readableBytes()
    {
        return writerIndex_ - readerIndex_;
    }

    size_t prependableBytes()
    {
        return readerIndex_;
    }

    const char* peek()
    {
        return begin() + readerIndex_;
    }

    void retrieve(size_t len)
    {
        assert(len <= readableBytes());        
        readerIndex_ += len;
    }

    void retrieveUntil(const char* end)
    {
        assert(peek() <= end);       
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    std::string retrieveAsString()    
    {
        std::string str(peek(), readableBytes());
        retrieveAll();
        return str;
    }

    char* beginWrite()
    {
        return begin() + writerIndex_;
    }

    const char* beginWrite() const 
    {
        return begin() + writerIndex_;
    }


    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);                        
        std::copy(data, data+len, beginWrite());
        hasWritten(len);        
    }

private:    
    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    void makeSpace(size_t len)        
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)        
        {
            buffer_.resize(writerIndex_ + len);
        }
        else
        {
            assert(kCheapPrepend <= readerIndex_);
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                      begin() + writerIndex_,
                      begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = kCheapPrepend + readable;
            assert(readable == readableBytes());
        }
    }

    char* begin()
    {
        return &*buffer_.begin();
    }

private:    
    std::vector<char> buffer_;    
    size_t readerIndex_;
    size_t writerIndex_;
};

}


#endif
