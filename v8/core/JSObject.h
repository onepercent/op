//
//  JSObject.h
//  v8
//
//  Created by jie on 13-8-5.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__JSObject__
#define __v8__JSObject__

#include <iostream>
#include <v8.h>
#include "../app/node.h"
#include "../global.h"

using namespace v8;

class JSObject {
public:
    JSObject(Handle<Object> obj);
    ~JSObject();
    
    template <class T>
    inline Local<T> getAttribute(const char* name) {
        HandleScope scope(node_isolate);

        Local<Object> obj = Local<Object>::New(node_isolate, object_);
        Local<Value> value = obj->Get(String::New(name));
        if(value.IsEmpty() || value->IsUndefined()) {
			LOGI("attribute not found:%s", name);
            return Local<T>();
        }
        return scope.Close(Local<T>::Cast(value));
    }

    inline Local<Value> callFunction(const char* name, int size, Handle<Value>* argv) {
        HandleScope scope;

        Local<Function> fn = getAttribute<Function>(name);
        return scope.Close(fn->Call(Local<Object>::New(node_isolate, object_), size, argv));
    }

    inline Local<Value> callFunction(const char* name) {
        HandleScope scope(node_isolate);
        
        Local<Function> fn = getAttribute<Function>(name);
        Handle<Value> argv;
        Local<Value> fuRet = fn->Call(Local<Object>::New(node_isolate, object_), 0, &argv);
        return scope.Close(fuRet);
    }

protected:
    Persistent<Object> object_;
};

#endif /* defined(__v8__JSObject__) */
