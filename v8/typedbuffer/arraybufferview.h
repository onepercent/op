//
//  buffers.h
//  v8
//
//  Created by jie on 13-8-20.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__buffers__
#define __v8__buffers__

#include "arraybuffer.h"
#include "../core/ClassBase.h"

class NodeBufferView : public ClassBase {
public:
    NodeBufferView();
    virtual ~NodeBufferView();
    virtual char* value_ptr();

    /**
     * handle the offset for you
     */
    virtual long writeBytes(long byteOffset, char* bytes, long byteLength);
    virtual long readBytes(long byteOffset, char* dest, long byteLength);

    virtual void initWithArray(Handle<Array>& array, int offset)=0;
    virtual void toArray(Handle<Array>& array, int offset)=0;

    NodeBuffer* mBuffer;
    long mByteOffset;
    long mByteLength;

    static const char* BUFFER;
};

#endif /* defined(__v8__buffers__) */
