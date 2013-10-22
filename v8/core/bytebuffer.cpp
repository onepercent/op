//
//  bytebuffer.cpp
//  v8
//
//  Created by jie on 13-8-22.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "bytebuffer.h"
#include "../typedbuffer/arraybufferview.h"

ByteBuffer::ByteBuffer() : mElement(CLASS_ArrayBuffer), mPtr(0), mByteOffset(0), mByteLength(0), mElementSize(1), mRealse(false) {
}
ByteBuffer::~ByteBuffer() {
    if(mRealse) {
        delete[] mPtr;
    }
}
void ByteBuffer::allocate(int byteLength) {
    mPtr = new char[byteLength];
    mByteLength = byteLength;
    mRealse = true;
}
void ByteBuffer::init(char* ptr, long length, ClassType type) {
    this->mPtr = ptr;
    this->mByteLength = length;
    this->mElement = type;
}
void ByteBuffer::init(NodeBufferView* view, ClassType type) {
    init(view->mBuffer, view->mByteOffset, view->mByteLength, type);
}
void ByteBuffer::init(NodeBufferView* view, ClassType type, int elementSize) {
    init(view->mBuffer, view->mByteOffset, view->mByteLength, type, elementSize);
}

void ByteBuffer::init(NodeBuffer* buf) {
    this->mPtr = buf->mData;
    this->mByteLength = buf->mLength;
}
void ByteBuffer::init(NodeBuffer* buf, ClassType type) {
    this->mPtr = buf->mData;
    this->mByteLength = buf->mLength;
    this->mElement = type;
    this->mElementSize = getTypeSize(type);
}
void ByteBuffer::init(NodeBuffer* buf, long bOffset, long bLength, ClassType type) {
    this->mPtr = buf->mData;
    this->mByteOffset = bOffset;
    this->mByteLength = bLength;
    this->mElement = type;
    this->mElementSize = getTypeSize(type);
}
void ByteBuffer::init(NodeBuffer* buf, long bOffset, long bLength, ClassType type, int typeUnit) {
    this->mPtr = buf->mData;
    this->mByteOffset = bOffset;
    this->mByteLength = bLength;
    this->mElement = type;
    this->mElementSize = typeUnit;
}

int ByteBuffer::getTypeSize(ClassType type) {
    switch (type) {
        case CLASS_Int8Array:
        case CLASS_Uint8Array:
            return 1;
        case CLASS_Int16Array:
        case CLASS_Uint16Array:
            return 2;
        case CLASS_Int32Array:
        case CLASS_Uint32Array:
        case CLASS_Float32Array:
            return 4;
        case CLASS_Float64Array:
            return 8;
        default:
            return 1;
    }
}

char* ByteBuffer::value_ptr(long offset) {
    return mPtr + mByteOffset + offset;
}
long ByteBuffer::length() {
    return mByteLength;
}
int ByteBuffer::typedLength() {
    return (int)(mByteLength / mElementSize);
}
