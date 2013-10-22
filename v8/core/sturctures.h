//
//  sturctures.h
//  v8
//
//  Created by jie on 13-8-15.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef v8_sturctures_h
#define v8_sturctures_h

#include <v8.h>
#include "../classes/classenum.h"

typedef v8::Local<v8::Function> (*export_fn)(v8::Handle<v8::FunctionTemplate>& temp);

struct class_struct {
    export_fn initFn;

    const char* mClassName;
    ClassType mType;
};

#endif
