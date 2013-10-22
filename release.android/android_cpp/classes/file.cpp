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
#include <errno.h>

static int readfn(void *handler, char *buf, int size) {
	FileRefer *mem = static_cast<FileRefer*>(handler);
	size_t available = mem->size - mem->pos;

	if (size > available) {
		size = available;
	}
	memcpy(buf, mem->buffer + mem->pos, sizeof(char) * size);
	mem->pos += size;

	return size;
}
static int writefn(void *handler, const char *buf, int size) {
	FileRefer *mem = static_cast<FileRefer*>(handler);
	size_t available = mem->size - mem->pos;

	if (size > available) {
		size = available;
	}
	memcpy(mem->buffer + mem->pos, buf, sizeof(char) * size);
	mem->pos += size;

	return size;
}
static fpos_t seekfn(void *handler, fpos_t offset, int whence) {
	size_t pos;
	FileRefer *mem = static_cast<FileRefer*>(handler);

	switch (whence) {
	case SEEK_SET:
		pos = offset;
		break;
	case SEEK_CUR:
		pos = mem->pos + offset;
		break;
	case SEEK_END:
		pos = mem->size + offset;
		break;
	default:
		return -1;
	}

	if (pos > mem->size) {
		return -1;
	}

	mem->pos = pos;
	return (fpos_t) pos;
}
static int closefn(void *handler) {
	FileRefer* mem = static_cast<FileRefer*>(handler);
	delete mem;
	return 0;
}

// ==========================
// JSFile
// ==========================
bool JSFile::isEmpty() {
	return mRelease;
}
char* JSFile::allocate(int length) {
	mRelease = false;
	mLength = length;
	mBuffer = new char[length + 1];
	mBuffer[length] = 0;

	return mBuffer;
}
JSFile::JSFile() :
		mBuffer(0), mLength(0) {
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
	if (file->isReleased()) {
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
	static class_struct mTemplate = { initClass, "file", CLASS_FILE };
	return &mTemplate;
}
ClassType JSFile::getClassType() {
	return getExportStruct()->mType;
}
void JSFile::doRelease() {
	//LOGI("JSFile::doRelease %p", mBuffer);
	delete[] mBuffer;
}
FILE* JSFile::fopen(const char *mode) {
	FileRefer* mem = new FileRefer();
	mem->pos = 0;
	mem->size = mLength;
	mem->buffer = mBuffer;

	// funopen's man page: https://developer.apple.com/library/mac/#documentation/Darwin/Reference/ManPages/man3/funopen.3.html
	return funopen(mem, readfn, writefn, seekfn, closefn);
}

