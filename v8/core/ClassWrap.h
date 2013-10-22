
/*
 * JSWrap.h
 *
 *  Created on: 2013-8-11
 *      Author: jie
 */

#ifndef JSWRAP_H_
#define JSWRAP_H_

#include <v8.h>
#include "../app/node.h"
#include "v8Utils.h"
#include "sturctures.h"
#include "../global.h"

using namespace v8;

/**
 * wrap cpp object to js class
 */
template<class T>
class ClassWrap {
public:
	static Persistent<Function> _function;// create function
    static bool mInit;

    /**
     * constructor
     */
	static void constructorCallback(const v8::FunctionCallbackInfo<Value> &args) {
		HandleScope scope(node_isolate);
		if (!args.IsConstructCall()) {
			return;
		}

		T* instance = new T();
		instance->init(args);

		Persistent<Object> ret(node_isolate, args.This());
		ret.MakeWeak(instance, unrefCallback);

		args.This()->SetInternalField(0, External::New(instance));
	}

    /**
     * release will unbind native resource
     */
    static void release(const v8::FunctionCallbackInfo<Value>& info) {
		HandleScope scope(node_isolate);
        ClassBase* t = internalPtr<ClassBase>(info);
    	//LOGI("ClassWrap.release 01:%p", t);
        if(t != 0) {
//        	int type = t->getExportStruct()->mType;
//        	LOGI("ClassWrap.release %d", type);
            t->release();
        }
    }
    static void toString(const v8::FunctionCallbackInfo<Value>& info) {
		HandleScope scope(node_isolate);
        ClassBase* t = internalPtr<ClassBase>(info);
        if(t != 0) {
            info.GetReturnValue().Set(String::New(t->toString()));
        }
    }
    /**
     * when js release the last refer of this object
     */
	static void unrefCallback(Isolate* isolate, Persistent<Object>* value, T* parameter) {
        bool nearDeath = value->IsNearDeath();
        parameter->releasePersistent();
		value->Dispose();// dispose first
        value->Clear();
	}

	static void initFunction() {
		HandleScope scope(node_isolate);

        class_struct* clz = T::getExportStruct();
		Handle<FunctionTemplate> fn = FunctionTemplate::New(constructorCallback);

		fn->SetClassName(String::New(clz->mClassName));

        Local<ObjectTemplate> fnproto = fn->PrototypeTemplate();
        EXPOSE_METHOD(fnproto, release, ReadOnly | DontDelete);
        EXPOSE_METHOD(fnproto, toString, ReadOnly | DontDelete);
        fnproto->SetInternalFieldCount(1);

        Local<ObjectTemplate> fninst = fn->InstanceTemplate();
		fninst->SetInternalFieldCount(1);

        if(clz->initFn !=0) {
            Local<Function> exportFn = clz->initFn(fn);
            _function.Reset(node_isolate, exportFn);
        } else {
            _function.Reset(node_isolate, fn->GetFunction());
        }
	}

    /**
     * create a copy of current js object
     */
    static void clone(const v8::FunctionCallbackInfo<Value>& info) {
        HandleScope scope(node_isolate);
        
        T* t1 = internalPtr<T>(info, T::getExportStruct()->mType);
        if(t1 == 0) {
            return;
        }
        
        if(info.Length() == 0) {
            // create a copy on current object
            Local<Object> other = Local<Function>::New(node_isolate, _function)->NewInstance();
            T* t2 = internalPtr<T>(other);
            
            T::onClone(other, *t2, info.This(), *t1);
            info.GetReturnValue().Set(other);
        } else {
            // copy from an existing object
            if(info[0].IsEmpty() || !info[0]->IsObject()) {
                return;
            }
            T* t2 = internalArg<T>(info[0], T::getExportStruct()->mType);
            if(t2 == 0) {
                return;
            }
            
            T::onClone(info.This(), *t1, info[0]->ToObject(), *t2);
        }
    }

	static Local<Function> getFunction() {
        if(!mInit) {
            initFunction();
            mInit = true;
        }
		return Local<Function>::New(node_isolate, _function);
	}
    static void expose(Local<Object> env) {
        class_struct* clz = T::getExportStruct();
        env->Set(String::New(clz->mClassName), getFunction());
    }
    static void dispose() {
        if(mInit) {
            _function.Dispose(node_isolate);
            mInit = false;
        }
    }
    /**
     * expose using a given class name
     */
    static void expose(const char* clzname, Local<Object> env) {
        env->Set(String::New(clzname), getFunction());
    }
    static Handle<Object> newInstance() {
		HandleScope scope(node_isolate);
        return scope.Close(getFunction()->NewInstance());
    }
    static Handle<Object> newInstance(int argc, Handle<Value>* argv) {
		HandleScope scope(node_isolate);
        return scope.Close(getFunction()->NewInstance(argc, argv));
    }
    static Handle<Object> newInstance(Handle<Value> arg) {
		HandleScope scope(node_isolate);
        Handle<Value> argv[] = {arg};
        return scope.Close(getFunction()->NewInstance(1, argv));
    }
};

template<typename T>
Persistent<Function> ClassWrap<T>::_function;

template<typename T>
bool ClassWrap<T>::mInit = false;

#define NEW_INSTANCE(name, type, ...) Handle<Object> name;\
{\
    Handle<Object> __##name = ClassWrap<type>::newInstance();\
    type* self = internalPtr<type>(__##name);\
    self->init(__VA_ARGS__);\
    name = __##name;\
}

#endif /* JSWRAP_H_ */
