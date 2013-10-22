//
//  file.cpp
//  v8
//
//  Created by jie on 13-8-15.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include "file.h"
#include "../utils/AssetUtil.h"
#include "../core/v8Utils.h"

bool JSFile::isEmpty() {
    return mRelease;
}
char* JSFile::allocate(int length) {
    mRelease = false;
    mLength = length;
    mBuffer = new char[length];
    return mBuffer;
}
JSFile::JSFile() : mBuffer(0), mLength(0) {
}
JSFile::~JSFile() {
    release();
}
const char* JSFile::chars() {
	return this->mBuffer;
}
int JSFile::size() {
	return this->mLength;
}

JSFile* JSFile::loadAsset(const char* path) {
    JSFile* file = new JSFile();
    AssetUtil::load(file, path);
    return file;
}

METHOD_BEGIN(loadAsset, info) {
    HandleScope scope;

    JSFile* file = internalPtr<JSFile>(info);
    file->release();
    AssetUtil::load(file, *String::Utf8Value(info[0]->ToString()));
}
METHOD_BEGIN(getContent, info) {
    HandleScope scope;

    JSFile* file = internalPtr<JSFile>(info);
    if(file->isReleased()) {
        info.GetReturnValue().Set(String::New(""));
    } else {
        info.GetReturnValue().Set(String::New(file->chars(), file->size()));
    }
}

static v8::Local<v8::Function> initClass(v8::Handle<v8::FunctionTemplate>& temp) {
    HandleScope scope;

    Local<ObjectTemplate> obj = temp->PrototypeTemplate();
    EXPOSE_METHOD(obj, loadAsset, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, getContent, ReadOnly | DontDelete);

    return scope.Close(temp->GetFunction());
}

class_struct* JSFile::getExportStruct() {
    static class_struct mTemplate = {
        initClass, "file", CLASS_FILE
    };
    return &mTemplate;
}
ClassType JSFile::getClassType() {
    return getExportStruct()->mType;
}
void JSFile::doRelease() {
    delete[] mBuffer;
}

