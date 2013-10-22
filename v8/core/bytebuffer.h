//
//  bytebuffer.h
//  v8
//
//  Created by jie on 13-8-22.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__bytebuffer__
#define __v8__bytebuffer__

#include "../classes/classenum.h"
#include <stdarg.h>

class NodeBuffer;
class NodeBufferView;

/**
 * used as a bridge when sequence/unsequence of classes
 */
class ByteBuffer {
public:
    char* mPtr;
    long mByteOffset;
    long mByteLength;
    int mElementSize;
    ClassType mElement;
    bool mRealse;

    ByteBuffer();
    virtual ~ByteBuffer();
    void allocate(int byteLength);

    void init(char* ptr, long length, ClassType type);
    void init(NodeBufferView* view, ClassType type);
    void init(NodeBufferView* view, ClassType type, int elementSize);

    void init(NodeBuffer* buf);
    void init(NodeBuffer* buf, ClassType type);
    void init(NodeBuffer* buf, long bOffset, long bLength, ClassType type);
    void init(NodeBuffer* buf, long bOffset, long bLength, ClassType type, int typeUnit);
    
    static int getTypeSize(ClassType type);

    template<typename T>
    T* value_ptr(int index=0);
    template<typename T>
    void set_value(int index, T value);
    template<typename T>
    void set_value(int index, T* values, int count);

    char* value_ptr(long offset=0);
    long length();
    int typedLength();
};

template<typename T>
T* ByteBuffer::value_ptr(int index) {
    return (T*)(mPtr + mByteOffset) + index;
}
template<typename T>
void ByteBuffer::set_value(int index, T value) {
    *((T*)(mPtr + mByteOffset) + index) = value;
}
template<typename T>
void ByteBuffer::set_value(int index, T* values, int count) {
    memcpy(value_ptr<T>(index), values, count * sizeof(T));
}

#endif /* defined(__v8__bytebuffer__) */
