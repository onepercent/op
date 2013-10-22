//
//  ArrayBufferAllocator.cpp
//  v8
//
//  Created by jie on 13-8-20.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "ArrayBufferAllocator.h"

//ArrayBufferAllocator ArrayBufferAllocator::the_singleton;
//
//void* ArrayBufferAllocator::Allocate(size_t length) {
//    if (length > kMaxLength)
//        return NULL;
//    char* c = new char[length];
//    printf("ArrayBufferAllocator %p\n", c);
//    return c;
//}
//void ArrayBufferAllocator::Free(void* data) {
//    printf("ArrayBufferAllocator release %p\n", data);
//    delete[] static_cast<char*>(data);
//}
