/*
 * AssetUtil.cpp
 *
 *  Created on: 2013-7-19
 *      Author: jie
 */

#include "AssetUtil.h"
#include "../global.h"
#include <string>
#include <errno.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "../classes/file.h"

using namespace v8;

AAssetManager* AssetUtil::mgr = 0;

AssetUtil::AssetUtil() {
}
AssetUtil::~AssetUtil() {
}

void AssetUtil::load(JSFile* tofile, const char* path) {
    FILE* file = android_fopen(path, "rb");
    if (file == NULL) {
    	LOGE("AssetUtil::load notfound:%s", path);
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* chars = tofile->allocate(size);
    int i = 0;
    while (i < size) {
        int read = fread(&chars[i], 1, size - i, file);
        i += read;
    }

    fclose(file);
}

static int android_read(void* cookie, char* buf, int size) {
	return AAsset_read((AAsset*) cookie, buf, size);
}
static int android_write(void* cookie, const char* buf, int size) {
	return EACCES; // can't provide write access to the apk
}
static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
	return AAsset_seek((AAsset*) cookie, offset, whence);
}
static int android_close(void* cookie) {
	AAsset_close((AAsset*)cookie);
	return 0;
}

FILE* AssetUtil::android_fopen(const char* fname, const char* mode) {
	std::string pathstr;
	pathstr.append(fname);
	if (mode[0] == 'w') {
		return NULL;
	}

//	AAsset* asset = AAssetManager_open(AssetUtil::mgr, fname, AASSET_MODE_UNKNOWN);
	AAsset* asset = AAssetManager_open(AssetUtil::mgr, fname, AASSET_MODE_STREAMING);
//	AAsset* asset = AAssetManager_open(AssetUtil::mgr, fname, AASSET_MODE_RANDOM);
//	AAsset* asset = AAssetManager_open(AssetUtil::mgr, fname, AASSET_MODE_BUFFER);
	if (!asset) {
		return NULL;
	}
	return funopen(asset, android_read, android_write, android_seek, android_close);
}
