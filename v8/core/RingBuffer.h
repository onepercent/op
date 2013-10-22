//
//  eventstructor.h
//  v8
//
//  Created by jie on 13-9-10.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__eventstructor__
#define __v8__eventstructor__

#include "../core/ClassBase.h"

class DataRange;

class RingBuffer {
public:
    /**
     * @param stride element size in bytes
     * @param count element count
     */
    RingBuffer(int stride, int count);
    virtual ~RingBuffer();
    virtual void read(char* dest, int index);
    virtual void write(char* src, int index);

    virtual DataRange* startRead();
    virtual DataRange* startWrite();
    virtual void endRange(DataRange* range);
    /**
     * reset all bytes, make it maxmum writable
     */
    virtual void clear();
    char* value_ptr(int index);
    int stride();

private:
    int mRead;
    int mWrite;
    int mCount;
    int mStride;
    char* mBuffer;

    DataRange* mReadRange;
    DataRange* mWriteRange;
};

/**
 * hold a tempary read/write range
 */
class DataRange {
public:
    DataRange(RingBuffer* eStruct, char type);
    virtual ~DataRange();

    int mStart;
    int mEnd;
    char mType;
    RingBuffer* mStructor;
    virtual void clear();

    template <typename T>
    T* value_ptr() {
        if(mStart == mEnd) {
            return 0;
        }
        return (T*)(mStructor->value_ptr(mStart));
    }

    virtual bool isEmpty();
    /**
     * init range
     */
    virtual void start(int start, int end);
    /**
     * read a structor unit and return structors remain
     */
    virtual int readOne(char* dest);
    /**
     * write a structor unit and return empty slot remain
     */
    virtual int writeOne(char* dest);
    /**
     * @param dest dest byte pointer
     * @param length max byte avaiable in dest
     * return element readed
     */
    virtual int read(char* dest, int length);
    virtual int write(char* dest, int length);
    /**
     * finish read and tell the underlying EventStructor how much data was readed or written
     */
    virtual void end();
    /**
     * increase the cursor
     * return avaiable slot remain
     */
    virtual int next();
    virtual int remain();
};

#endif /* defined(__v8__eventstructor__) */
