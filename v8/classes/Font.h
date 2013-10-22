//
//  Font.h
//  v8
//
//  Created by jie on 13-9-24.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__Font__
#define __v8__Font__

#include "../core/ClassBase.h"
#include "freetype-gl.h"

class Font : public ClassBase {
public:
	Font();
	virtual ~Font();

    virtual void doRelease();
    virtual void init(const FunctionCallbackInfo<Value> &args);

    static class_struct* getExportStruct();
    virtual ClassType getClassType();

public:
    texture_font_t *font;
};


#endif /* defined(__v8__Font__) */
