//
//  FeatureArray.cpp
//  v8
//
//  Created by jie on 13-8-22.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "array.h"

using namespace v8;
using namespace globalfn::array;

void globalfn::array::length(Local<String> property, const PropertyCallbackInfo<Value>& info) {
    HandleScope scope;
    ClassBase* c = internalPtr<ClassBase>(info);
    if(c == 0) {
        return;
    }
    ByteBuffer buf;
    c->getUnderlying(&buf);
    info.GetReturnValue().Set(buf.typedLength());
}
