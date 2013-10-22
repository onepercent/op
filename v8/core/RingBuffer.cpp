//
//  RingBuffer.cpp
//  v8
//
//  Created by jie on 13-9-10.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include "RingBuffer.h"

RingBuffer::RingBuffer(int stride, int count) {
    mStride = stride;
    mCount = count + 1;

    mBuffer = new char[mStride * mCount];
    mReadRange = new DataRange(this, 'r');
    mWriteRange = new DataRange(this, 'w');
    clear();
}
RingBuffer::~RingBuffer() {
    delete[] mBuffer;
    delete mReadRange;
    delete mWriteRange;
}
void RingBuffer::read(char* dest, int index) {
    memcpy(dest, value_ptr(index), mStride);
}
void RingBuffer::write(char* src, int index) {
    memcpy(value_ptr(index), src, mStride);
}
int RingBuffer::stride() {
    return mStride;
}

/**
 * mRead -> mWrite
 * mRead -> mWrite + mCount
 */
DataRange* RingBuffer::startRead() {
    mReadRange->start(mRead, mWrite < mRead ? mWrite + mCount : mWrite);
    return mReadRange;
}
/**
 * mWrite -> mRead - 1
 * mWrite -> mRead - 1 + mCount
 */
DataRange* RingBuffer::startWrite() {
    mWriteRange->start(mWrite, mRead <= mWrite ? mRead - 1 + mCount : mRead - 1);
    return mWriteRange;
}
void RingBuffer::endRange(DataRange* range) {
    int cursor = range->mStart;
    cursor = cursor >= mCount ? cursor - mCount : cursor;
    if(range->mType == 'r') {
        mRead = cursor;
    } else if(range->mType == 'w') {
        mWrite = cursor;
    }
}
void RingBuffer::clear() {
    mReadRange->clear();
    mWriteRange->clear();
    mRead = mWrite = 0;
}
char* RingBuffer::value_ptr(int index) {
    return mBuffer + (index < mCount ? index : index - mCount) * mStride;
}

// ====================================
// DataRange
// ====================================
DataRange::DataRange(RingBuffer* eStruct, char type) {
    mStructor = eStruct;
    mType = type;
}
DataRange::~DataRange() {
}

void DataRange::start(int start, int end) {
    mStart = start;
    mEnd = end;
}
int DataRange::readOne(char* dest) {
    if(isEmpty()) {
        return -1;
    }
    mStructor->read(dest, mStart++);
    return mEnd - mStart;
}
int DataRange::writeOne(char* src) {
    if(isEmpty()) {
        return -1;
    }
    mStructor->write(src, mStart++);
    return mEnd - mStart;
}
int DataRange::next() {
    return mEnd - ++mStart;
}
void DataRange::end() {
    mStructor->endRange(this);
}
void DataRange::clear() {
    mStart = mEnd = 0;
}
bool DataRange::isEmpty() {
    return mStart == mEnd;
}
int DataRange::remain() {
    return mEnd - mStart;
}
int DataRange::read(char* dest, int length) {
    int r = remain();
    if(r == 0) {
        return -1;
    }

    int stride = mStructor->stride();
    int max = length / stride;
    max = (max > r) ? r : max;
    for(int i = 0; i < max; i++) {
        mStructor->read(dest, mStart++);
        dest += stride;
    }
    return max;
}
int DataRange::write(char* dest, int length) {
    int r = remain();
    if(r == 0) {
        return -1;
    }

    int stride = mStructor->stride();
    int max = length / stride;
    max = (max > r) ? r : max;
    for(int i = 0; i < max; i++) {
        mStructor->write(dest, mStart++);
        dest += stride;
    }
    return max;
}


