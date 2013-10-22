//
//  Image.h
//  v8
//
//  Created by jie on 13-9-1.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__Image__
#define __v8__Image__

#include "../gl_include.h"
#include "../core/ClassBase.h"
#include "CCImage.h"
#include "CCTexture2D.h"
#include <v8.h>

class Image : public ClassBase {
public:
    Image();
    virtual ~Image();

    virtual void init(const v8::FunctionCallbackInfo<v8::Value> &args);
    virtual void doRelease();
    static class_struct* getExportStruct();
    virtual ClassType getClassType();
    virtual void* getData();
    virtual int getWidth();
    virtual int getHeight();
    virtual unsigned int getInternalFormat();
    virtual unsigned int getFormat();
    virtual unsigned int getType();

    node::CCImage* mImage;
    node::CCTexture2D* mTexture;
};

#endif /* defined(__v8__Image__) */
