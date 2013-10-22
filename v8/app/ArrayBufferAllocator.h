//
//  ArrayBufferAllocator.h
//  v8
//
//  Created by jie on 13-8-20.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__ArrayBufferAllocator__
#define __v8__ArrayBufferAllocator__

//#include <v8.h>
//
//class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
//public:
//    // Impose an upper limit to avoid out of memory errors that bring down the process.
//    static const size_t kMaxLength = 0x3fffffff;
//    static ArrayBufferAllocator the_singleton;
//    virtual ~ArrayBufferAllocator() {}
//    virtual void* Allocate(size_t length);
//    virtual void Free(void* data);
//private:
//    ArrayBufferAllocator() {}
//    ArrayBufferAllocator(const ArrayBufferAllocator&);
//    void operator=(const ArrayBufferAllocator&);
//};

#endif /* defined(__v8__ArrayBufferAllocator__) */
