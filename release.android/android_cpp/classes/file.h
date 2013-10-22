//
//  file.h
//  v8
//
//  Created by jie on 13-8-15.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__file__
#define __v8__file__

#include "../core/ClassBase.h"
#include <stdio.h>

class JSFile;

class FileRefer {
public:
	size_t pos;
	size_t size;
	char *buffer;
};

class JSFile: public ClassBase {
public:
	JSFile();
	~JSFile();

	bool isEmpty();
	char* allocate(int length);
	const char* chars();
	int size();
	virtual void doRelease();

	static JSFile* loadAsset(const char* path);
	static class_struct* getExportStruct();
	virtual ClassType getClassType();

	FILE* fopen(const char *mode);

private:
	char* mBuffer;
	int mLength;
};

#endif /* defined(__v8__file__) */
