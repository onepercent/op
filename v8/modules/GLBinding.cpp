//
//  GLBinding.cpp
//  jstest
//
//  Created by jie on 13-7-25.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include "GLBinding.h"
#include "../gl_include.h"
#include "../global.h"
#include "../core/ClassBase.h"
#include "../core/v8Utils.h"
#include "../core/bytebuffer.h"
#include "Image.h"

using namespace v8;

static void checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}

#define JS_METHOD(name) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& args)
#define JS_STR(...) v8::String::New(__VA_ARGS__)
#define JS_INT(val) v8::Integer::New(val)
#define JS_UINT(val) v8::Integer::NewFromUnsigned(val)
#define JS_FLOAT(val) v8::Number::New(val)
#define JS_BOOL(val) v8::Boolean::New(val)
#define UNUSED_PARAM(variable) (void)variable

static void argValue(Local<Value> &info, float* slot) {
    *slot = info->NumberValue();
}
static void argValue(Local<Value> &info, uint8_t* slot) {
    *slot = info->BooleanValue();
}
static void argValue(Local<Value> &info, int32_t* slot) {
    *slot = info->Int32Value();
}

static void getInternal(Local<Value> infoV, ByteBuffer* buffer) {
    if(infoV.IsEmpty() || infoV->IsUndefined()) {
        LOGE("getInternal failed, value is Empty");
        return;
    }
    ClassBase* p = internalArg<ClassBase>(infoV);
    p->getUnderlying(buffer);
}

static GLuint ToGLuint(const void* ptr) {
	return static_cast<GLuint>(reinterpret_cast<size_t>(ptr));
}
Handle<Integer> getIntParameter(GLenum pname)
{
    GLint value = 0;
    glGetIntegerv(pname, &value);
    return v8::Int32::New(value);
}
Handle<Integer> getUnsignedIntParameter(GLenum pname)
{
    GLint value = 0;
    glGetIntegerv(pname, &value);
    return v8::Uint32::New(value);
}
Handle<Boolean> getBooleanParameter(GLenum pname)
{
	GLboolean value = 0;
	glGetBooleanv(pname, &value);
    return value == 0 ? v8::False() : v8::True();
}
Handle<Value> getBooleanArrayParameter(GLenum pname)
{
    if (pname != GL_COLOR_WRITEMASK) {
//        notImplemented();
//        return WebGLGetInfo(0, 0);
    	return v8::Undefined();
    }
    GLboolean value[4] = {0};
    glGetBooleanv(pname, value);
    Local<Array> boolValue = Array::New(4);
    for (int ii = 0; ii < 4; ++ii) {
    	boolValue->Set(Boolean::New(ii), value[ii] == GL_FALSE ? v8::False() : v8::True());
    }
    return boolValue;
}
Handle<Number> getFloatParameter(GLenum pname)
{
    GLfloat value = 0;
    glGetFloatv(pname, &value);
    return v8::Number::New(value);
}
Handle<String> getStringParameter(GLenum pname)
{
    const GLubyte* value  = glGetString(pname);
    return v8::String::New((const char*)value);
}
Handle<Value> getWebGLFloatArrayParameter(GLenum pname)
{
	GLfloat value[4] = {0};
	glGetFloatv(pname, value);
    unsigned length = 0;
    switch (pname) {
//		case GL_ALIASED_POINT_SIZE_RANGE:
		case GL_ALIASED_LINE_WIDTH_RANGE:
		case GL_DEPTH_RANGE:
			length = 2;
			break;
		case GL_BLEND_COLOR:
		case GL_COLOR_CLEAR_VALUE:
			length = 4;
			break;
		default:
			return v8::Undefined();
//			notImplemented();
    }
    Local<v8::ArrayBuffer> arrays = v8::ArrayBuffer::New(value, 8 * length);
    return v8::Float32Array::New(arrays, 0, length);
}
Handle<Value> getWebGLIntArrayParameter(GLenum pname)
{
    GLint value[4] = {0};
    glGetIntegerv(pname, value);
    unsigned length = 0;
    switch (pname) {
		case GL_MAX_VIEWPORT_DIMS:
			length = 2;
			break;
		case GL_SCISSOR_BOX:
		case GL_VIEWPORT:
			length = 4;
			break;
		default:
			return v8::Undefined();
//			notImplemented();
    }
    Local<v8::ArrayBuffer> arrays = v8::ArrayBuffer::New(value, 8 * length);
    return v8::Int32Array::New(arrays, 0, length);
}

static void getArgPtr(ByteBuffer* dest, ClassType ftype, const Local<Value>& arg) {
    ClassBase* wrap = internalArg<ClassBase>(arg->ToObject());
    if(wrap == 0) {
        LOGE("arguments is not subclass of ClassBase");
        return;
    }
    wrap->getUnderlying(dest);
    if(dest->mElement != ftype) {
        LOGI("arguments underlying datatype not match. %d", ftype);
    }
}

#define ARGS_NAME args

#define CHECK_L(len) ARGS_NAME.Length() != len
#define CHECK_GLchar(a) !a->IsInt32()
// char* string value
#define CHECK_GLcharP(a) !a->IsStringObject()
#define CHECK_GLenum(a) !a->IsUint32()
// typedef unsigned
#define CHECK_GLenumP(a) !a->IsUint16Array()
#define CHECK_GLboolean(a) !a->IsBoolean()
#define CHECK_GLbooleanP(a) !a->IsUint8Array()
#define CHECK_GLbyte(a) !a->IsInt32()
#define CHECK_GLbyteP(a) !a->IsInt8Array()
#define CHECK_GLshort(a) !a->IsInt32()
#define CHECK_GLshortP(a) !a->IsInt16Array()
#define CHECK_GLint(a) !a->IsInt32()
#define CHECK_GLintP(a) !a->IsInt32Array()
#define CHECK_GLsizei(a) !a->IsInt32()
#define CHECK_GLsizeiP(a) !a->IsInt32Array()
#define CHECK_GLubyte(a) !a->IsUint32()
#define CHECK_GLubyteP(a) !a->IsUint8Array()
#define CHECK_GLushort(a) !a->IsUint32()
#define CHECK_GLushortP(a) !a->IsUint16Array()
#define CHECK_GLuint(a) !a->IsUint32()
#define CHECK_GLuintP(a) !a->IsUint32Array()
#define CHECK_GLfloat(a) !a->IsNumber()
#define CHECK_GLfloatP(a) !a->IsFloat32Array()
// float GLclampf
#define CHECK_GLclampf(a) !a->IsNumber()
// long GLsizeiptr
#define CHECK_GLsizeiptr(a) !a->IsNumber()
#define CHECK_GLvoidP(a) !a->IsTypedArray()
#define CHECK_GLintptr(a) !a->IsNumber()
#define CHECK_GLbitfield(a) !a->IsUint32()

// should not present
#define ARGS_GLchar(a) !!!
#define ARGS_GLenum(a) a->Uint32Value()
#define ARGS_GLboolean(a) a->BooleanValue()
#define ARGS_GLbyte(a) a->Int32Value()
#define ARGS_GLshort(a) a->Int32Value()
#define ARGS_GLint(a) a->Int32Value()
#define ARGS_GLsizei(a) a->Int32Value()
#define ARGS_GLubyte(a) a->Uint32Value()
#define ARGS_GLushort(a) a->Uint32Value()
#define ARGS_GLuint(a) a->Uint32Value()
#define ARGS_GLfloat(a) a->NumberValue()
#define ARGS_GLclampf(a) a->NumberValue()
#define ARGS_GLsizeiptr(a) a->IntegerValue()
#define ARGS_GLintptr(a) a->IntegerValue()
#define ARGS_GLbitfield(a) a->Uint32Value()

// char* string value
#define ARGS_GLcharP(a) (GLchar*)*String::Utf8Value(a->ToObject())
#define ARGS_GLbooleanP(a) (GLbooleanP*)getArrayPtr(a->ToObject())
#define ARGS_GLenumP(a) (GLbooleanP*)getArrayPtr(a->ToObject())
#define ARGS_GLbyteP(a) (GLbooleanP*)getArrayPtr(a->ToObject())
#define ARGS_GLshortP(a) (GLbooleanP*)getArrayPtr(a->ToObject())
#define ARGS_GLintP(a) (GLint*)getArrayPtr(a->ToObject())
#define ARGS_GLsizeiP(a) (GLsizei*)getArrayPtr(a->ToObject())
#define ARGS_GLubyteP(a) (GLubyte*)getArrayPtr(a->ToObject())
#define ARGS_GLushortP(a) (GLushort*)getArrayPtr(a->ToObject())
#define ARGS_GLuintP(a) (GLuint*)getArrayPtr(a->ToObject())
#define ARGS_GLfloatP(a) (GLfloat*)getArrayPtr(a->ToObject())
#define ARGS_GLvoidP(a) (GLvoid*)getArrayPtr(a->ToObject())

// check arguments
#define CHECK_ARG_1(name, t1) \
if(ARGS_NAME.Length() != 1 || CHECK_##t1(ARGS_NAME[0])) {\
    args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
}
#define CHECK_ARG_2(name, t1, t2) \
if(ARGS_NAME.Length() != 2 || CHECK_##t1(ARGS_NAME[0]) || CHECK_##t2(ARGS_NAME[1])) {\
    args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
}
#define CHECK_ARG_3(name, t1, t2, t3) \
if(ARGS_NAME.Length() != 3 || CHECK_##t1(ARGS_NAME[0]) || CHECK_##t2(ARGS_NAME[1]) || CHECK_##t3(ARGS_NAME[2])) {\
    args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
}
#define CHECK_ARG_4(name, t1, t2, t3, t4) \
if(ARGS_NAME.Length() != 4 || CHECK_##t1(ARGS_NAME[0]) || CHECK_##t2(ARGS_NAME[1]) || CHECK_##t3(ARGS_NAME[2]) || CHECK_##t4(ARGS_NAME[3])) {\
    args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
}
#define CHECK_ARG_5(name, t1, t2, t3, t4, t5) \
if(ARGS_NAME.Length() != 4 || CHECK_##t1(ARGS_NAME[0]) || CHECK_##t2(ARGS_NAME[1]) || CHECK_##t3(ARGS_NAME[2]) || CHECK_##t4(ARGS_NAME[3]) || CHECK_##t5(ARGS_NAME[4])) {\
    args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
}
#define CHECK_ARG_8(name, t1, t2, t3, t4, t5, t6, t7, t8) \
if(ARGS_NAME.Length() != 4 || CHECK_##t1(ARGS_NAME[0]) || CHECK_##t2(ARGS_NAME[1]) || CHECK_##t3(ARGS_NAME[2]) || CHECK_##t4(ARGS_NAME[3]) || CHECK_##t5(ARGS_NAME[4]) || CHECK_##t6(ARGS_NAME[5]) || CHECK_##t7(ARGS_NAME[6]) || CHECK_##t8(ARGS_NAME[7])) {\
    args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
}
#define CHECK_ARG_9(name, t1, t2, t3, t4, t5, t6, t7, t8, t9) \
if(ARGS_NAME.Length() != 4 || CHECK_##t1(ARGS_NAME[0]) || CHECK_##t2(ARGS_NAME[1]) || CHECK_##t3(ARGS_NAME[2]) || CHECK_##t4(ARGS_NAME[3]) || CHECK_##t5(ARGS_NAME[4]) || CHECK_##t6(ARGS_NAME[5]) || CHECK_##t7(ARGS_NAME[6]) || CHECK_##t8(ARGS_NAME[7]) || CHECK_##t9(ARGS_NAME[8])) {\
    args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
}

// gl methods delegate
#define CALL_GL(glname) glname()
#define CALL_GL_1(glname, t1) glname(ARGS_##t1(ARGS_NAME[0]))
#define CALL_GL_2(glname, t1, t2) glname(ARGS_##t1(ARGS_NAME[0]), ARGS_##t2(ARGS_NAME[1]))
#define CALL_GL_3(glname, t1, t2, t3) glname(ARGS_##t1(ARGS_NAME[0]), ARGS_##t2(ARGS_NAME[1]), ARGS_##t3(ARGS_NAME[2]))
#define CALL_GL_4(glname, t1, t2, t3, t4) glname(ARGS_##t1(ARGS_NAME[0]), ARGS_##t2(ARGS_NAME[1]), ARGS_##t3(ARGS_NAME[2]), ARGS_##t4(ARGS_NAME[3]))
#define CALL_GL_5(glname, t1, t2, t3, t4, t5) glname(ARGS_##t1(ARGS_NAME[0]), ARGS_##t2(ARGS_NAME[1]), ARGS_##t3(ARGS_NAME[2]), ARGS_##t4(ARGS_NAME[3]), ARGS_##t5(ARGS_NAME[4]))
#define CALL_GL_8(glname, t1, t2, t3, t4, t5, t6, t7, t8) glname(ARGS_##t1(ARGS_NAME[0]), ARGS_##t2(ARGS_NAME[1]), ARGS_##t3(ARGS_NAME[2]), ARGS_##t4(ARGS_NAME[3]), ARGS_##t5(ARGS_NAME[4]), ARGS_##t6(ARGS_NAME[5]), ARGS_##t7(ARGS_NAME[6]), ARGS_##t8(ARGS_NAME[7]))
#define CALL_GL_9(glname, t1, t2, t3, t4, t5, t6, t7, t8, t9) glname(ARGS_##t1(ARGS_NAME[0]), ARGS_##t2(ARGS_NAME[1]), ARGS_##t3(ARGS_NAME[2]), ARGS_##t4(ARGS_NAME[3]), ARGS_##t5(ARGS_NAME[4]), ARGS_##t6(ARGS_NAME[5]), ARGS_##t7(ARGS_NAME[6]), ARGS_##t8(ARGS_NAME[7]), ARGS_##t9(ARGS_NAME[8]))

#define DELEGATE_TO_GL_N(name, glname) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
    HandleScope scope;\
    glname();\
}
#define DELEGATE_TO_GL_N1(name, glname, t1) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
    HandleScope scope;\
    CHECK_ARG_1(name, t1)\
    CALL_GL_1(glname, t1);\
}
#define DELEGATE_TO_GL_N2(name, glname, t1, t2) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
    HandleScope scope;\
    CHECK_ARG_2(name, t1, t2)\
    CALL_GL_2(glname, t1, t2);\
}
#define DELEGATE_TO_GL_N3(name, glname, t1, t2, t3) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
    HandleScope scope;\
    CHECK_ARG_3(name, t1, t2, t3)\
    CALL_GL_3(glname, t1, t2, t3);\
}
#define DELEGATE_TO_GL_N4(name, glname, t1, t2, t3, t4) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
    HandleScope scope;\
    CHECK_ARG_4(name, t1, t2, t3, t4)\
    CALL_GL_4(glname, t1, t2, t3, t4);\
}
#define DELEGATE_TO_GL_N5(name, glname, t1, t2, t3, t4, t5) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
    HandleScope scope;\
    CHECK_ARG_5(name, t1, t2, t3, t4, t5)\
    CALL_GL_5(glname, t1, t2, t3, t4, t5);\
}
#define DELEGATE_TO_GL_N8(name, glname, t1, t2, t3, t4, t5, t6, t7, t8) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
    HandleScope scope;\
    CHECK_ARG_8(name, t1, t2, t3, t4, t5, t6, t7, t8)\
    CALL_GL_8(glname, t1, t2, t3, t4, t5, t6, t7, t8);\
}
#define DELEGATE_TO_GL_N9(name, glname, t1, t2, t3, t4, t5, t6, t7, t8, t9) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
    HandleScope scope;\
    CHECK_ARG_9(name, t1, t2, t3, t4, t5, t6, t7, t8, t9)\
    CALL_GL_9(glname, t1, t2, t3, t4, t5, t6, t7, t8, t9);\
}

#define DELEGATE_TO_GL(glname) DELEGATE_TO_GL_N(glname, glname)
#define DELEGATE_TO_GL_1(glname, t1) DELEGATE_TO_GL_N1(glname, glname, t1)
#define DELEGATE_TO_GL_2(glname, t1, t2) DELEGATE_TO_GL_N2(glname, glname, t1, t2)
#define DELEGATE_TO_GL_3(glname, t1, t2, t3) DELEGATE_TO_GL_N3(glname, glname, t1, t2, t3)
#define DELEGATE_TO_GL_4(glname, t1, t2, t3, t4) DELEGATE_TO_GL_N4(glname, glname, t1, t2, t3, t4)
#define DELEGATE_TO_GL_5(glname, t1, t2, t3, t4, t5) DELEGATE_TO_GL_N5(glname, glname, t1, t2, t3, t4, t5)
#define DELEGATE_TO_GL_8(glname, t1, t2, t3, t4, t5, t6, t7, t8) DELEGATE_TO_GL_N8(glname, glname, t1, t2, t3, t4, t5, t6, t7, t8)
#define DELEGATE_TO_GL_9(glname, t1, t2, t3, t4, t5, t6, t7, t8, t9) DELEGATE_TO_GL_N9(glname, glname, t1, t2, t3, t4, t5, t6, t7, t8, t9)

#define DELEGATE_TO_GL_1R(name, glname, type, v8type) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
	HandleScope scope;\
	type value;\
    glname(1, &value);\
	args.GetReturnValue().Set(v8type::New(value));\
}
// 2 param and return boolean
#define DELEGATE_TO_GL_1_BR(name, glname, t1) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
	HandleScope scope;\
    CHECK_ARG_1(name, t1)\
    GLboolean value = CALL_GL_1(glname, t1);\
	args.GetReturnValue().Set(value == 0 ? v8::False() : v8::True());\
}
/**
 * call with length 1 array
 */
#define DELEGATE_TO_GL_1A(name, glname, type) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
	HandleScope scope;\
	CHECK_ARG_1(name, type)\
	type value = ARGS_##type(args[0]);\
    glname(1, &value);\
}
#define DELEGATE_TO_GL_R(name, glname, type, v8type) void GLBinding::name##Callback(const v8::FunctionCallbackInfo<Value>& ARGS_NAME) {\
	HandleScope scope;\
	type value = glname();\
	args.GetReturnValue().Set(v8type::New(value));\
}

#define _GLenum(name) (GLenum)name->Uint32Value()
#define _GLint(name) (GLint)name->Int32Value()
#define _GLuint(name) (GLuint)name->Uint32Value()
#define _GLsizei(name) (GLsizei)name->Int32Value()

#define BIND_GL_CONST(proto, name, value) proto->Set(String::New(#name), Integer::New(value), v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete))
#define BIND_GL(proto, name) proto->Set(String::New(#name), FunctionTemplate::New(&GLBinding::name##Callback)->GetFunction())

#define GL_GENARRY_IMPL(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 1 || !args[0]->IsNumber()) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name", Bad arguments")));\
    }\
\
    GLsizei size = args[0]->Int32Value();\
    GLuint* buffers = new GLuint[size];\
    name(size, buffers);\
\
    Handle<Array> arr = Array::New(size);\
    for (int i = 0; i < size; ++i) {\
        arr->Set(Integer::New(i), Integer::New(buffers[i]));\
    }\
    delete[] buffers;\
\
    args.GetReturnValue().Set(arr);\
}
#define GL_GENSINGLE_IMPL(show, name) \
Handle<Value> GLBinding::show##Callback(const Arguments& args) {\
    HandleScope scope;\
\
    GLuint o;\
    name(1, &o);\
\
    args.GetReturnValue().Set(Integer::New(o));\
}
#define GL_IMPL_GLenum(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 1 || !args[0]->IsUint32()) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
    }\
    name((GLenum) args[0]->Uint32Value());\
}
#define GL_IMPL_GLenum_GLuint(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 2) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
    }\
    name((GLenum) args[0]->Uint32Value(), (GLuint) args[1]->Uint32Value());\
}
#define GL_IMPL_GLenum_GLenum(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 2) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments 2")));\
    }\
    name((GLenum) args[0]->Uint32Value(), (GLenum) args[1]->Uint32Value());\
}
#define GL_IMPL_GLuint_GLfloatPtr(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 2 || !args[0]->IsUint32() || !args[1]->IsFloat32Array()) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
    }\
    name((GLuint) args[0]->Uint32Value(), (const GLfloat*) getArrayPtr(args[1]->ToObject()));\
}
#define GL_IMPL_ReturnGLboolean_GLuint(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 1) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments numbers")));\
    }\
    bool value = name((GLuint) args[0]->Uint32Value());\
    args.GetReturnValue().Set(v8::Boolean::New(value));\
}
#define GL_IMPL_GLuint_GLsizei_GLfloatPtr(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() == 2) {\
        if(!args[0]->IsInt32() || !args[1]->IsFloat32Array()) {\
            args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments 2")));\
        }\
        Local<Object> array = args[1]->ToObject();\
		name((GLuint)args[0]->Int32Value(), (GLsizei)ARR_LENGTH(array), (const GLfloat*)getArrayPtr(array));\
    } else if (args.Length() == 3) {\
        if(!args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsFloat32Array()) {\
            args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments 3")));\
        }\
        Local<Object> array = args[2]->ToObject();\
		name((GLuint)args[0]->Int32Value(), (GLsizei)args[1]->Int32Value(), (const GLfloat*)getArrayPtr(array));\
    } else {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments number")));\
    }\
}
#define GL_IMPL_GLuint_GLsizei_GLintPtr(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() == 2) {\
        if(!args[0]->IsInt32() || !args[1]->IsInt32Array()) {\
            args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments 2")));\
        }\
        Local<Object> array = args[1]->ToObject();\
		name((GLuint)args[0]->Int32Value(), (GLsizei)ARR_LENGTH(array), (const GLint*)getArrayPtr(array));\
    } else if (args.Length() == 3) {\
        if(!args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32Array()) {\
            args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments 3")));\
        }\
        Local<Object> array = args[2]->ToObject();\
		name((GLuint)args[0]->Int32Value(), (GLsizei)args[1]->Int32Value(), (const GLint*)getArrayPtr(array));\
    } else {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments number")));\
    }\
}
// GLsizei n, const GLuint* buffers
#define GL_IMPL_GLsizei_InGLuintPtr(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() == 1) {\
        if(!args[0]->IsInt32Array()) {\
            args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments 1")));\
        }\
        Local<Object> array = args[0]->ToObject();\
		name((GLsizei)ARR_LENGTH(array), (const GLuint*)getArrayPtr(array));\
    } else if (args.Length() == 2) {\
        if(!args[0]->IsInt32() || !args[1]->IsInt32Array()) {\
            args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments 2")));\
        }\
        Local<Object> array = args[1]->ToObject();\
		name((GLsizei)args[0]->Int32Value(), (const GLuint*)getArrayPtr(array));\
    } else {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments number")));\
    }\
}
#define GL_IMPL_GLuint(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 2 || !args[0]->IsUint32()) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments")));\
    }\
    name((GLuint) args[0]->Uint32Value());\
    return v8::Undefined();\
}
#define GL_IMPL_GLenum_4(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 4) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments, count not 4")));\
    }\
    name(_GLenum(args[0]), _GLenum(args[1]), _GLenum(args[2]), _GLenum(args[3]));\
    return v8::Undefined();\
}
#define GL_IMPL_GLenum2_GLintPtr(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 3) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments, count not 4")));\
    }\
    GLint value;\
    name(_GLenum(args[0]), _GLenum(args[1]),  &value);\
    args.GetReturnValue().Set(Integer::New(value));\
}
#define GL_IMP_uint_enum_intP(name) \
Handle<Value> GLBinding::name##Callback(const Arguments& args) {\
    HandleScope scope;\
    if (args.Length() != 3) {\
        args.GetReturnValue().Set(ThrowException(String::New(#name"Bad arguments, count not 4")));\
    }\
    GLint value;\
    name(_GLuint(args[0]), _GLenum(args[1]), &value);\
    args.GetReturnValue().Set(Integer::New(value));\
}

static Handle<Array> toArray(GLuint* buffers, int size) {
    Handle<Array> arr = Array::New(size);
    for (int i = 0; i < size; ++i) {
        arr->Set(Integer::New(i), Integer::New(buffers[i]));
    }
    return arr;
}

static void init(const v8::FunctionCallbackInfo<Value>& args) {
    HandleScope scope;
    Local<Object> _proto_ = args[0]->ToObject();

    BIND_GL_CONST(_proto_, BYTE, 0x1400);
    BIND_GL_CONST(_proto_, UNSIGNED_BYTE, 0x1401);
    BIND_GL_CONST(_proto_, SHORT, 0x1402);
    BIND_GL_CONST(_proto_, UNSIGNED_SHORT, 0x1403);
    BIND_GL_CONST(_proto_, INT, 0x1404);
    BIND_GL_CONST(_proto_, UNSIGNED_INT, 0x1405);
    BIND_GL_CONST(_proto_, FLOAT, 0x1406);
    BIND_GL_CONST(_proto_, 2_BYTES, 0x1407);
    BIND_GL_CONST(_proto_, 3_BYTES, 0x1408);
    BIND_GL_CONST(_proto_, 4_BYTES, 0x1409);
    BIND_GL_CONST(_proto_, DOUBLE, 0x140A);

    BIND_GL_CONST(_proto_, DEPTH_BUFFER_BIT, 0x00000100);
    BIND_GL_CONST(_proto_, STENCIL_BUFFER_BIT, 0x00000400);
    BIND_GL_CONST(_proto_, COLOR_BUFFER_BIT, 0x00004000);
    BIND_GL_CONST(_proto_, POINTS, 0x0000);
    BIND_GL_CONST(_proto_, LINES, 0x0001);
    BIND_GL_CONST(_proto_, LINE_LOOP, 0x0002);
    BIND_GL_CONST(_proto_, LINE_STRIP, 0x0003);
    BIND_GL_CONST(_proto_, TRIANGLES, 0x0004);
    BIND_GL_CONST(_proto_, TRIANGLE_STRIP, 0x0005);
    BIND_GL_CONST(_proto_, TRIANGLE_FAN, 0x0006);
    BIND_GL_CONST(_proto_, ZERO, 0);
    BIND_GL_CONST(_proto_, ONE, 1);
    BIND_GL_CONST(_proto_, SRC_COLOR, 0x0300);
    BIND_GL_CONST(_proto_, ONE_MINUS_SRC_COLOR, 0x0301);
    BIND_GL_CONST(_proto_, SRC_ALPHA, 0x0302);
    BIND_GL_CONST(_proto_, ONE_MINUS_SRC_ALPHA, 0x0303);
    BIND_GL_CONST(_proto_, DST_ALPHA, 0x0304);
    BIND_GL_CONST(_proto_, ONE_MINUS_DST_ALPHA, 0x0305);
    BIND_GL_CONST(_proto_, DST_COLOR, 0x0306);
    BIND_GL_CONST(_proto_, ONE_MINUS_DST_COLOR, 0x0307);
    BIND_GL_CONST(_proto_, SRC_ALPHA_SATURATE, 0x0308);
    BIND_GL_CONST(_proto_, FUNC_ADD, 0x8006);
    BIND_GL_CONST(_proto_, BLEND_EQUATION, 0x8009);
    BIND_GL_CONST(_proto_, BLEND_EQUATION_RGB, 0x8009);
    BIND_GL_CONST(_proto_, BLEND_EQUATION_ALPHA, 0x883D);
    BIND_GL_CONST(_proto_, FUNC_SUBTRACT, 0x800A);
    BIND_GL_CONST(_proto_, FUNC_REVERSE_SUBTRACT, 0x800B);
    BIND_GL_CONST(_proto_, BLEND_DST_RGB, 0x80C8);
    BIND_GL_CONST(_proto_, BLEND_SRC_RGB, 0x80C9);
    BIND_GL_CONST(_proto_, BLEND_DST_ALPHA, 0x80CA);
    BIND_GL_CONST(_proto_, BLEND_SRC_ALPHA, 0x80CB);
    BIND_GL_CONST(_proto_, CONSTANT_COLOR, 0x8001);
    BIND_GL_CONST(_proto_, ONE_MINUS_CONSTANT_COLOR, 0x8002);
    BIND_GL_CONST(_proto_, CONSTANT_ALPHA, 0x8003);
    BIND_GL_CONST(_proto_, ONE_MINUS_CONSTANT_ALPHA, 0x8004);
    BIND_GL_CONST(_proto_, BLEND_COLOR, 0x8005);
    BIND_GL_CONST(_proto_, ARRAY_BUFFER, 0x8892);
    BIND_GL_CONST(_proto_, ELEMENT_ARRAY_BUFFER, 0x8893);
    BIND_GL_CONST(_proto_, ARRAY_BUFFER_BINDING, 0x8894);
    BIND_GL_CONST(_proto_, ELEMENT_ARRAY_BUFFER_BINDING, 0x8895);
    BIND_GL_CONST(_proto_, STREAM_DRAW, 0x88E0);
    BIND_GL_CONST(_proto_, STATIC_DRAW, 0x88E4);
    BIND_GL_CONST(_proto_, DYNAMIC_DRAW, 0x88E8);
    BIND_GL_CONST(_proto_, BUFFER_SIZE, 0x8764);
    BIND_GL_CONST(_proto_, BUFFER_USAGE, 0x8765);
    BIND_GL_CONST(_proto_, CURRENT_VERTEX_ATTRIB, 0x8626);
    BIND_GL_CONST(_proto_, FRONT, 0x0404);
    BIND_GL_CONST(_proto_, BACK, 0x0405);
    BIND_GL_CONST(_proto_, FRONT_AND_BACK, 0x0408);
    BIND_GL_CONST(_proto_, CULL_FACE, 0x0B44);
    BIND_GL_CONST(_proto_, BLEND, 0x0BE2);
    BIND_GL_CONST(_proto_, DITHER, 0x0BD0);
    BIND_GL_CONST(_proto_, STENCIL_TEST, 0x0B90);
    BIND_GL_CONST(_proto_, DEPTH_TEST, 0x0B71);
    BIND_GL_CONST(_proto_, SCISSOR_TEST, 0x0C11);
    BIND_GL_CONST(_proto_, POLYGON_OFFSET_FILL, 0x8037);
    BIND_GL_CONST(_proto_, SAMPLE_ALPHA_TO_COVERAGE, 0x809E);
    BIND_GL_CONST(_proto_, SAMPLE_COVERAGE, 0x80A0);
    BIND_GL_CONST(_proto_, NO_ERROR, 0);
    BIND_GL_CONST(_proto_, INVALID_ENUM, 0x0500);
    BIND_GL_CONST(_proto_, INVALID_VALUE, 0x0501);
    BIND_GL_CONST(_proto_, INVALID_OPERATION, 0x0502);
    BIND_GL_CONST(_proto_, OUT_OF_MEMORY, 0x0505);
    BIND_GL_CONST(_proto_, CW, 0x0900);
    BIND_GL_CONST(_proto_, CCW, 0x0901);
    BIND_GL_CONST(_proto_, LINE_WIDTH, 0x0B21);
    BIND_GL_CONST(_proto_, ALIASED_POINT_SIZE_RANGE, 0x846D);
    BIND_GL_CONST(_proto_, ALIASED_LINE_WIDTH_RANGE, 0x846E);
    BIND_GL_CONST(_proto_, CULL_FACE_MODE, 0x0B45);
    BIND_GL_CONST(_proto_, FRONT_FACE, 0x0B46);
    BIND_GL_CONST(_proto_, DEPTH_RANGE, 0x0B70);
    BIND_GL_CONST(_proto_, DEPTH_WRITEMASK, 0x0B72);
    BIND_GL_CONST(_proto_, DEPTH_CLEAR_VALUE, 0x0B73);
    BIND_GL_CONST(_proto_, DEPTH_FUNC, 0x0B74);
    BIND_GL_CONST(_proto_, STENCIL_CLEAR_VALUE, 0x0B91);
    BIND_GL_CONST(_proto_, STENCIL_FUNC, 0x0B92);
    BIND_GL_CONST(_proto_, STENCIL_FAIL, 0x0B94);
    BIND_GL_CONST(_proto_, STENCIL_PASS_DEPTH_FAIL, 0x0B95);
    BIND_GL_CONST(_proto_, STENCIL_PASS_DEPTH_PASS, 0x0B96);
    BIND_GL_CONST(_proto_, STENCIL_REF, 0x0B97);
    BIND_GL_CONST(_proto_, STENCIL_VALUE_MASK, 0x0B93);
    BIND_GL_CONST(_proto_, STENCIL_WRITEMASK, 0x0B98);
    BIND_GL_CONST(_proto_, STENCIL_BACK_FUNC, 0x8800);
    BIND_GL_CONST(_proto_, STENCIL_BACK_FAIL, 0x8801);
    BIND_GL_CONST(_proto_, STENCIL_BACK_PASS_DEPTH_FAIL, 0x8802);
    BIND_GL_CONST(_proto_, STENCIL_BACK_PASS_DEPTH_PASS, 0x8803);
    BIND_GL_CONST(_proto_, STENCIL_BACK_REF, 0x8CA3);
    BIND_GL_CONST(_proto_, STENCIL_BACK_VALUE_MASK, 0x8CA4);
    BIND_GL_CONST(_proto_, STENCIL_BACK_WRITEMASK, 0x8CA5);
    BIND_GL_CONST(_proto_, VIEWPORT, 0x0BA2);
    BIND_GL_CONST(_proto_, SCISSOR_BOX, 0x0C10);
    BIND_GL_CONST(_proto_, COLOR_CLEAR_VALUE, 0x0C22);
    BIND_GL_CONST(_proto_, COLOR_WRITEMASK, 0x0C23);
    BIND_GL_CONST(_proto_, UNPACK_ALIGNMENT, 0x0CF5);
    BIND_GL_CONST(_proto_, PACK_ALIGNMENT, 0x0D05);
    BIND_GL_CONST(_proto_, MAX_TEXTURE_SIZE, 0x0D33);
    BIND_GL_CONST(_proto_, MAX_VIEWPORT_DIMS, 0x0D3A);
    BIND_GL_CONST(_proto_, SUBPIXEL_BITS, 0x0D50);
    BIND_GL_CONST(_proto_, RED_BITS, 0x0D52);
    BIND_GL_CONST(_proto_, GREEN_BITS, 0x0D53);
    BIND_GL_CONST(_proto_, BLUE_BITS, 0x0D54);
    BIND_GL_CONST(_proto_, ALPHA_BITS, 0x0D55);
    BIND_GL_CONST(_proto_, DEPTH_BITS, 0x0D56);
    BIND_GL_CONST(_proto_, STENCIL_BITS, 0x0D57);
    BIND_GL_CONST(_proto_, POLYGON_OFFSET_UNITS, 0x2A00);
    BIND_GL_CONST(_proto_, POLYGON_OFFSET_FACTOR, 0x8038);
    BIND_GL_CONST(_proto_, TEXTURE_BINDING_2D, 0x8069);
    BIND_GL_CONST(_proto_, SAMPLE_BUFFERS, 0x80A8);
    BIND_GL_CONST(_proto_, SAMPLES, 0x80A9);
    BIND_GL_CONST(_proto_, SAMPLE_COVERAGE_VALUE, 0x80AA);
    BIND_GL_CONST(_proto_, SAMPLE_COVERAGE_INVERT, 0x80AB);
    BIND_GL_CONST(_proto_, NUM_COMPRESSED_TEXTURE_FORMATS, 0x86A2);
    BIND_GL_CONST(_proto_, COMPRESSED_TEXTURE_FORMATS, 0x86A3);
    BIND_GL_CONST(_proto_, DONT_CARE, 0x1100);
    BIND_GL_CONST(_proto_, FASTEST, 0x1101);
    BIND_GL_CONST(_proto_, NICEST, 0x1102);
    BIND_GL_CONST(_proto_, GENERATE_MIPMAP_HINT, 0x8192);
    BIND_GL_CONST(_proto_, BYTE, 0x1400);
    BIND_GL_CONST(_proto_, UNSIGNED_BYTE, 0x1401);
    BIND_GL_CONST(_proto_, SHORT, 0x1402);
    BIND_GL_CONST(_proto_, UNSIGNED_SHORT, 0x1403);
    BIND_GL_CONST(_proto_, INT, 0x1404);
    BIND_GL_CONST(_proto_, UNSIGNED_INT, 0x1405);
    BIND_GL_CONST(_proto_, FLOAT, 0x1406);
    BIND_GL_CONST(_proto_, DEPTH_COMPONENT, 0x1902);
    BIND_GL_CONST(_proto_, ALPHA, 0x1906);
    BIND_GL_CONST(_proto_, RGB, 0x1907);
    BIND_GL_CONST(_proto_, RGBA, 0x1908);
    BIND_GL_CONST(_proto_, LUMINANCE, 0x1909);
    BIND_GL_CONST(_proto_, LUMINANCE_ALPHA, 0x190A);
    BIND_GL_CONST(_proto_, UNSIGNED_SHORT_4_4_4_4, 0x8033);
    BIND_GL_CONST(_proto_, UNSIGNED_SHORT_5_5_5_1, 0x8034);
    BIND_GL_CONST(_proto_, UNSIGNED_SHORT_5_6_5, 0x8363);
    BIND_GL_CONST(_proto_, FRAGMENT_SHADER, 0x8B30);
    BIND_GL_CONST(_proto_, VERTEX_SHADER, 0x8B31);
    BIND_GL_CONST(_proto_, MAX_VERTEX_ATTRIBS, 0x8869);
    BIND_GL_CONST(_proto_, MAX_VERTEX_UNIFORM_VECTORS, 0x8DFB);
    BIND_GL_CONST(_proto_, MAX_VARYING_VECTORS, 0x8DFC);
    BIND_GL_CONST(_proto_, MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0x8B4D);
    BIND_GL_CONST(_proto_, MAX_VERTEX_TEXTURE_IMAGE_UNITS, 0x8B4C);
    BIND_GL_CONST(_proto_, MAX_TEXTURE_IMAGE_UNITS, 0x8872);
    BIND_GL_CONST(_proto_, MAX_FRAGMENT_UNIFORM_VECTORS, 0x8DFD);
    BIND_GL_CONST(_proto_, SHADER_TYPE, 0x8B4F);
    BIND_GL_CONST(_proto_, DELETE_STATUS, 0x8B80);
    BIND_GL_CONST(_proto_, LINK_STATUS, 0x8B82);
    BIND_GL_CONST(_proto_, VALIDATE_STATUS, 0x8B83);
    BIND_GL_CONST(_proto_, ATTACHED_SHADERS, 0x8B85);
    BIND_GL_CONST(_proto_, ACTIVE_UNIFORMS, 0x8B86);
    BIND_GL_CONST(_proto_, ACTIVE_ATTRIBUTES, 0x8B89);
    BIND_GL_CONST(_proto_, SHADING_LANGUAGE_VERSION, 0x8B8C);
    BIND_GL_CONST(_proto_, CURRENT_PROGRAM, 0x8B8D);
    BIND_GL_CONST(_proto_, NEVER, 0x0200);
    BIND_GL_CONST(_proto_, LESS, 0x0201);
    BIND_GL_CONST(_proto_, EQUAL, 0x0202);
    BIND_GL_CONST(_proto_, LEQUAL, 0x0203);
    BIND_GL_CONST(_proto_, GREATER, 0x0204);
    BIND_GL_CONST(_proto_, NOTEQUAL, 0x0205);
    BIND_GL_CONST(_proto_, GEQUAL, 0x0206);
    BIND_GL_CONST(_proto_, ALWAYS, 0x0207);
    BIND_GL_CONST(_proto_, KEEP, 0x1E00);
    BIND_GL_CONST(_proto_, REPLACE, 0x1E01);
    BIND_GL_CONST(_proto_, INCR, 0x1E02);
    BIND_GL_CONST(_proto_, DECR, 0x1E03);
    BIND_GL_CONST(_proto_, INVERT, 0x150A);
    BIND_GL_CONST(_proto_, INCR_WRAP, 0x8507);
    BIND_GL_CONST(_proto_, DECR_WRAP, 0x8508);
    BIND_GL_CONST(_proto_, VENDOR, 0x1F00);
    BIND_GL_CONST(_proto_, RENDERER, 0x1F01);
    BIND_GL_CONST(_proto_, VERSION, 0x1F02);
    BIND_GL_CONST(_proto_, NEAREST, 0x2600);
    BIND_GL_CONST(_proto_, LINEAR, 0x2601);
    BIND_GL_CONST(_proto_, NEAREST_MIPMAP_NEAREST, 0x2700);
    BIND_GL_CONST(_proto_, LINEAR_MIPMAP_NEAREST, 0x2701);
    BIND_GL_CONST(_proto_, NEAREST_MIPMAP_LINEAR, 0x2702);
    BIND_GL_CONST(_proto_, LINEAR_MIPMAP_LINEAR, 0x2703);
    BIND_GL_CONST(_proto_, TEXTURE_MAG_FILTER, 0x2800);
    BIND_GL_CONST(_proto_, TEXTURE_MIN_FILTER, 0x2801);
    BIND_GL_CONST(_proto_, TEXTURE_WRAP_S, 0x2802);
    BIND_GL_CONST(_proto_, TEXTURE_WRAP_T, 0x2803);
    BIND_GL_CONST(_proto_, TEXTURE_2D, 0x0DE1);
    BIND_GL_CONST(_proto_, TEXTURE, 0x1702);
    BIND_GL_CONST(_proto_, TEXTURE_CUBE_MAP, 0x8513);
    BIND_GL_CONST(_proto_, TEXTURE_BINDING_CUBE_MAP, 0x8514);
    BIND_GL_CONST(_proto_, TEXTURE_CUBE_MAP_POSITIVE_X, 0x8515);
    BIND_GL_CONST(_proto_, TEXTURE_CUBE_MAP_NEGATIVE_X, 0x8516);
    BIND_GL_CONST(_proto_, TEXTURE_CUBE_MAP_POSITIVE_Y, 0x8517);
    BIND_GL_CONST(_proto_, TEXTURE_CUBE_MAP_NEGATIVE_Y, 0x8518);
    BIND_GL_CONST(_proto_, TEXTURE_CUBE_MAP_POSITIVE_Z, 0x8519);
    BIND_GL_CONST(_proto_, TEXTURE_CUBE_MAP_NEGATIVE_Z, 0x851A);
    BIND_GL_CONST(_proto_, MAX_CUBE_MAP_TEXTURE_SIZE, 0x851C);
    BIND_GL_CONST(_proto_, TEXTURE0, 0x84C0);
    BIND_GL_CONST(_proto_, TEXTURE1, 0x84C1);
    BIND_GL_CONST(_proto_, TEXTURE2, 0x84C2);
    BIND_GL_CONST(_proto_, TEXTURE3, 0x84C3);
    BIND_GL_CONST(_proto_, TEXTURE4, 0x84C4);
    BIND_GL_CONST(_proto_, TEXTURE5, 0x84C5);
    BIND_GL_CONST(_proto_, TEXTURE6, 0x84C6);
    BIND_GL_CONST(_proto_, TEXTURE7, 0x84C7);
    BIND_GL_CONST(_proto_, TEXTURE8, 0x84C8);
    BIND_GL_CONST(_proto_, TEXTURE9, 0x84C9);
    BIND_GL_CONST(_proto_, TEXTURE10, 0x84CA);
    BIND_GL_CONST(_proto_, TEXTURE11, 0x84CB);
    BIND_GL_CONST(_proto_, TEXTURE12, 0x84CC);
    BIND_GL_CONST(_proto_, TEXTURE13, 0x84CD);
    BIND_GL_CONST(_proto_, TEXTURE14, 0x84CE);
    BIND_GL_CONST(_proto_, TEXTURE15, 0x84CF);
    BIND_GL_CONST(_proto_, TEXTURE16, 0x84D0);
    BIND_GL_CONST(_proto_, TEXTURE17, 0x84D1);
    BIND_GL_CONST(_proto_, TEXTURE18, 0x84D2);
    BIND_GL_CONST(_proto_, TEXTURE19, 0x84D3);
    BIND_GL_CONST(_proto_, TEXTURE20, 0x84D4);
    BIND_GL_CONST(_proto_, TEXTURE21, 0x84D5);
    BIND_GL_CONST(_proto_, TEXTURE22, 0x84D6);
    BIND_GL_CONST(_proto_, TEXTURE23, 0x84D7);
    BIND_GL_CONST(_proto_, TEXTURE24, 0x84D8);
    BIND_GL_CONST(_proto_, TEXTURE25, 0x84D9);
    BIND_GL_CONST(_proto_, TEXTURE26, 0x84DA);
    BIND_GL_CONST(_proto_, TEXTURE27, 0x84DB);
    BIND_GL_CONST(_proto_, TEXTURE28, 0x84DC);
    BIND_GL_CONST(_proto_, TEXTURE29, 0x84DD);
    BIND_GL_CONST(_proto_, TEXTURE30, 0x84DE);
    BIND_GL_CONST(_proto_, TEXTURE31, 0x84DF);
    BIND_GL_CONST(_proto_, ACTIVE_TEXTURE, 0x84E0);
    BIND_GL_CONST(_proto_, REPEAT, 0x2901);
    BIND_GL_CONST(_proto_, CLAMP_TO_EDGE, 0x812F);
    BIND_GL_CONST(_proto_, MIRRORED_REPEAT, 0x8370);
    BIND_GL_CONST(_proto_, FLOAT_VEC2, 0x8B50);
    BIND_GL_CONST(_proto_, FLOAT_VEC3, 0x8B51);
    BIND_GL_CONST(_proto_, FLOAT_VEC4, 0x8B52);
    BIND_GL_CONST(_proto_, INT_VEC2, 0x8B53);
    BIND_GL_CONST(_proto_, INT_VEC3, 0x8B54);
    BIND_GL_CONST(_proto_, INT_VEC4, 0x8B55);
    BIND_GL_CONST(_proto_, BOOL, 0x8B56);
    BIND_GL_CONST(_proto_, BOOL_VEC2, 0x8B57);
    BIND_GL_CONST(_proto_, BOOL_VEC3, 0x8B58);
    BIND_GL_CONST(_proto_, BOOL_VEC4, 0x8B59);
    BIND_GL_CONST(_proto_, FLOAT_MAT2, 0x8B5A);
    BIND_GL_CONST(_proto_, FLOAT_MAT3, 0x8B5B);
    BIND_GL_CONST(_proto_, FLOAT_MAT4, 0x8B5C);
    BIND_GL_CONST(_proto_, SAMPLER_2D, 0x8B5E);
    BIND_GL_CONST(_proto_, SAMPLER_CUBE, 0x8B60);
    BIND_GL_CONST(_proto_, VERTEX_ATTRIB_ARRAY_ENABLED, 0x8622);
    BIND_GL_CONST(_proto_, VERTEX_ATTRIB_ARRAY_SIZE, 0x8623);
    BIND_GL_CONST(_proto_, VERTEX_ATTRIB_ARRAY_STRIDE, 0x8624);
    BIND_GL_CONST(_proto_, VERTEX_ATTRIB_ARRAY_TYPE, 0x8625);
    BIND_GL_CONST(_proto_, VERTEX_ATTRIB_ARRAY_NORMALIZED, 0x886A);
    BIND_GL_CONST(_proto_, VERTEX_ATTRIB_ARRAY_POINTER, 0x8645);
    BIND_GL_CONST(_proto_, VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, 0x889F);
    BIND_GL_CONST(_proto_, COMPILE_STATUS, 0x8B81);
    BIND_GL_CONST(_proto_, LOW_FLOAT, 0x8DF0);
    BIND_GL_CONST(_proto_, MEDIUM_FLOAT, 0x8DF1);
    BIND_GL_CONST(_proto_, HIGH_FLOAT, 0x8DF2);
    BIND_GL_CONST(_proto_, LOW_INT, 0x8DF3);
    BIND_GL_CONST(_proto_, MEDIUM_INT, 0x8DF4);
    BIND_GL_CONST(_proto_, HIGH_INT, 0x8DF5);
    BIND_GL_CONST(_proto_, FRAMEBUFFER, 0x8D40);
    BIND_GL_CONST(_proto_, RENDERBUFFER, 0x8D41);
    BIND_GL_CONST(_proto_, RGBA4, 0x8056);
    BIND_GL_CONST(_proto_, RGB5_A1, 0x8057);
    BIND_GL_CONST(_proto_, RGB565, 0x8D62);
    BIND_GL_CONST(_proto_, DEPTH_COMPONENT16, 0x81A5);
    BIND_GL_CONST(_proto_, STENCIL_INDEX, 0x1901);
    BIND_GL_CONST(_proto_, STENCIL_INDEX8, 0x8D48);
    BIND_GL_CONST(_proto_, DEPTH_STENCIL, 0x84F9);
    BIND_GL_CONST(_proto_, RENDERBUFFER_WIDTH, 0x8D42);
    BIND_GL_CONST(_proto_, RENDERBUFFER_HEIGHT, 0x8D43);
    BIND_GL_CONST(_proto_, RENDERBUFFER_INTERNAL_FORMAT, 0x8D44);
    BIND_GL_CONST(_proto_, RENDERBUFFER_RED_SIZE, 0x8D50);
    BIND_GL_CONST(_proto_, RENDERBUFFER_GREEN_SIZE, 0x8D51);
    BIND_GL_CONST(_proto_, RENDERBUFFER_BLUE_SIZE, 0x8D52);
    BIND_GL_CONST(_proto_, RENDERBUFFER_ALPHA_SIZE, 0x8D53);
    BIND_GL_CONST(_proto_, RENDERBUFFER_DEPTH_SIZE, 0x8D54);
    BIND_GL_CONST(_proto_, RENDERBUFFER_STENCIL_SIZE, 0x8D55);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, 0x8CD0);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, 0x8CD1);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL, 0x8CD2);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE, 0x8CD3);
    BIND_GL_CONST(_proto_, COLOR_ATTACHMENT0, 0x8CE0);
    BIND_GL_CONST(_proto_, DEPTH_ATTACHMENT, 0x8D00);
    BIND_GL_CONST(_proto_, STENCIL_ATTACHMENT, 0x8D20);
    BIND_GL_CONST(_proto_, DEPTH_STENCIL_ATTACHMENT, 0x821A);
    BIND_GL_CONST(_proto_, NONE, 0);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_COMPLETE, 0x8CD5);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_INCOMPLETE_ATTACHMENT, 0x8CD6);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, 0x8CD7);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_INCOMPLETE_DIMENSIONS, 0x8CD9);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_UNSUPPORTED, 0x8CDD);
    BIND_GL_CONST(_proto_, FRAMEBUFFER_BINDING, 0x8CA6);
    BIND_GL_CONST(_proto_, RENDERBUFFER_BINDING, 0x8CA7);
    BIND_GL_CONST(_proto_, MAX_RENDERBUFFER_SIZE, 0x84E8);
    BIND_GL_CONST(_proto_, INVALID_FRAMEBUFFER_OPERATION, 0x0506);
    BIND_GL_CONST(_proto_, UNPACK_FLIP_Y_WEBGL, 0x9240);
    BIND_GL_CONST(_proto_, UNPACK_PREMULTIPLY_ALPHA_WEBGL, 0x9241);
    BIND_GL_CONST(_proto_, CONTEXT_LOST_WEBGL, 0x9242);
    BIND_GL_CONST(_proto_, UNPACK_COLORSPACE_CONVERSION_WEBGL, 0x9243);
    BIND_GL_CONST(_proto_, BROWSER_DEFAULT_WEBGL, 0x9244);

    BIND_GL(_proto_, getAttachedShaders);
    BIND_GL(_proto_, getSupportedExtensions);
    BIND_GL(_proto_, getExtension);
    BIND_GL(_proto_, activeTexture);
    BIND_GL(_proto_, attachShader);
    BIND_GL(_proto_, bindAttribLocation);
    BIND_GL(_proto_, bindBuffer);
    BIND_GL(_proto_, bindFramebuffer);
    BIND_GL(_proto_, bindRenderbuffer);
    BIND_GL(_proto_, bindTexture);
    BIND_GL(_proto_, blendColor);
    BIND_GL(_proto_, blendEquation);
    BIND_GL(_proto_, blendEquationSeparate);
    BIND_GL(_proto_, blendFunc);
    BIND_GL(_proto_, blendFuncSeparate);
    BIND_GL(_proto_, bufferData);
    BIND_GL(_proto_, bufferSubData);
    BIND_GL(_proto_, checkFramebufferStatus);
    BIND_GL(_proto_, clear);
    BIND_GL(_proto_, clearColor);
    BIND_GL(_proto_, clearDepth);
    BIND_GL(_proto_, clearStencil);
    BIND_GL(_proto_, colorMask);
    BIND_GL(_proto_, compileShader);
    BIND_GL(_proto_, copyTexImage2D);
    BIND_GL(_proto_, copyTexSubImage2D);
    BIND_GL(_proto_, createBuffer);
    BIND_GL(_proto_, createFramebuffer);
    BIND_GL(_proto_, createProgram);
    BIND_GL(_proto_, createRenderbuffer);
    BIND_GL(_proto_, createShader);
    BIND_GL(_proto_, createTexture);
    BIND_GL(_proto_, cullFace);
    BIND_GL(_proto_, deleteBuffer);
    BIND_GL(_proto_, deleteFramebuffer);
    BIND_GL(_proto_, deleteProgram);
    BIND_GL(_proto_, deleteRenderbuffer);
    BIND_GL(_proto_, deleteShader);
    BIND_GL(_proto_, deleteTexture);
    BIND_GL(_proto_, depthFunc);
    BIND_GL(_proto_, depthMask);
    BIND_GL(_proto_, depthRange);
    BIND_GL(_proto_, detachShader);
    BIND_GL(_proto_, disable);
    BIND_GL(_proto_, disableVertexAttribArray);
    BIND_GL(_proto_, drawArrays);
    BIND_GL(_proto_, drawElements);
    BIND_GL(_proto_, enable);
    BIND_GL(_proto_, enableVertexAttribArray);
    BIND_GL(_proto_, finish);
    BIND_GL(_proto_, flush);
    BIND_GL(_proto_, framebufferRenderbuffer);
    BIND_GL(_proto_, framebufferTexture2D);
    BIND_GL(_proto_, frontFace);
    BIND_GL(_proto_, generateMipmap);
    BIND_GL(_proto_, getActiveAttrib);
    BIND_GL(_proto_, getActiveUniform);
    BIND_GL(_proto_, getAttribLocation);
    BIND_GL(_proto_, getParameter);// *
    BIND_GL(_proto_, getBufferParameter);
    BIND_GL(_proto_, getError);
    BIND_GL(_proto_, getFramebufferAttachmentParameter);
    BIND_GL(_proto_, getProgramParameter);
    BIND_GL(_proto_, getProgramInfoLog);
    BIND_GL(_proto_, getRenderbufferParameter);
    BIND_GL(_proto_, getShaderParameter);
    BIND_GL(_proto_, getShaderInfoLog);
    BIND_GL(_proto_, getShaderSource);
    BIND_GL(_proto_, getTexParameter);
//    BIND_GL(_proto_, getUniform);// *
    BIND_GL(_proto_, getUniformLocation);
    BIND_GL(_proto_, getVertexAttrib);// check
    BIND_GL(_proto_, getVertexAttribOffset);// check
    BIND_GL(_proto_, hint);
    BIND_GL(_proto_, isBuffer);
    BIND_GL(_proto_, isEnabled);
    BIND_GL(_proto_, isFramebuffer);
    BIND_GL(_proto_, isProgram);
    BIND_GL(_proto_, isRenderbuffer);
    BIND_GL(_proto_, isShader);
    BIND_GL(_proto_, isTexture);
    BIND_GL(_proto_, lineWidth);
    BIND_GL(_proto_, linkProgram);
    BIND_GL(_proto_, pixelStorei);
    BIND_GL(_proto_, polygonOffset);
    BIND_GL(_proto_, readPixels);// *
    BIND_GL(_proto_, renderbufferStorage);
    BIND_GL(_proto_, sampleCoverage);
    BIND_GL(_proto_, scissor);
    BIND_GL(_proto_, shaderSource);
    BIND_GL(_proto_, stencilFunc);
    BIND_GL(_proto_, stencilFuncSeparate);
    BIND_GL(_proto_, stencilMask);
    BIND_GL(_proto_, stencilMaskSeparate);
    BIND_GL(_proto_, stencilOp);
    BIND_GL(_proto_, stencilOpSeparate);
    BIND_GL(_proto_, texImage2D);
    BIND_GL(_proto_, internalTexImage2D);
    BIND_GL(_proto_, texParameterf);
    BIND_GL(_proto_, texParameteri);
    BIND_GL(_proto_, texSubImage2D);
    BIND_GL(_proto_, internalTexSubImage2D);
    BIND_GL(_proto_, uniform1f);
    BIND_GL(_proto_, uniform1fv);
    BIND_GL(_proto_, uniform1i);
    BIND_GL(_proto_, uniform1iv);
    BIND_GL(_proto_, uniform2f);
    BIND_GL(_proto_, uniform2fv);
    BIND_GL(_proto_, uniform2i);
    BIND_GL(_proto_, uniform2iv);
    BIND_GL(_proto_, uniform3f);
    BIND_GL(_proto_, uniform3fv);
    BIND_GL(_proto_, uniform3i);
    BIND_GL(_proto_, uniform3iv);
    BIND_GL(_proto_, uniform4f);
    BIND_GL(_proto_, uniform4fv);
    BIND_GL(_proto_, uniform4i);
    BIND_GL(_proto_, uniform4iv);
    BIND_GL(_proto_, uniformMatrix2fv);
    BIND_GL(_proto_, uniformMatrix3fv);
    BIND_GL(_proto_, uniformMatrix4fv);

	BIND_GL(_proto_, useProgram);
	BIND_GL(_proto_, validateProgram);
	BIND_GL(_proto_, vertexAttrib1f);
	BIND_GL(_proto_, vertexAttrib1fv);
	BIND_GL(_proto_, vertexAttrib2f);
	BIND_GL(_proto_, vertexAttrib2fv);
	BIND_GL(_proto_, vertexAttrib3f);
	BIND_GL(_proto_, vertexAttrib3fv);
	BIND_GL(_proto_, vertexAttrib4f);
	BIND_GL(_proto_, vertexAttrib4fv);
	BIND_GL(_proto_, vertexAttribPointer);
	BIND_GL(_proto_, viewport);
}

JS_METHOD(getAttachedShaders) {
    HandleScope scope;
    GLuint program = ARGS_GLuint(args[0]);

	GLuint shaders[1024];
	GLsizei count;
	glGetAttachedShaders(program, 1024, &count, shaders);

	Local<Array> shadersArr = Array::New(count);
	for(int i=0;i<count;i++) {
		shadersArr->Set(i, JS_UINT(shaders[i]));
	}
    args.GetReturnValue().Set(shadersArr);
}
JS_METHOD(getSupportedExtensions) {
    HandleScope scope;
    char *extensions=(char*) glGetString(GL_EXTENSIONS);
	args.GetReturnValue().Set(JS_STR(extensions));
}
JS_METHOD(getExtension) {
    HandleScope scope;
	args.GetReturnValue().Set(JS_STR(""));
}

DELEGATE_TO_GL_N1(activeTexture, glActiveTexture, GLenum);
DELEGATE_TO_GL_N2(attachShader, glAttachShader, GLuint, GLuint);
DELEGATE_TO_GL_N3(bindAttribLocation, glBindAttribLocation, GLuint, GLuint, GLcharP);
DELEGATE_TO_GL_N2(bindBuffer, glBindBuffer, GLenum, GLuint);
//JS_METHOD(bindBuffer) {
//    HandleScope scope;
//    CHECK_ARG_2(bindBuffer, GLenum, GLuint);
//    CALL_GL_2(glBindBuffer, GLenum, GLuint);
//    LOGI("bindBuffer %x %x", args[0]->Uint32Value(), args[1]->Uint32Value());
//    checkGlError("bindBuffer");
//}
DELEGATE_TO_GL_N2(bindFramebuffer, glBindFramebuffer, GLenum, GLuint);
DELEGATE_TO_GL_N2(bindRenderbuffer, glBindRenderbuffer, GLenum, GLuint);
DELEGATE_TO_GL_N2(bindTexture, glBindTexture, GLenum, GLuint);
DELEGATE_TO_GL_N4(blendColor, glBlendColor, GLclampf, GLclampf, GLclampf, GLclampf);
DELEGATE_TO_GL_N1(blendEquation, glBlendEquation, GLenum);
DELEGATE_TO_GL_N2(blendEquationSeparate, glBlendEquationSeparate, GLenum, GLenum);
//DELEGATE_TO_GL_N2(blendFunc, glBlendFunc, GLenum, GLenum);
JS_METHOD(blendFunc) {
    HandleScope scope;

 	GLenum source = args[0]->Uint32Value();
 	GLenum dest = args[1]->Uint32Value();
    glBlendFunc(source, dest);
}
DELEGATE_TO_GL_N4(blendFuncSeparate, glBlendFuncSeparate, GLenum, GLenum, GLenum, GLenum);

/**
 @param {Number} target
 @param {Number} size | {ArrayBufferView} data | {ArrayBuffer} data
 @param {Number} usage
 */
JS_METHOD(bufferData) {
 	HandleScope scope;
 	int target = args[0]->Int32Value();
    GLenum usage = args[2]->Int32Value();

 	if(args[1]->IsObject()) {
        ClassBase* base = internalArg<ClassBase>(args[1]);
        if(base == NULL) {
            ThrowException(String::New(("bufferData with NULL data")));
            return;
        }

        ByteBuffer buf;
        base->getUnderlying(&buf);
 		glBufferData(target, buf.mByteLength, buf.value_ptr(), usage);

 	} else if(args[1]->IsNumber()) {// create an empty buffer
 		GLsizeiptr size = args[1]->Uint32Value();
 		glBufferData(target, size, NULL, usage);
 	}
    checkGlError("glBufferData");
}
/**
 @param {Number} target
 @param {Number} offset
 @param {ArrayBuffer} data | {ArrayBufferView} data
 */
JS_METHOD(bufferSubData) {
    HandleScope scope;

 	int target = args[0]->Int32Value();
 	int offset = args[1]->Int32Value();
    ClassBase* c = internalArg<ClassBase>(args[2]);
    if(c == 0) {
        ThrowException(String::New(("bufferSubData with NULL data")));
        return;
    }

    ByteBuffer buf;
    c->getUnderlying(&buf);
 	glBufferSubData(target, offset, buf.mByteLength, buf.value_ptr());
}
/**
 @param {Number} target
 @return {Number}
 */
JS_METHOD(checkFramebufferStatus) {
    HandleScope scope;
    CHECK_ARG_1(glCheckFramebufferStatus, GLenum);
    args.GetReturnValue().Set(Uint32::New(CALL_GL_1(glCheckFramebufferStatus, GLenum)));
}
DELEGATE_TO_GL_N1(clear, glClear, GLbitfield);
DELEGATE_TO_GL_N4(clearColor, glClearColor, GLclampf, GLclampf, GLclampf, GLclampf);
//JS_METHOD(clearColor) {
//    LOGI("args[0] %lf", args[0]->NumberValue());
//}
DELEGATE_TO_GL_N1(clearDepth, glClearDepthf, GLclampf);
DELEGATE_TO_GL_N1(clearStencil, glClearStencil, GLint);
DELEGATE_TO_GL_N4(colorMask, glColorMask, GLboolean, GLboolean, GLboolean, GLboolean);
DELEGATE_TO_GL_N1(compileShader, glCompileShader, GLuint);

/**
 @param {Number} target
 @param {Number} level
 @param {Number} internalformat
 @param {Number} x
 @param {Number} y
 @param {Number} width
 @param {Number} height
 @param {Number} border
 @return {void}
 */
DELEGATE_TO_GL_N8(copyTexImage2D, glCopyTexImage2D, GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint);
/**
 @param {Number} target
 @param {Number} level
 @param {Number} xoffset
 @param {Number} yoffset
 @param {Number} x
 @param {Number} y
 @param {Number} width
 @param {Number} height
 @return {void}
 */
DELEGATE_TO_GL_N8(copyTexSubImage2D, glCopyTexSubImage2D, GLenum , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei);
DELEGATE_TO_GL_1R(createBuffer, glGenBuffers, GLuint, Uint32);
DELEGATE_TO_GL_1R(createFramebuffer, glGenFramebuffers, GLuint, Uint32);
DELEGATE_TO_GL_R(createProgram, glCreateProgram, GLuint, Uint32);
DELEGATE_TO_GL_1R(createRenderbuffer, glGenRenderbuffers, GLuint, Uint32);
JS_METHOD(createShader) {
	HandleScope scope;
    CHECK_ARG_1(glCreateShader, GLenum);
    GLuint value = glCreateShader(ARGS_GLenum(args[0]));
    args.GetReturnValue().Set(value);
}
DELEGATE_TO_GL_1R(createTexture, glGenTextures, GLuint, Uint32);
DELEGATE_TO_GL_N1(cullFace, glCullFace, GLenum);
DELEGATE_TO_GL_1A(deleteBuffer, glDeleteBuffers, GLuint);
DELEGATE_TO_GL_1A(deleteFramebuffer, glDeleteFramebuffers, GLuint);
DELEGATE_TO_GL_N1(deleteProgram, glDeleteProgram, GLuint);
DELEGATE_TO_GL_1A(deleteRenderbuffer, glDeleteRenderbuffers, GLuint);
DELEGATE_TO_GL_N1(deleteShader, glDeleteShader, GLuint);
DELEGATE_TO_GL_1A(deleteTexture, glDeleteTextures, GLuint);
DELEGATE_TO_GL_N1(depthFunc, glDepthFunc, GLenum);
DELEGATE_TO_GL_N1(depthMask, glDepthMask, GLboolean);
DELEGATE_TO_GL_N2(depthRange, glDepthRangef, GLclampf, GLclampf);
DELEGATE_TO_GL_N2(detachShader, glDetachShader, GLuint, GLuint);
DELEGATE_TO_GL_N1(disable, glDisable, GLenum);
DELEGATE_TO_GL_N1(disableVertexAttribArray, glDisableVertexAttribArray, GLuint);
//DELEGATE_TO_GL_N3(drawArrays, glDrawArrays, GLenum, GLint, GLsizei);

/**
 @param {Number} mode
 @param {Number} first
 @param {Number} count
 */
JS_METHOD(drawArrays) {
    HandleScope scope;
    GLenum mode = ARGS_GLenum(args[0]);
    GLint first = ARGS_GLint(args[1]);
    GLsizei count = ARGS_GLsizei(args[2]);
    checkGlError("before drawArrays");
    glDrawArrays(mode, first, count);
    checkGlError("after drawArrays");
}
/**
 * GLenum mode, GLsizei count, GLenum type, const GLvoid *indices
 */
JS_METHOD(drawElements) {
    GLenum mode = ARGS_GLenum(args[0]);
    GLsizei count = ARGS_GLsizei(args[1]);
    GLenum type = ARGS_GLenum(args[2]);
    if(args[3]->IsInt32()) {
        glDrawElements(mode, count, type, (void*)(args[3]->Int32Value()));
    } else {
        ByteBuffer buf;
        getInternal(args[3], &buf);
        glDrawElements(mode, count, type, buf.value_ptr());
    }
}
DELEGATE_TO_GL_N1(enable, glEnable, GLenum);
DELEGATE_TO_GL_N1(enableVertexAttribArray, glEnableVertexAttribArray, GLuint);
DELEGATE_TO_GL_N(finish, glFinish);
DELEGATE_TO_GL_N(flush, glFlush);
/**
 @param {Number} target
 @param {Number} attachment
 @param {Number} renderbuffertarget
 @param {WebGLRenderbuffer} renderbuffer
 @return {void}
 */
DELEGATE_TO_GL_N4(framebufferRenderbuffer, glFramebufferRenderbuffer, GLenum, GLenum, GLenum, GLuint);
/**
 @param {Number} target
 @param {Number} attachment
 @param {Number} textarget
 @param {WebGLTexture} texture
 @param {Number} level
 @return {void}
 */
DELEGATE_TO_GL_N5(framebufferTexture2D, glFramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint);
DELEGATE_TO_GL_N1(frontFace, glFrontFace, GLenum);
DELEGATE_TO_GL_N1(generateMipmap, glGenerateMipmap, GLenum);

JS_METHOD(getActiveAttrib) {
    HandleScope scope;

	GLuint program = ARGS_GLuint(args[0]);
	GLint max_name_length = -1;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_length);
	if (max_name_length < 0) {
		return;
	}

	GLchar* name = new GLchar[max_name_length];
	GLuint index = ARGS_GLuint(args[1]);
	GLsizei length = 0;
	GLint size = -1;
	GLenum type = 0;
	glGetActiveAttrib(program, index, max_name_length, &length, &size, &type, name);
	if (size < 0) {
		delete name;
		return;
	}

	Handle<Object> info = Object::New();
	info->Set(String::New("name"), String::New(name, length));
	info->Set(String::New("type"), Uint32::New(type));
	info->Set(String::New("size"), Integer::New(size));
	delete name;

	args.GetReturnValue().Set(info);
}
JS_METHOD(getActiveUniform) {
    HandleScope scope;

	GLuint program = ARGS_GLuint(args[0]);
	GLint max_name_length = -1;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);
	if (max_name_length < 0) {
		return;
	}

	GLchar* name = new GLchar[max_name_length];
	GLuint index = ARGS_GLuint(args[1]);
	GLsizei length = 0;
	GLint size = -1;
	GLenum type = 0;
	glGetActiveUniform(program, index, max_name_length, &length, &size, &type, name);
	if (size < 0) {
		delete name;
		return;
	}

	Handle<Object> info = Object::New();
	info->Set(String::New("name"), String::New(name, length));
	info->Set(String::New("type"), Uint32::New(type));
	info->Set(String::New("size"), Integer::New(size));
	delete name;

	args.GetReturnValue().Set(info);
}
JS_METHOD(getAttribLocation) {
    HandleScope scope;
    int res = CALL_GL_2(glGetAttribLocation, GLuint, GLcharP);
    args.GetReturnValue().Set(Integer::New(res));
}
JS_METHOD(getParameter) {
    HandleScope scope;

    GLenum pname = ARGS_GLenum(args[0]);
//    UNUSED_PARAM(ec);
//	if (isContextLost())
//		return WebGLGetInfo();
	const int intZero = 0;
//	WebGLStateRestorer(this, false);
	switch (pname) {
	case GL_ACTIVE_TEXTURE:
		args.GetReturnValue().Set(getUnsignedIntParameter(pname));
        return;
	case GL_ALIASED_LINE_WIDTH_RANGE:
		args.GetReturnValue().Set(getWebGLFloatArrayParameter(pname));
            return;
//	case GL_ALIASED_POINT_SIZE_RANGE:
//		args.GetReturnValue().Set(getWebGLFloatArrayParameter(pname));
//            return;
//	case GL_ALPHA_BITS:
//		args.GetReturnValue().Set(getIntParameter(pname));
//            return;
//	case GL_ARRAY_BUFFER_BINDING:
//		return WebGLGetInfo(PassRefPtr<WebGLBuffer>(m_boundArrayBuffer));
	case GL_BLEND:
		args.GetReturnValue().Set(getBooleanParameter(pname));
            return;
	case GL_BLEND_COLOR:
		args.GetReturnValue().Set(getWebGLFloatArrayParameter(pname));
            return;
	case GL_BLEND_DST_ALPHA:
		args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
	case GL_BLEND_DST_RGB:
		args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
	case GL_BLEND_EQUATION_ALPHA:
		args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
	case GL_BLEND_EQUATION_RGB:
		args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
	case GL_BLEND_SRC_ALPHA:
		args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
	case GL_BLEND_SRC_RGB:
		args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
//	case GL_BLUE_BITS:
//		args.GetReturnValue().Set(getIntParameter(pname));
//            return;
	case GL_COLOR_CLEAR_VALUE:
		args.GetReturnValue().Set(getWebGLFloatArrayParameter(pname));
            return;
	case GL_COLOR_WRITEMASK:
		args.GetReturnValue().Set(getBooleanArrayParameter(pname));
            return;
//	case GL_COMPRESSED_TEXTURE_FORMATS:
//		return WebGLGetInfo(Uint32Array::create(m_compressedTextureFormats.data(), m_compressedTextureFormats.size()));
        case GL_CULL_FACE:
            args.GetReturnValue().Set(getBooleanParameter(pname));
            return;
        case GL_CULL_FACE_MODE:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
            //	case GL_CURRENT_PROGRAM:
            //		args.GetReturnValue().Set(WebGLGetInfo(PassRefPtr<WebGLProgram>(m_currentProgram)));
            //return;
//        case GL_DEPTH_BITS:
//            args.GetReturnValue().Set(getIntParameter(pname));
//            return;
        case GL_DEPTH_CLEAR_VALUE:
            args.GetReturnValue().Set(getFloatParameter(pname));
            return;
        case GL_DEPTH_FUNC:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_DEPTH_RANGE:
            args.GetReturnValue().Set(getWebGLFloatArrayParameter(pname));
            return;
        case GL_DEPTH_TEST:
            args.GetReturnValue().Set(getBooleanParameter(pname));
            return;
        case GL_DEPTH_WRITEMASK:
            args.GetReturnValue().Set(getBooleanParameter(pname));
            return;
        case GL_DITHER:
            args.GetReturnValue().Set(getBooleanParameter(pname));
            return;
        case GL_ELEMENT_ARRAY_BUFFER_BINDING:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_FRAMEBUFFER_BINDING:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_FRONT_FACE:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
//        case GL_GENERATE_MIPMAP_HINT:
//            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
//            return;
//        case GL_GREEN_BITS:
//            args.GetReturnValue().Set(getIntParameter(pname));
//            return;
        case GL_LINE_WIDTH:
            args.GetReturnValue().Set(getFloatParameter(pname));
            return;
        case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
            //	case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
            //		args.GetReturnValue().Set(getIntParameter(pname));
            //return;
        case GL_MAX_RENDERBUFFER_SIZE:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_MAX_TEXTURE_IMAGE_UNITS:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_MAX_TEXTURE_SIZE:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
            //	case GL_MAX_VARYING_VECTORS:
            //		args.GetReturnValue().Set(getIntParameter(pname));
            //return;
        case GL_MAX_VERTEX_ATTRIBS:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
            //	case GL_MAX_VERTEX_UNIFORM_VECTORS:
            //		args.GetReturnValue().Set(getIntParameter(pname));
            //return;
        case GL_MAX_VIEWPORT_DIMS:
            args.GetReturnValue().Set(getWebGLIntArrayParameter(pname));
            return;
            //	case GL_NUM_SHADER_BINARY_FORMATS:
            //		// FIXME: should we always return 0 for this?
            //		args.GetReturnValue().Set(getIntParameter(pname));
            //return;
        case GL_PACK_ALIGNMENT:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_POLYGON_OFFSET_FACTOR:
            args.GetReturnValue().Set(getFloatParameter(pname));
            return;
        case GL_POLYGON_OFFSET_FILL:
            args.GetReturnValue().Set(getBooleanParameter(pname));
            return;
        case GL_POLYGON_OFFSET_UNITS:
            args.GetReturnValue().Set(getFloatParameter(pname));
            return;
//        case GL_RED_BITS:
//            args.GetReturnValue().Set(getIntParameter(pname));
//            return;
        case GL_RENDERBUFFER_BINDING:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_RENDERER:
            args.GetReturnValue().Set(getStringParameter(pname));
            return;
        case GL_SAMPLE_BUFFERS:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_SAMPLE_COVERAGE_INVERT:
            args.GetReturnValue().Set(getBooleanParameter(pname));
            return;
        case GL_SAMPLE_COVERAGE_VALUE:
            args.GetReturnValue().Set(getFloatParameter(pname));
            return;
        case GL_SAMPLES:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_SCISSOR_BOX:
            args.GetReturnValue().Set(getWebGLIntArrayParameter(pname));
            return;
        case GL_SCISSOR_TEST:
            args.GetReturnValue().Set(getBooleanParameter(pname));
            return;
        case GL_SHADING_LANGUAGE_VERSION:
            args.GetReturnValue().Set(getStringParameter(pname));
            return;
        case GL_STENCIL_BACK_FAIL:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_BACK_FUNC:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_BACK_PASS_DEPTH_PASS:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_BACK_REF:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_STENCIL_BACK_VALUE_MASK:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_BACK_WRITEMASK:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
//        case GL_STENCIL_BITS:
//            args.GetReturnValue().Set(getIntParameter(pname));
//            return;
        case GL_STENCIL_CLEAR_VALUE:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_STENCIL_FAIL:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_FUNC:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_PASS_DEPTH_FAIL:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_PASS_DEPTH_PASS:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_REF:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_STENCIL_TEST:
            args.GetReturnValue().Set(getBooleanParameter(pname));
            return;
        case GL_STENCIL_VALUE_MASK:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_STENCIL_WRITEMASK:
            args.GetReturnValue().Set(getUnsignedIntParameter(pname));
            return;
        case GL_SUBPIXEL_BITS:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_TEXTURE_BINDING_2D:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_TEXTURE_BINDING_CUBE_MAP:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
        case GL_UNPACK_ALIGNMENT:
            args.GetReturnValue().Set(getIntParameter(pname));
            return;
//	case GL_UNPACK_FLIP_Y_WEBGL:
//		return WebGLGetInfo(m_unpackFlipY);
//	case GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL:
//		return WebGLGetInfo(m_unpackPremultiplyAlpha);
//	case GL_UNPACK_COLORSPACE_CONVERSION_WEBGL:
//		return WebGLGetInfo(m_unpackColorspaceConversion);
	case GL_VENDOR:
		args.GetReturnValue().Set(getStringParameter(pname));
            return;
	case GL_VERSION:
    args.GetReturnValue().Set(getStringParameter(pname));
            return;
	case GL_VIEWPORT:
		args.GetReturnValue().Set(getWebGLIntArrayParameter(pname));
            return;
//	case GL_Extensions3D::FRAGMENT_SHADER_DERIVATIVE_HINT_OES: // OES_standard_derivatives
//		if (m_oesStandardDerivatives)
//			return getUnsignedIntParameter(Extensions3D::FRAGMENT_SHADER_DERIVATIVE_HINT_OES);
//		synthesizeGLError(INVALID_ENUM, "getParameter", "invalid parameter name, OES_standard_derivatives not enabled");
//		return WebGLGetInfo();
//	case GL_WebGLDebugRendererInfo::UNMASKED_RENDERER_WEBGL:
//		if (m_webglDebugRendererInfo)
//			return WebGLGetInfo(m_context->getString(RENDERER));
//		synthesizeGLError(INVALID_ENUM, "getParameter", "invalid parameter name, WEBGL_debug_renderer_info not enabled");
//		return WebGLGetInfo();
//	case GL_WebGLDebugRendererInfo::UNMASKED_VENDOR_WEBGL:
//		if (m_webglDebugRendererInfo)
//			return WebGLGetInfo(m_context->getString(VENDOR));
//		synthesizeGLError(INVALID_ENUM, "getParameter", "invalid parameter name, WEBGL_debug_renderer_info not enabled");
//		return WebGLGetInfo();
//	case GL_Extensions3D::VERTEX_ARRAY_BINDING_OES: // OES_vertex_array_object
//		if (m_oesVertexArrayObject) {
//			if (!m_boundVertexArrayObject->isDefaultObject())
//				return WebGLGetInfo(PassRefPtr<WebGLVertexArrayObjectOES>(m_boundVertexArrayObject));
//			return WebGLGetInfo();
//		}
//		synthesizeGLError(INVALID_ENUM, "getParameter", "invalid parameter name, OES_vertex_array_object not enabled");
//		return WebGLGetInfo();
//	case GL_Extensions3D::MAX_TEXTURE_MAX_ANISOTROPY_EXT: // EXT_texture_filter_anisotropic
//		if (m_extTextureFilterAnisotropic)
//			return getUnsignedIntParameter(Extensions3D::MAX_TEXTURE_MAX_ANISOTROPY_EXT);
//		synthesizeGLError(INVALID_ENUM, "getParameter", "invalid parameter name, EXT_texture_filter_anisotropic not enabled");
//		return WebGLGetInfo();
//	case GL_Extensions3D::MAX_COLOR_ATTACHMENTS_EXT: // EXT_draw_buffers BEGIN
//		if (m_extDrawBuffers)
//			return WebGLGetInfo(getMaxColorAttachments());
//		synthesizeGLError(INVALID_ENUM, "getParameter", "invalid parameter name, EXT_draw_buffers not enabled");
//		return WebGLGetInfo();
//	case GL_Extensions3D::MAX_DRAW_BUFFERS_EXT:
//		if (m_extDrawBuffers)
//			return WebGLGetInfo(getMaxDrawBuffers());
//		synthesizeGLError(INVALID_ENUM, "getParameter", "invalid parameter name, EXT_draw_buffers not enabled");
//		return WebGLGetInfo();
	default:
//		if (m_extDrawBuffers
//			&& pname >= Extensions3D::DRAW_BUFFER0_EXT
//			&& pname < static_cast<GC3Denum>(Extensions3D::DRAW_BUFFER0_EXT + getMaxDrawBuffers())) {
//			GLint value = NONE;
//			if (m_framebufferBinding)
//				value = m_framebufferBinding->getDrawBuffer(pname);
//			else // emulated backbuffer
//				value = m_backDrawBuffer;
//			return WebGLGetInfo(value);
//		}
//		synthesizeGLError(INVALID_ENUM, "getParameter", "invalid parameter name");
//		return WebGLGetInfo();
		break;
	}
    args.GetReturnValue().Set(v8::ThrowException(String::New("Unknown parameter")));
}
JS_METHOD(getBufferParameter) {
    HandleScope scope;
	GLenum target = args[0]->Int32Value();
	GLenum pname = args[1]->Int32Value();

	GLint params;
	glGetBufferParameteriv(target,pname,&params);
	args.GetReturnValue().Set(JS_INT(params));
}
JS_METHOD(getError) {
    HandleScope scope;
    args.GetReturnValue().Set(Uint32::New(glGetError()));
}
JS_METHOD(getFramebufferAttachmentParameter) {
    HandleScope scope;
	GLenum target = args[0]->Int32Value();
	GLenum attachment = args[1]->Int32Value();
	GLenum pname = args[2]->Int32Value();

	GLint params;
	glGetFramebufferAttachmentParameteriv(target,attachment, pname,&params);
	args.GetReturnValue().Set(JS_INT(params));
}
JS_METHOD(getProgramParameter) {
    HandleScope scope;
	int program = args[0]->Int32Value();
	int pname = args[1]->Int32Value();

	int value = 0;
	switch (pname) {
		case GL_DELETE_STATUS:
		case GL_LINK_STATUS:
		case GL_VALIDATE_STATUS:
			glGetProgramiv(program, pname, &value);
			args.GetReturnValue().Set(JS_BOOL(static_cast<bool>(value!=0)));
            break;
		case GL_ATTACHED_SHADERS:
		case GL_ACTIVE_ATTRIBUTES:
		case GL_ACTIVE_UNIFORMS:
			glGetProgramiv(program, pname, &value);
			args.GetReturnValue().Set(JS_INT(value));// return number of parameters
            break;
		default:
			args.GetReturnValue().Set(v8::ThrowException(v8::Exception::TypeError(String::New("GetProgramParameter: Invalid Enum"))));
	}
}
JS_METHOD(getProgramInfoLog) {
	HandleScope scope;

	GLuint program = args[0]->Int32Value();
	int Len = 1024;
	char Error[1024];
	glGetProgramInfoLog(program, 1024, &Len, Error);

	args.GetReturnValue().Set(String::New(Error));
}
JS_METHOD(getRenderbufferParameter) {
	HandleScope scope;

	int target = args[0]->Int32Value();
	int pname = args[1]->Int32Value();
	int value = 0;
	glGetRenderbufferParameteriv(target,pname,&value);

	args.GetReturnValue().Set(JS_INT(value));
}
JS_METHOD(getShaderParameter) {
	HandleScope scope;

	int shader = args[0]->Int32Value();
	int pname = args[1]->Int32Value();
	int value = 0;
	switch (pname) {
		case GL_DELETE_STATUS:
		case GL_COMPILE_STATUS:
			glGetShaderiv(shader, pname, &value);
			//LOGI("getShaderParameter:%d %d", pname, value);
			args.GetReturnValue().Set(JS_BOOL(value!=0?true:false));
            break;
		case GL_SHADER_TYPE:
			glGetShaderiv(shader, pname, &value);
			args.GetReturnValue().Set(JS_INT(value));
            break;
		case GL_INFO_LOG_LENGTH:
		case GL_SHADER_SOURCE_LENGTH:
			glGetShaderiv(shader, pname, &value);
			args.GetReturnValue().Set(JS_INT(value));
            break;
		default:
			args.GetReturnValue().Set(ThrowException(Exception::TypeError(String::New("GetShaderParameter: Invalid Enum"))));
	}
}
JS_METHOD(getShaderInfoLog) {
    HandleScope scope;

    GLuint shader = ARGS_GLuint(args[0]);
	GLint length = -1;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    //LOGE("getShaderInfoLog:%d %d", shader, length);
    if(length <= 0) {
    	args.GetReturnValue().Set(v8::Undefined());
    } else {
    	GLchar* src = new GLchar[length];
    	glGetShaderInfoLog(shader, length, NULL, src);
    	//LOGE("getShaderInfoLog.log:%s", src);
    	Local<String> jslog = String::New(src, length);
    	delete[] src;

    	args.GetReturnValue().Set(jslog);
    }
}
JS_METHOD(getShaderSource) {
    HandleScope scope;

	int shader = args[0]->Int32Value();

	GLint len;
	glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &len);
	GLchar *source = new GLchar[len];
	glGetShaderSource(shader, len, NULL, source);

	Local<String> str = String::New(source);
	delete source;

	args.GetReturnValue().Set(str);
}
JS_METHOD(getTexParameter) {
    HandleScope scope;

	GLenum target = args[0]->Int32Value();
	GLenum pname = args[1]->Int32Value();

	GLint param_value=0;
	glGetTexParameteriv(target, pname, &param_value);

	args.GetReturnValue().Set(Number::New(param_value));
}
JS_METHOD(getUniformLocation) {
    HandleScope scope;

	int program = args[0]->Int32Value();
	String::AsciiValue name(args[1]);

	args.GetReturnValue().Set(JS_INT(glGetUniformLocation(program, *name)));
}
JS_METHOD(getVertexAttrib) {
    HandleScope scope;

	GLuint index = args[0]->Int32Value();
	GLuint pname = args[1]->Int32Value();

	GLint value = 0;

	switch (pname) {
		case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
		case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
			glGetVertexAttribiv(index,pname,&value);
			args.GetReturnValue().Set(JS_BOOL(value!=0));

		case GL_VERTEX_ATTRIB_ARRAY_SIZE:
		case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
		case GL_VERTEX_ATTRIB_ARRAY_TYPE:
			glGetVertexAttribiv(index,pname,&value);
			args.GetReturnValue().Set(JS_INT(value));

		case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
			glGetVertexAttribiv(index,pname,&value);
			args.GetReturnValue().Set(JS_INT(value));
		case GL_CURRENT_VERTEX_ATTRIB: {
			float vextex_attribs[4];
			glGetVertexAttribfv(index,pname,vextex_attribs);
			Local<Array> arr=Array::New(4);
			arr->Set(0,JS_FLOAT(vextex_attribs[0]));
			arr->Set(1,JS_FLOAT(vextex_attribs[1]));
			arr->Set(2,JS_FLOAT(vextex_attribs[2]));
			arr->Set(3,JS_FLOAT(vextex_attribs[3]));
			args.GetReturnValue().Set(arr);
		}
		default:
			args.GetReturnValue().Set(v8::ThrowException(String::New("GetVertexAttrib: Invalid Enum")));
	}

	args.GetReturnValue().Set(v8::Undefined());
}
JS_METHOD(getVertexAttribOffset) {
    HandleScope scope;

	GLuint index = args[0]->Uint32Value();
	GLenum pname = args[1]->Int32Value();
	void *ret = NULL;

	glGetVertexAttribPointerv(index, pname, &ret);
	args.GetReturnValue().Set(JS_INT(ToGLuint(ret)));
}
DELEGATE_TO_GL_N2(hint, glHint, GLenum, GLenum);
DELEGATE_TO_GL_1_BR(isBuffer, glIsBuffer, GLuint);
DELEGATE_TO_GL_1_BR(isEnabled, glIsEnabled, GLenum);
DELEGATE_TO_GL_1_BR(isFramebuffer, glIsFramebuffer, GLuint);
DELEGATE_TO_GL_1_BR(isProgram, glIsProgram, GLuint);
DELEGATE_TO_GL_1_BR(isRenderbuffer, glIsRenderbuffer, GLuint);
DELEGATE_TO_GL_1_BR(isShader, glIsShader, GLuint);
DELEGATE_TO_GL_1_BR(isTexture, glIsTexture, GLuint);
DELEGATE_TO_GL_N1(lineWidth, glLineWidth, GLfloat);
DELEGATE_TO_GL_N1(linkProgram, glLinkProgram, GLuint);
//DELEGATE_TO_GL_N2(pixelStorei, glPixelStorei, GLenum, GLint);
JS_METHOD(pixelStorei) {
    if(args.Length() != 2) {
        ThrowException(String::New("pixelStorei: Invalid arguments count, wanted 2"));
        return;
    }
    GLenum pname = ARGS_GLenum(args[0]);
    GLenum param = ARGS_GLint(args[1]);
    // UNPACK_FLIP_Y_WEBGL - BROWSER_DEFAULT_WEBGL
    if(pname >= 0x9240 && pname <= 0x9244) {
        LOGE("pixelStorei: Invalid pname 0x%x", pname);
        return;
    }
    glPixelStorei(pname, param);
}
DELEGATE_TO_GL_N2(polygonOffset, glPolygonOffset, GLfloat, GLfloat);
JS_METHOD(readPixels) {
    HandleScope scope;
//    if (args.Length() != 6 || !args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsUint32() || !args[3]->IsUint32() || !args[4]->IsUint32() || !args[5]->IsUint32())
//        args.GetReturnValue().Set(ThrowException(String::New("Bad arguments")));
//
//    int x = args[0]->IntegerValue();
//    int y = args[1]->IntegerValue();
//    unsigned width = args[2]->Uint32Value();
//    unsigned height = args[3]->Uint32Value();
//    unsigned format = args[4]->Uint32Value();
//    unsigned type = args[5]->Uint32Value();
//
//    //TODO(nico): should support other formats also
//    if (format == GL_RGB && type == GL_UNSIGNED_BYTE) {
//        unsigned len = width * height * 3;
//        GLubyte *ans = new GLubyte[len];
//        glReadPixels((GLint) x, (GLint) y, (GLsizei) width, (GLsizei) height, (GLenum) format, (GLenum) type, ans);
//
//        Local<Array> res = Array::New(len);
//        for (unsigned i = 0; i < len; ++i) {
//            res->Set(Integer::New(i), Integer::New(ans[i]));
//        }
//
//        delete[] ans;
//        args.GetReturnValue().Set(v8::Undefined());
//    }

    args.GetReturnValue().Set(v8::Undefined());
}
DELEGATE_TO_GL_N4(renderbufferStorage, glRenderbufferStorage, GLenum, GLenum, GLsizei, GLsizei);
DELEGATE_TO_GL_N2(sampleCoverage, glSampleCoverage, GLclampf, GLboolean);
DELEGATE_TO_GL_N4(scissor, glScissor, GLint, GLint, GLsizei, GLsizei);
JS_METHOD(shaderSource) {
    HandleScope scope;

	int id = args[0]->Int32Value();
	String::Utf8Value code(args[1]);

	const char* codes[1];
	codes[0] = *code;
	GLint length = code.length();

	glShaderSource(id, 1, codes, &length);
}
DELEGATE_TO_GL_N3(stencilFunc, glStencilFunc, GLenum, GLint, GLuint);
DELEGATE_TO_GL_N4(stencilFuncSeparate, glStencilFuncSeparate, GLenum, GLenum, GLint, GLuint);
DELEGATE_TO_GL_N1(stencilMask, glStencilMask, GLuint);
DELEGATE_TO_GL_N2(stencilMaskSeparate, glStencilMaskSeparate, GLenum, GLuint);
DELEGATE_TO_GL_N3(stencilOp, glStencilOp, GLenum, GLenum, GLenum);
DELEGATE_TO_GL_N4(stencilOpSeparate, glStencilOpSeparate, GLenum, GLenum, GLenum, GLenum);

/**
 * 9
 @param {Number} target
 @param {Number} level
 @param {Number} internalformat
 @param {Number} width
 @param {Number} height
 @param {Number} border
 @param {Number} format
 @param {Number} type
 @param {ArrayBufferView} pixels
 */
/**
 * 6
 @param {Number} target
 @param {Number} level
 @param {Number} internalformat
 @param {Number} format
 @param {Number} type
 @param {ImageData|HTMLImageElement|HTMLCanvasElement|HTMLVideoElement} pixelsImageCanvasOrVideo or null
 */
JS_METHOD(texImage2D) {
    if(args.Length() == 9) {
        ByteBuffer buf;
        ClassBase* c = internalArg<ClassBase>(args[8]);
        if(c != 0) {
            c->getUnderlying(&buf);
        }
        glTexImage2D(ARGS_GLenum(args[0]),// target
                     ARGS_GLint(args[1]),// level
                     ARGS_GLint(args[2]),// internalformat
                     ARGS_GLsizei(args[3]),// width
                     ARGS_GLsizei(args[4]),// height
                     ARGS_GLint(args[5]),// border
                     ARGS_GLenum(args[6]),// format
                     ARGS_GLenum(args[7]),// type
                     buf.value_ptr());
//                     0);
//        LOGI("target:%x level:%x internal:%x width:%d, height:%d, border:%d format:%x, type:%x ptr:%d",
//             ARGS_GLenum(args[0]),
//             ARGS_GLint(args[1]),
//             ARGS_GLint(args[2]),
//             ARGS_GLsizei(args[3]),
//             ARGS_GLsizei(args[4]),
//             ARGS_GLint(args[5]),
//             ARGS_GLenum(args[6]),
//             ARGS_GLenum(args[7]),
//             c != 0);
        checkGlError("texImage2D native");

    } else if(args.Length() == 6) {
        Image* image = internalArg<Image>(args[5], CLASS_IMAGE);
        if(image == 0) {
            ThrowException(String::New("texImage2D args[5] must be an image"));
            return;
        }
        if(image->isReleased()) {
            ThrowException(String::New("texImage2D args[5] image instance has released"));
            return;
        }
        glTexImage2D(ARGS_GLenum(args[0]),// target
                     ARGS_GLint(args[1]),// level
                     ARGS_GLint(args[2]),// internalformat
                     image->getWidth(),//
                     image->getHeight(),//
                     0,// border
                     ARGS_GLenum(args[3]),// format
                     ARGS_GLenum(args[4]),// type
                     image->getData());
    }
}
/**
 * load texture to GL
 * 3
 @param {Number} target
 @param {Number} level
 @param {ImageData|HTMLImageElement|HTMLCanvasElement|HTMLVideoElement} pixelsImageCanvasOrVideo
 */
/**
 * create a empty texture with the given width/height
 * 5
 @param {Number} target
 @param {Number} level
 @param {Number} width
 @param {Number} height
 @param {ImageData|HTMLImageElement|HTMLCanvasElement|HTMLVideoElement} pixelsImageCanvasOrVideo
 */
JS_METHOD(internalTexImage2D) {
    int length = args.Length();
    if(length != 3 && length != 5) {
        ThrowException(String::New("internalTexImage2D invalide arguments number"));
        return;
    }

    int last = args.Length() - 1;
    Image* image = internalArg<Image>(args[last], CLASS_IMAGE);
    if(image == 0) {
        ThrowException(String::New("internalTexImage2D args[last] must be an image"));
        return;
    }
    if(image->isReleased()) {
        ThrowException(String::New("internalTexImage2D args[last] image instance has released"));
        return;
    }
    if(length == 3) {
        glTexImage2D(ARGS_GLenum(args[0]),// target
                     ARGS_GLint(args[1]),// level
                     image->getInternalFormat(),// internalformat
                     image->getWidth(),//
                     image->getHeight(),//
                     0,// border
                     image->getFormat(),// format
                     image->getType(),// type
                     image->getData());
    } else {
        glTexImage2D(ARGS_GLenum(args[0]),// target
                     ARGS_GLint(args[1]),// level
                     image->getInternalFormat(),// internalformat
                     ARGS_GLint(args[2]),//
                     ARGS_GLint(args[3]),//
                     0,// border
                     image->getFormat(),// format
                     image->getType(),// type
                     0);
    }
}

DELEGATE_TO_GL_N3(texParameterf, glTexParameterf, GLenum, GLenum, GLfloat);
DELEGATE_TO_GL_N3(texParameteri, glTexParameteri, GLenum, GLenum, GLint);
/**
 * 9
 @param {Number} target
 @param {Number} level
 @param {Number} xoffset
 @param {Number} yoffset
 @param {Number} width
 @param {Number} height
 @param {Number} format
 @param {Number} type
 @param {ArrayBufferView} pixels
 */
/**
 * 7
 @param {Number} target 0
 @param {Number} level 1
 @param {Number} xoffset 2
 @param {Number} yoffset 3
 @param {Number} format 4
 @param {Number} type 5
 @param {ImageData|HTMLImageElement|HTMLCanvasElement|HTMLVideoElement} pixelsImageCanvasOrVideo
 */
JS_METHOD(texSubImage2D) {
    if(args.Length() == 9) {
        ByteBuffer buf;
        ClassBase* c = internalArg<ClassBase>(args[8]);
        if(c != 0) {
            c->getUnderlying(&buf);
        }
        glTexSubImage2D(ARGS_GLenum(args[0]),// GLenum target
                     ARGS_GLint(args[1]),// GLint level
                     ARGS_GLint(args[2]),// GLint xoffset
                     ARGS_GLint(args[3]),// GLint yoffset
                     ARGS_GLsizei(args[4]),// GLsizei width
                     ARGS_GLsizei(args[5]),// GLsizei height
                     ARGS_GLenum(args[6]),// GLenum format
                     ARGS_GLenum(args[7]),// GLenum type
                     buf.value_ptr());// const GLvoid *pixels

    } else if(args.Length() == 7) {
        Image* image = internalArg<Image>(args[6], CLASS_IMAGE);
        if(image == 0) {
            ThrowException(String::New("args[6] must be an image"));
            return;
        }
        if(image->isReleased()) {
            ThrowException(String::New("args[6] image instance has released"));
            return;
        }
        glTexSubImage2D(ARGS_GLenum(args[0]),// GLenum target
                        ARGS_GLint(args[1]),// GLint level
                        ARGS_GLint(args[2]),// GLint xoffset
                        ARGS_GLint(args[3]),// GLint yoffset
                        image->getWidth(),// GLsizei width
                        image->getHeight(),// GLsizei height
                        ARGS_GLenum(args[4]),// GLenum format
                        ARGS_GLenum(args[5]),// GLenum type
                        image->getData());// const GLvoid *pixels
    }
}
//DELEGATE_TO_GL_N9(texSubImage2D, glTexSubImage2D, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoidP);
/**
 * load image with it internal format
 *
 @param {Number} target
 @param {Number} level
 @param {Number} xoffset
 @param {Number} yoffset
 @param {ArrayBufferView} pixels
 */
JS_METHOD(internalTexSubImage2D) {
    Image* image = internalArg<Image>(args[4], CLASS_IMAGE);
    if(image == 0) {
        ThrowException(String::New("internalTexSubImage2D args[4] must be an image"));
        return;
    }
    if(image->isReleased()) {
        ThrowException(String::New("internalTexSubImage2D args[4] image instance has released"));
        return;
    }
    glTexSubImage2D(ARGS_GLenum(args[0]),// GLenum target
                    ARGS_GLint(args[1]),// GLint level
                    ARGS_GLint(args[2]),// GLint xoffset
                    ARGS_GLint(args[3]),// GLint yoffset
                    image->getWidth(),// GLsizei width
                    image->getHeight(),// GLsizei height
                    image->getFormat(),// GLenum format
                    image->getType(),// GLenum type
                    image->getData());// const GLvoid *pixels
}

DELEGATE_TO_GL_N2(uniform1f, glUniform1f, GLint, GLfloat);
DELEGATE_TO_GL_N3(uniform2f, glUniform2f, GLint, GLfloat, GLfloat);
DELEGATE_TO_GL_N4(uniform3f, glUniform3f, GLint, GLfloat, GLfloat, GLfloat);
DELEGATE_TO_GL_N5(uniform4f, glUniform4f, GLint, GLfloat, GLfloat, GLfloat, GLfloat);
DELEGATE_TO_GL_N2(uniform1i, glUniform1i, GLint, GLint);
DELEGATE_TO_GL_N3(uniform2i, glUniform2i, GLint, GLint, GLint);
DELEGATE_TO_GL_N4(uniform3i, glUniform3i, GLint, GLint, GLint, GLint);
DELEGATE_TO_GL_N5(uniform4i, glUniform4i, GLint, GLint, GLint, GLint, GLint);

JS_METHOD(uniform1fv) {
    HandleScope scope;
    
    int location = args[0]->Int32Value();
    if(args[1]->IsNumber()) {
        float f = args[1]->NumberValue();
        glUniform1f(location, f);
        return;
    }
    
    ByteBuffer fPtr;
    getArgPtr(&fPtr, CLASS_Float32Array, args[1]);
    glUniform1fv(location, fPtr.typedLength(), fPtr.value_ptr<float>());
}
JS_METHOD(uniform1iv) {
    HandleScope scope;
    
    int location = args[0]->Int32Value();
    if(args[1]->IsNumber()) {
        int32_t f = args[1]->Int32Value();
        glUniform1i(location, f);
        return;
    }

    ByteBuffer fPtr;
    getArgPtr(&fPtr, CLASS_Int32Array, args[1]);
    glUniform1iv(location, fPtr.typedLength(), fPtr.value_ptr<int32_t>());
}

#define GL_UNIFORM(T, checkType, name, unit) \
JS_METHOD(uniform##name) {\
    HandleScope scope;\
\
    int location = args[0]->Int32Value();\
    ByteBuffer fPtr;\
    getArgPtr(&fPtr, checkType, args[1]);\
    glUniform##name(location, fPtr.typedLength() / unit, fPtr.value_ptr<T>());\
}
GL_UNIFORM(float, CLASS_Float32Array, 2fv, 2);
GL_UNIFORM(float, CLASS_Float32Array, 3fv, 3);
GL_UNIFORM(float, CLASS_Float32Array, 4fv, 4);
GL_UNIFORM(int, CLASS_Int32Array, 2iv, 2);
GL_UNIFORM(int, CLASS_Int32Array, 3iv, 3);
GL_UNIFORM(int, CLASS_Int32Array, 4iv, 4);

#define UNIFORM_MATRIX(name, edge, unit) \
JS_METHOD(uniformMatrix##name) {\
    HandleScope scope;\
\
    GLint location = args[0]->Int32Value();\
    GLboolean transpose = args[1]->BooleanValue();\
    ByteBuffer fPtr;\
    getArgPtr(&fPtr, CLASS_Float32Array, args[2]);\
    glUniformMatrix##edge##fv(location, fPtr.typedLength() / unit, transpose, fPtr.value_ptr<float>());\
}
UNIFORM_MATRIX(2fv, 2, 4);
UNIFORM_MATRIX(3fv, 3, 9);
UNIFORM_MATRIX(4fv, 4, 16);
//JS_METHOD(uniformMatrix4fv) {
//    HandleScope scope;
//
//    GLint location = args[0]->Int32Value();
//    GLboolean transpose = args[1]->BooleanValue();
//    ByteBuffer fPtr;
//    getArgPtr(&fPtr, CLASS_Float32Array, args[2]);
//    glUniformMatrix4fv(location, fPtr.typedLength() / 16, transpose, fPtr.value_ptr<float>());
//}

DELEGATE_TO_GL_N1(useProgram, glUseProgram, GLuint);
DELEGATE_TO_GL_N1(validateProgram, glValidateProgram, GLuint);

DELEGATE_TO_GL_N2(vertexAttrib1f, glVertexAttrib1f, GLuint, GLfloat);
DELEGATE_TO_GL_N3(vertexAttrib2f, glVertexAttrib2f, GLuint, GLfloat, GLfloat);
DELEGATE_TO_GL_N4(vertexAttrib3f, glVertexAttrib3f, GLuint, GLfloat, GLfloat, GLfloat);
DELEGATE_TO_GL_N5(vertexAttrib4f, glVertexAttrib4f, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);

#define GL_VERTEX_ATTR(T, name) \
JS_METHOD(vertexAttrib##name) {\
    HandleScope scope;\
\
    int location = args[0]->Int32Value();\
    ByteBuffer fPtr;\
    getArgPtr(&fPtr, CLASS_Float32Array, args[1]);\
    glVertexAttrib##name(location, fPtr.value_ptr<float>());\
}
GL_VERTEX_ATTR(float, 1fv);
GL_VERTEX_ATTR(float, 2fv);
GL_VERTEX_ATTR(float, 3fv);
GL_VERTEX_ATTR(float, 4fv);

JS_METHOD(vertexAttribPointer) {
    HandleScope scope;

	int indx = args[0]->Int32Value();
	int size = args[1]->Int32Value();
	int type = args[2]->Int32Value();
	int normalized = args[3]->BooleanValue();
	int stride = args[4]->Int32Value();

    if (args[5]->IsInt32()) {
        int offset = args[5]->Int32Value();
        glVertexAttribPointer(indx, size, type, normalized, stride, (const GLvoid*)offset);
    } else {
        ClassBase* c = internalArg<ClassBase>(args[5]);
        if(c == 0) {
            ThrowException(String::New("vertexAttribPointer args[5] is not an instance ArrayBuffer"));
            return;
        }
        ByteBuffer buf;
        c->getUnderlying(&buf);
        glVertexAttribPointer(indx, size, type, normalized, stride, (const GLvoid*)buf.value_ptr());
    }
    checkGlError("vertexAttribPointer");
}
DELEGATE_TO_GL_N4(viewport, glViewport, GLint, GLint, GLsizei, GLsizei);

node::node_module_struct* GLBinding::getModule(node::node_module_struct* t) {
	t = Module::getModule(t);
	t->filename = __FILE__;
	t->modname = "node_opengl";
	t->register_func = init;
	return t;
}
