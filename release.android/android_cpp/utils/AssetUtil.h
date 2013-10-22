/*
 * AssetUtil.h
 *
 *  Created on: 2013-7-19
 *      Author: jie
 */

#ifndef ASSETUTIL_H_
#define ASSETUTIL_H_

#include <v8.h>

class JSFile;
class AAssetManager;

class AssetUtil {
public:
	AssetUtil();
	virtual ~AssetUtil();

    /**
     * load asset to file
     */
	static void load(JSFile* file, const char* path);
	static FILE* android_fopen(const char* fname, const char* mode);

public:
	static AAssetManager* mgr;
};

#endif /* ASSETUTIL_H_ */
