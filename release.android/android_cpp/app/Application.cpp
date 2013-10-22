//
//  Application.cpp
//  v8
//
//  Created by jie on 13-8-4.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "node.h"
#include "Application.h"
#include "../core/ClassWrap.h"
#include "../core/Module.h"
#include "../core/v8Utils.h"
#include "../classes/file.h"

#include "../modules/modules.h"
#include "../modules/CCImage.h"
#include "../global.h"

#include <string>
#include "../gl_include.h"
#include <glm/gtc/type_ptr.hpp>
#include "../autorelease/ReleaseImpl.h"

using namespace node;
bool Application::debug = false;

Isolate* node_isolate = NULL;
int spritecount = 0;

#define ENTER_ISOLATE \
Locker locker(node_isolate);\
Isolate::Scope iso_scope(node_isolate)

#define EXIT_ISOLATE v8::Unlocker unlocker(node_isolate)
#define HANDLE_SCOPE HandleScope scope(node_isolate)

#define CONTEXT_SCOPE \
Local<Context> context = GetV8Context();\
Context::Scope context_scope(context)

#define SAFE_DELETE(p) if(p!=NULL){delete p;}
#define SAFE_DISPOSE(p) p.Dispose()

static void ExceptionToString(v8::TryCatch* try_catch) {
	v8::HandleScope handle_scope;
	v8::String::Utf8Value exception(try_catch->Exception());
	v8::Handle<v8::Message> message = try_catch->Message();
	if (message.IsEmpty()) {
		LOGE("%s\n", *exception);
	} else {
		v8::String::Utf8Value filename(message->GetScriptResourceName());
		int linenum = message->GetLineNumber();
		int colnum = message->GetStartColumn();
		LOGE("%s:%i:%i %s\n", *filename, linenum, colnum, *exception);
		v8::String::Utf8Value sourceline(message->GetSourceLine());
		LOGE("%s\n", *sourceline);
	}
}

void ReportMessage(v8::Handle<v8::Message> message, v8::Handle<v8::Value> data) {
    v8::String::Utf8Value filename(message->GetScriptResourceName());
    int lineno = message->GetLineNumber();
    v8::String::Utf8Value sourceline(message->GetSourceLine());

    char s[512];
    int len = sprintf(s, "file:%s, line:%d->%s", *filename, lineno, *sourceline);
    std::string exp(s, len);
    LOGI("Exception %s", exp.c_str());
}

Application::Application() {
    mWidth = 0;
    mHeight = 0;

    //v8::V8::AddMessageListener(ReportMessage);
	node_isolate = Isolate::New();
	ENTER_ISOLATE;

	game = 0;
	render = 0;
    touchEvent = 0;
    keyEvent = 0;

	HANDLE_SCOPE;
	context_p.Reset(node_isolate, Context::New(node_isolate));
}
Application::~Application() {
	{
		ENTER_ISOLATE;

		SAFE_DISPOSE(process_p);
		SAFE_DISPOSE(context_p);

		SAFE_DELETE(render);
		SAFE_DELETE(game);
		SAFE_DELETE(touchEvent);
		SAFE_DELETE(keyEvent);

		release_buildin_module();
	}
	node_isolate->Dispose();
	node_isolate = 0;
}
Local<Context> Application::GetV8Context() {
	return Local<Context>::New(node_isolate, context_p);
}

v8::Handle<v8::String> ReadFile(const char* name) {
	HANDLE_SCOPE;

	JSFile* file = JSFile::loadAsset(name);
    if(file->isEmpty()) {
        delete file;
        return scope.Close(String::New(""));
    }

	v8::Handle<v8::String> result = v8::String::New(file->chars(), file->size());
    delete file;
	return scope.Close(result);
}

static void printf__(const v8::FunctionCallbackInfo<Value>& args) {
	std::string buf;
	int length = args.Length();
	if (length == 0) {
		return;
	}
	buf.append(*String::Utf8Value(args[0]->ToString()));
	for (int i = 1; i < length; i++) {
		buf.append(",");
		buf.append(*String::Utf8Value(args[i]->ToString()));
	}
	LOGI("%s", buf.c_str());
}

Local<Function> Application::loadModuleFn(const char* name) {
//	LOGI("loadModuleFn 01");
	HANDLE_SCOPE;
//	LOGI("loadModuleFn 02");

	JSFile* file = JSFile::loadAsset(name);
	if (file->isEmpty()) {
		LOGE("error, file not found:%s\n", name);
	}

	std::string sc("(function (exports, require, module, __filename) {try{\n");
	if (!file->isEmpty()) {
		sc.append(file->chars(), file->size());
	}
	sc.append("\n}catch(e){console.error('Exception occur:");
    sc.append(name);
    sc.append(" ['+e+']');}})");
//	LOGI("loadModuleFn %d", file->size());

    delete file;

	v8::Handle<v8::String> source = String::New(sc.c_str());
//	LOGI("loadModuleFn 03");
	v8::TryCatch try_catch;
	Local<Script> comp = Script::Compile(source);
	if (comp.IsEmpty()) {
		LOGE("comp.IsEmpty()");
//		ReportException(try_catch);
	}
	Local<Function> fun = Local<Function>::Cast(comp->Run());
//	LOGI("loadModuleFn 04");
//	LOGE("before return");
//	ExceptionToString(&try_catch);
	return scope.Close(fun);
}

void Application::Binding(const v8::FunctionCallbackInfo<Value>& args) {
	HANDLE_SCOPE;

	Local<String> module = args[0]->ToString();
	String::Utf8Value module_v(module);
	node::node_module_struct* modp;

	Handle<Function> func;
	//LOGI("=============>>> Binding %s", *module_v);
	if ((modp = get_builtin_module(*module_v)) != NULL) { // c++ 实现的模块
		//LOGI("==>01");
		func = FunctionTemplate::New(modp->register_func)->GetFunction();
	} else {
		//LOGI("==>02");
		func = loadModuleFn(*module_v);
		spritecount++;
	}
	//LOGI("=============<<< %s", *module_v);
	args.GetReturnValue().Set(func);
}
Local<Script> Application::loadScript(const char* path) {
	HANDLE_SCOPE;

	v8::Handle<v8::String> source = ReadFile(path);
	return scope.Close(Script::Compile(source));
}

Handle<Object> Application::SetupProcessObject() {
	HANDLE_SCOPE;

	Local<FunctionTemplate> t = FunctionTemplate::New();
	t->SetClassName(String::New("process"));

	NODE_SET_PROTOTYPE_METHOD(t, "binding", Binding);

	Local<Object> process = t->GetFunction()->NewInstance();
	return scope.Close(process);
}

void Application::init() {
	{
		ENTER_ISOLATE;
		HANDLE_SCOPE;
		CONTEXT_SCOPE;

        // binding test func
		context->Global()->Set(String::New("print"), FunctionTemplate::New(printf__)->GetFunction());
		Handle<Object> process = SetupProcessObject();
		process_p.Reset(node_isolate, process);

		// init with node.js
		Local<Script> initscript = loadScript("node.js");
		Local<Value> f_value = initscript->Run();
		Local<Function> f = Local<Function>::Cast(f_value);

		// init global
		Handle<Value> arg = process;
		f->Call(context->Global(), 1, &arg);

        // bind event
        Handle<Object> eventExports = eval("require('core/event.js')")->ToObject();
        touchEvent = new TouchEvent(eventExports->Get(String::New("touchEvent"))->ToObject());
        keyEvent = new TouchEvent(eventExports->Get(String::New("keyEvent"))->ToObject());

		// load game module
		Handle<Value> gameExports = eval("require('game.js')");
		game = new JSObject(gameExports->ToObject());
		render = new JSObject(game->getAttribute<Object>("render"));
    }
}
// ==========================
// Events
// ==========================
void Application::appendMouseTouch(int button, int state, int x, int y) {
    touchEvent->appendMouseTouch(button, state, x, mHeight - y);
}
void Application::appendMouseMove(int x, int y) {
    touchEvent->appendMouseMove(x, mHeight - y);
}
void Application::appendKeyPress(unsigned char key, int x, int y) {
    keyEvent->appendKeyPress(key, x, mHeight - y);
}

// ==========================
// Life Cycle
// ==========================
void Application::destroy() {
    ReleaseTask::glAvaiable = false;
	{
		ENTER_ISOLATE;
		EXIT_ISOLATE;
	}
	ENTER_ISOLATE;
	HANDLE_SCOPE;
	CONTEXT_SCOPE;

	while (!v8::V8::IdleNotification());
}
void Application::pause() {
	if(game == 0) {
		return;
	}
	ENTER_ISOLATE;
	HANDLE_SCOPE;
	CONTEXT_SCOPE;

	game->callFunction("pause");
	LOGI("Application.pause");
}
void Application::resume() {
	if(game == 0) {
		return;
	}
	ENTER_ISOLATE;
	HANDLE_SCOPE;
	CONTEXT_SCOPE;

	game->callFunction("resume");
	LOGI("Application.resume");
}
void Application::gc() {
	ENTER_ISOLATE;
	HANDLE_SCOPE;
	CONTEXT_SCOPE;

	while (!v8::V8::IdleNotification());
}
void Application::evalScript(const char* sprite) {
	LOGI("evalScript:%s", sprite);
	ENTER_ISOLATE;
	HANDLE_SCOPE;
	CONTEXT_SCOPE;

    v8::Handle<v8::String> source = String::New(sprite);
	Local<Script> comp = Script::Compile(source);
    Local<Value> result = comp->Run();

	LOGI("%s", *String::Utf8Value(result->ToString()));
}
Handle<Value> Application::eval(const char* script) {
//	LOGI("Application::eval %s", script);
	HANDLE_SCOPE;
	CONTEXT_SCOPE;

	v8::Handle<v8::String> source = String::New(script);
	Local<Script> comp = Script::Compile(source);
	return scope.Close(comp->Run());
}
void Application::onSurfaceCreated(int width, int height) {
    ReleaseTask::glAvaiable = true;
    mWidth = width;
    mHeight = height;

	ENTER_ISOLATE;
	HANDLE_SCOPE;
	CONTEXT_SCOPE;

    Handle<Value> argv[2];
    argv[0] = Number::New(width);
    argv[1] = Number::New(height);
	render->callFunction("onSurfaceCreated", 2, argv);
	LOGI("Application::onSurfaceCreated");
}
void Application::onSurfaceChanged(int width, int height) {
    mWidth = width;
    mHeight = height;

	ENTER_ISOLATE;
	HANDLE_SCOPE;
	CONTEXT_SCOPE;

	Handle<Value> argv[2];
	argv[0] = Number::New(width);
	argv[1] = Number::New(height);
	render->callFunction("onSurfaceChanged",2, argv);
	LOGI("Application::onSurfaceChanged");
}
void Application::onDrawFrame() {
	ENTER_ISOLATE;
	HANDLE_SCOPE;
	CONTEXT_SCOPE;

	static const char* name = "onDrawFrame";
	render->callFunction(name);
	//LOGI("onDrawFrame");
}
