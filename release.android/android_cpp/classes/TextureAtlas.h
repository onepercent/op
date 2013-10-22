//
//  TextureAtlas.h
//  v8
//
//  Created by jie on 13-9-24.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__TextureAtlas__
#define __v8__TextureAtlas__

#include "../core/ClassBase.h"
#include <freetype-gl.h>

class TextureAtlas : public ClassBase {
public:
	TextureAtlas();
	virtual ~TextureAtlas();

    virtual void doRelease();
    virtual void init(const v8::FunctionCallbackInfo<Value> &args);

    static class_struct* getExportStruct();
    virtual ClassType getClassType();

public:
    texture_atlas_t *atlas;
};

#endif /* defined(__v8__TextureAtlas__) */
