/*
 * AssetUtil.cpp
 *
 *  Created on: 2013-7-19
 *      Author: jie
 */

#include "AssetUtil.h"
#include <string>
#include "../global.h"

using namespace v8;

AssetUtil::AssetUtil() {
}
AssetUtil::~AssetUtil() {
}

void AssetUtil::load(JSFile* tofile, const char* path) {
    std::string abspath(source_root);
    abspath.append(path);
    
    FILE* file = fopen(abspath.c_str(), "rb");
    if (file == NULL) {
        return;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* chars = tofile->allocate(size);
    chars[size] = 0;
    for (int i = 0; i < size;)
    {
        int read = fread(&chars[i], 1, size - i, file);
        i += read;
    }
    fclose(file);
}
void AssetUtil::getFilePath(const char* path, std::string* wholepath) {
    wholepath->append(source_root);
    wholepath->append(path);
}