//
//  v8Utils.h
//  v8
//
//  Created by jie on 13-8-14.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef v8_v8Utils_h
#define v8_v8Utils_h

#include "ClassBase.h"

#define EXPOSE_PROPERTY(obj, name, attribute) obj->Set(String::New(#name), FunctionTemplate::New(name), PropertyAttribute(attribute))
#define EXPOSE_METHOD(obj, name, attribute) obj->Set(String::New(#name), FunctionTemplate::New(name), PropertyAttribute(attribute))
#define EXPOSE_METHOD_NAME(obj, mname, name, attribute) obj->Set(String::New(#mname), FunctionTemplate::New(name), PropertyAttribute(attribute))
#define EXPOSE_TEMPLATE_METHOD(obj, name, attribute) obj->Set(String::New(#name), FunctionTemplate::New(name<T>), PropertyAttribute(attribute))
#define METHOD_BEGIN(name, param) static void name(const v8::FunctionCallbackInfo<Value>& param)
#define NS_METHOD_BEGIN(name, param) void name(const v8::FunctionCallbackInfo<Value>& param)
#define INS_METHOD_BEGIN(T, name, param) void T::name(const v8::FunctionCallbackInfo<Value>& param)

#define V_2F(index) info[index]->NumberValue()

namespace classtype {
    template <typename T>
    ClassType getClassType();
    
    template <typename T>
    static T unwrap(Local<Value> arg);// unwrap v8::Object to raw
    
    template <typename T>
    static Local<Value> wrap(T t);// unwrap raw data to v8::Value
    
    bool isTheSameType(ClassType t1, ClassType t2);
}

// mast be called with a HandleScope
template<typename T>
static T* internalPtr(const v8::FunctionCallbackInfo<Value>& info) {
    Local<Object> self = info.This();
    if(self.IsEmpty()) {
        return 0;
    }
    if(self->InternalFieldCount() == 1) {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        return static_cast<T*>(wrap->Value());
    }
    return 0;
}
template<typename T>
static T* internalPtr(const v8::FunctionCallbackInfo<Value>& info, ClassType type) {
    Local<Object> self = info.This();
    if(self.IsEmpty()) {
        return 0;
    }
    if(self->InternalFieldCount() == 1) {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        ClassBase* base = static_cast<ClassBase*>(wrap->Value());
        if(base->getClassType() == type) {
            return static_cast<T*>(base);
        }
    }
    return 0;
}


template<typename T>
static T* internalPtr(const PropertyCallbackInfo<Value>& info) {
    Local<Object> self = info.This();
    if(self.IsEmpty()) {
        return 0;
    }
    if(self->InternalFieldCount() == 1) {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        return static_cast<T*>(wrap->Value());
    }
    return 0;
}
template<typename T>
static T* internalPtr(const PropertyCallbackInfo<Value>& info, ClassType type) {
    Local<Object> self = info.This();
    if(self.IsEmpty()) {
        return 0;
    }
    if(self->InternalFieldCount() == 1) {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        ClassBase* base = static_cast<ClassBase*>(wrap->Value());
        if(base->getClassType() == type) {
            return static_cast<T*>(base);
        }
    }
    return 0;
}


template<typename T>
static T* internalPtr(Handle<Object>& self) {
    if(self.IsEmpty()) {
        return 0;
    }
    if(self->InternalFieldCount() == 1) {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        return static_cast<T*>(wrap->Value());
    }
    return 0;
}
template<typename T>
static T* internalPtr(Handle<Object>& self, ClassType type) {
    if(self.IsEmpty()) {
        return 0;
    }
    if(self->InternalFieldCount() == 1) {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        ClassBase* base = static_cast<ClassBase*>(wrap->Value());
        if(base->getClassType() == type) {
            return static_cast<T*>(base);
        }
    }
    return 0;
}


template<typename T>
static T* internalArg(Local<Value> val) {
    Local<Object> self = val->ToObject();
    if(self.IsEmpty()) {
        return 0;
    }
    if(self->InternalFieldCount() == 1) {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        return static_cast<T*>(wrap->Value());
    }
    return 0;
}

template<typename T>
static T* internalArg(Local<Value> val, ClassType type) {
    Local<Object> self = val->ToObject();
    if(self.IsEmpty()) {
        return 0;
    }
    if(self->InternalFieldCount() == 1) {
        Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
        ClassBase* base = static_cast<ClassBase*>(wrap->Value());
        if(base->getClassType() == type) {
            return static_cast<T*>(base);
        }
    }
    return 0;
}

template <typename T>
static int populateValues(T* dest, Handle<Array>& array, int left=0) {
    int initial = left;
    for(int i = 0, len = array->Length(); i < len; i++) {
        *(dest++) = classtype::unwrap<T>(array->Get(i));
        if(left-- == 0) {
            break;
        }
    }
    return initial == 0 ? array->Length() : initial - left;
}

template <typename T, typename S>
static void convertValues(T* dest, S* source, int length) {
    for (int i=0; i<length; i++) {
        *(dest++) = (T)(*source++);
    }
}

template <typename T>
static void populateValues(T* dest, char* source, ClassType type, int length) {
    switch (type) {
        case CLASS_ArrayBuffer:
        case CLASS_Int8Array:
            convertValues<T, int8_t>(dest, (int8_t*)source, length);
            break;
        case CLASS_Uint8Array:
            convertValues<T, uint8_t>(dest, (uint8_t*)source, length);
            break;
        case CLASS_Int16Array:
            convertValues<T, int16_t>(dest, (int16_t*)source, length);
            break;
        case CLASS_Uint16Array:
            convertValues<T, uint16_t>(dest, (uint16_t*)source, length);
            break;
        case CLASS_Int32Array:
            convertValues<T, int32_t>(dest, (int32_t*)source, length);
            break;
        case CLASS_Uint32Array:
            convertValues<T, uint32_t>(dest, (uint32_t*)source, length);
            break;
        case CLASS_Float32Array:
            convertValues<T, float>(dest, (float*)source, length);
            break;
        case CLASS_Float64Array:
            convertValues<T, double>(dest, (double*)source, length);
            break;
        default:
            ThrowException(String::New("invalide TypedBuffer type %d"));
            break;
    }
}

#define JS_WRAP_UNWRAP(T, getter, setter) \
template<> T unwrap<T>(Local<Value> arg) {\
    return arg->getter();\
}\
template<> Local<Value> wrap<T>(T t) {\
    return setter(t);\
}

namespace classtype {
	JS_WRAP_UNWRAP(int8_t, Int32Value, Integer::New)
	JS_WRAP_UNWRAP(uint8_t, Uint32Value, Integer::NewFromUnsigned)
	JS_WRAP_UNWRAP(int16_t, Int32Value, Integer::New)
	JS_WRAP_UNWRAP(uint16_t, Uint32Value, Integer::NewFromUnsigned)
	JS_WRAP_UNWRAP(int32_t, Int32Value, Integer::New)
	JS_WRAP_UNWRAP(uint32_t, Uint32Value, Integer::NewFromUnsigned)
	JS_WRAP_UNWRAP(float, NumberValue, Number::New)
	JS_WRAP_UNWRAP(double, NumberValue, Number::New)
}

#endif
