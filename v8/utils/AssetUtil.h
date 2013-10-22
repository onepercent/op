/*
 * AssetUtil.h
 *
 *  Created on: 2013-7-19
 *      Author: jie
 */

#ifndef ASSETUTIL_H_
#define ASSETUTIL_H_

#include <v8.h>
#include "../classes/file.h"

class JSFile;

class AssetUtil {
public:
	AssetUtil();
	virtual ~AssetUtil();

    /**
     * load asset to file
     */
	static void load(JSFile* file, const char* path);
    static void getFilePath(const char* path, std::string* wholepath);
};

#endif /* ASSETUTIL_H_ */
