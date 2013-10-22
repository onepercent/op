//
//  AutoRelease.cpp
//  v8
//
//  Created by jie on 13-9-27.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include "AutoRelease.h"
#include "../core/v8Utils.h"

/**
 * @text
 */
METHOD_BEGIN(values, info) {
    HandleScope scope;
    AutoRelease* font = internalPtr<AutoRelease>(info, CLASS_AutoRelease);
    if(font == 0) {
        return;
    }
    font->values(info);
}
static v8::Local<v8::Function> initClass(v8::Handle<v8::FunctionTemplate>& temp) {
    HandleScope scope;

    Local<ObjectTemplate> obj = temp->PrototypeTemplate();
    
    EXPOSE_METHOD(obj, values, ReadOnly | DontDelete);
//    EXPOSE_METHOD(obj, measure, ReadOnly | DontDelete);
//    EXPOSE_METHOD(obj, glyphs, ReadOnly | DontDelete);
//    EXPOSE_METHOD(obj, outline_type, ReadOnly | DontDelete);
//    EXPOSE_METHOD(obj, outline_thickness, ReadOnly | DontDelete);
    
    return scope.Close(temp->GetFunction());
}

AutoRelease::AutoRelease() : mTask(0) {
}
AutoRelease::~AutoRelease() {
    release();
    if(mTask != 0) {
        delete mTask;
    }
}
void AutoRelease::values(const v8::FunctionCallbackInfo<Value>& param) {
    if(mTask != 0) {
        mTask->init(param);
    }
}
void AutoRelease::doRelease() {
    if(mTask != 0) {
        mTask->release();
    }
}
void AutoRelease::init(const v8::FunctionCallbackInfo<Value> &args) {
    mTask = ReleaseTask::createTask(args[0]->Uint32Value());
    if(mTask != 0) {
        mRelease = false;
    }
}

class_struct* AutoRelease::getExportStruct() {
    static class_struct mTemplate = {
        initClass, "autorelease", CLASS_AutoRelease
    };
    return &mTemplate;
}
ClassType AutoRelease::getClassType() {
    return getExportStruct()->mType;
}
