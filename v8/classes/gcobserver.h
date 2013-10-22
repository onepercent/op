//
//  gcobserver.h
//  v8
//
//  Created by jie on 13-9-10.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__gcobserver__
#define __v8__gcobserver__

#include "../core/ClassBase.h"
#include <string>

class GcObserver : public ClassBase {
public:
    GcObserver();
    virtual ~GcObserver();
    virtual void init(const v8::FunctionCallbackInfo<Value> &args);

    static class_struct* getExportStruct();
    virtual ClassType getClassType();

private:
    std::string tag;
    bool mMark;
};

#endif /* defined(__v8__gcobserver__) */
