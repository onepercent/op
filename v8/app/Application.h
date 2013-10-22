//
//  Application.h
//  v8
//
//  Created by jie on 13-8-4.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__Application__
#define __v8__Application__

#include <iostream>
#include <v8.h>
#include "node.h"
#include "../core/JSObject.h"
#include "../modules/Event.h"

using namespace v8;

class Application {
public:
    Persistent<Object> process_p;
    Persistent<Context> context_p;

    Application();
    ~Application();

    void init();
    void pause();
    void resume();
    void destroy();
    void gc();

    void onSurfaceCreated(int width, int height);
    void onSurfaceChanged(int width, int height);
    void onDrawFrame();
    void evalScript(const char* sprite);

    static char* source_root;
    static bool debug;

    void appendMouseTouch(int button, int state, int x, int y);
    void appendMouseMove(int x, int y);
    void appendKeyPress(unsigned char key, int x, int y);

private:
    /**
     * 加载一个 function
     */
    static void Binding(const FunctionCallbackInfo<Value>& args);
    static Local<Function> loadModuleFn(const char* name);

    JSObject* game;
    JSObject* render;
    TouchEvent* touchEvent;
    TouchEvent* keyEvent;
    
    int mWidth;
    int mHeight;

    Handle<Value> eval(const char* script);

    Local<Context> GetV8Context();
    Handle<Object> SetupProcessObject();
    Local<Script> loadScript(const char* path);
};

#endif /* defined(__v8__Application__) */
