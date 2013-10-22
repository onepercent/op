//
//  NodeBuffer.h
//  v8
//
//  Created by jie on 13-8-20.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__NodeBuffer__
#define __v8__NodeBuffer__

#include "../global.h"
#include "../classes/classenum.h"
#include "../core/bytebuffer.h"
#include "../core/ClassBase.h"
#include "../core/sturctures.h"

#include <v8.h>

using namespace v8;

/**
 * this underlying buffer of TypedBuffer
 */
class NodeBuffer : public ClassBase {
public:
	NodeBuffer();
	NodeBuffer(int length);

	virtual ~NodeBuffer();
    virtual void init(const v8::FunctionCallbackInfo<Value> &args);
    virtual void reset(const v8::FunctionCallbackInfo<Value> &args);
    virtual void allocate(long length);

    virtual ClassType getClassType();
    static class_struct* getExportStruct();
    virtual void getUnderlying(ByteBuffer* feature);
    static void onClone(Local<Object> jsCurrent, NodeBuffer& current, Local<Object> jsFrom, const NodeBuffer& from);

    template<typename T>
    void _write(long offset, int eSize, T value);
    template<typename T>
    T _read(long offset, int eSize);

    template<typename T>
    long _writeDatas(long offset, int eSize, T* value, int length);
    template<typename T>
    long _readDatas(long offset, int eSize, T* dest, int length);

    static bool isView(ClassType type);

    /**
     * write byte into current buffer
     * @return bytes written
     */
    virtual long writeBytes(long offset, char* bytes, long length);
    virtual long readBytes(long offset, char* dest, long length);
    
    virtual char* value_ptr(long offset);

    long mLength;
    char* mData;
};

template<typename T>
void NodeBuffer::_write(long offset, int eSize, T value) {
    if(offset < 0 || offset + eSize > mLength) {
        LOGE("ArrayBuffer write excceed from:%ld write:%d length:%ld", offset, eSize, mLength);
    }
    *((T*)(mData + offset)) = value;
}
template<typename T>
T NodeBuffer::_read(long offset, int eSize) {
    if(offset < 0 || offset + eSize > mLength) {
        LOGE("ArrayBuffer read excceed from:%ld write:%d length:%ld", offset, eSize, mLength);
    }
    return *((T*)(mData + offset));
}
template<typename T>
long NodeBuffer::_writeDatas(long offset, int eSize, T* value, int length) {
    long written = writeBytes(offset, (char*)(value), length * eSize);
    return written / eSize;
}
template<typename T>
long NodeBuffer::_readDatas(long offset, int eSize, T* dest, int length) {
    long readen = readBytes(offset, (char*)(dest), length * eSize);
    return readen / eSize;
}

#endif /* defined(__v8__NodeBuffer__) */
