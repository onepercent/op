//
//  Console.cpp
//  v8
//
//  Created by jie on 13-8-4.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "Console.h"
#include "../global.h"

using namespace v8;
static void appendContent(const v8::FunctionCallbackInfo<Value>& args, std::string& buf) {
    int length = args.Length();
    if(length == 0) {
        return;
    }
    buf.append(*String::Utf8Value(args[0]->ToString()));
    for (int i = 1; i < length; i++) {
        buf.append(",");
        buf.append(*String::Utf8Value(args[i]->ToString()));
    }
}
static void console_log(const v8::FunctionCallbackInfo<Value>& args) {
    std::string buf;
    appendContent(args, buf);
    LOGI("%s", buf.c_str());
}
static void console_error(const v8::FunctionCallbackInfo<Value>& args) {
    std::string buf;
    appendContent(args, buf);
    LOGE("%s", buf.c_str());
}

static void init(const v8::FunctionCallbackInfo<Value>& args) {
    HandleScope scope;
    Local<Object> target = args[0]->ToObject();

    NODE_SET_METHOD(target, "log", console_log);
    NODE_SET_METHOD(target, "error", console_error);
}

node::node_module_struct* Console::getModule(node::node_module_struct* t) {
	t = Module::getModule(t);
	t->filename = __FILE__;
	t->modname = "node_console";
	t->register_func = init;
	return t;
}
