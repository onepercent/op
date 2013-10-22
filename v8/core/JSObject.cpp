//
//  JSObject.cpp
//  v8
//
//  Created by jie on 13-8-5.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "JSObject.h"

JSObject::JSObject(Handle<Object> obj){
    object_.Reset(node_isolate, obj);
}
JSObject::~JSObject() {
    object_.Dispose();
    object_.Clear();
}
