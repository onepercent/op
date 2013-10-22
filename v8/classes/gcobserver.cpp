//
//  gcobserver.cpp
//  v8
//
//  Created by jie on 13-9-10.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "gcobserver.h"
#include "../global.h"

GcObserver::GcObserver() : mMark(false) {
}
GcObserver::~GcObserver() {
    if(mMark) {
        LOGI("~gc %s", tag.c_str());
    }
}
void GcObserver::init(const v8::FunctionCallbackInfo<Value> &args) {
    if(args.Length() > 0) {
        mMark = true;
        tag = std::string(*String::Utf8Value(args[0]->ToString()));
    }
    if(mMark) {
        LOGI("gc init %s", tag.c_str());
    }
}
class_struct* GcObserver::getExportStruct() {
    static class_struct mTemplate = {
        0, "gcobserver", CLASS_GC
    };
    return &mTemplate;
}
ClassType GcObserver::getClassType() {
    return getExportStruct()->mType;
}
