//
//  buffers.cpp
//  v8
//
//  Created by jie on 13-8-20.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include "arraybufferview.h"

const char* NodeBufferView::BUFFER = "buffer";

NodeBufferView::NodeBufferView() : mBuffer(0), mByteOffset(0), mByteLength(0) {
}
NodeBufferView::~NodeBufferView() {
}
char* NodeBufferView::value_ptr() {
    return mBuffer->mData + mByteOffset;
}
long NodeBufferView::writeBytes(long byteOffset, char* bytes, long byteLength) {
    return mBuffer->writeBytes(mByteOffset + byteOffset, bytes, byteLength);
}
long NodeBufferView::readBytes(long byteOffset, char* dest, long byteLength) {
    return mBuffer->readBytes(mByteOffset + byteOffset, dest, byteLength);
}
