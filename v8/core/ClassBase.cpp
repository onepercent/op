//
//  ClassBase.cpp
//  v8
//
//  Created by jie on 13-8-15.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "ClassBase.h"
#include "v8Utils.h"
#include "bytebuffer.h"

ClassBase::ClassBase() : mRelease(true) {
}
ClassBase::~ClassBase() {
}

void ClassBase::release() {
    if(!mRelease) {
        doRelease();
        mRelease = true;
    }
}
void ClassBase::doRelease() {
}
void ClassBase::init(const v8::FunctionCallbackInfo<Value> &args) {
}
const char* ClassBase::toString() {
    return "[object native]";
}

class_struct* ClassBase::getExportStruct() {
    return 0;
}
ClassType ClassBase::getClassType() {
    return CLASS_NULL;
}
bool ClassBase::isReleased() {
    return mRelease;
}
void ClassBase::getUnderlying(ByteBuffer *feature) {
}
void ClassBase::releasePersistent() {
    release();
    delete this;
}
