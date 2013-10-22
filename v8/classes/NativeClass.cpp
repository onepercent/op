//
//  NativeClass.cpp
//  v8
//
//  Created by jie on 13-8-17.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "NativeClass.h"
#include <v8.h>

using namespace v8;

#include "../glm/glmopt.h"
#include "../glm/matrix.h"
#include "../glm/matrix_inl.h"
#include "../glm/vector.h"
#include "../glm/vector_inl.h"
#include "../typedbuffer/typedbuffer.h"
#include "../core/ClassWrap.h"
#include "file.h"
#include "../modules/Image.h"
#include "../modules/Event.h"
#include "gcobserver.h"
#include "Font.h"
#include "TextureAtlas.h"
#include "AutoRelease.h"

#define FN_SEQ(fn) \
fn##_CLASS(EventAccessor);\
\
fn##_CLASS(Glm);\
fn##_CLASS(JSFile);\
\
fn##_CLASS(NodeBuffer);\
fn##_CLASS(TypedBuffer<int8_t>);\
fn##_CLASS(TypedBuffer<uint8_t>);\
fn##_CLASS(TypedBuffer<int16_t>);\
fn##_CLASS(TypedBuffer<uint16_t>);\
fn##_CLASS(TypedBuffer<int32_t>);\
fn##_CLASS(TypedBuffer<uint32_t>);\
fn##_CLASS(TypedBuffer<float>);\
fn##_CLASS(TypedBuffer<double>);\
\
fn##_CLASS_NAME(Vec4<int32_t>, vec4i);\
fn##_CLASS_NAME(Vec3<int32_t>, vec3i);\
fn##_CLASS_NAME(Vec2<int32_t>, vec2i);\
\
fn##_CLASS_NAME(Vec4<int16_t>, vec4s);\
fn##_CLASS_NAME(Vec3<int16_t>, vec3s);\
fn##_CLASS_NAME(Vec2<int16_t>, vec2s);\
\
fn##_CLASS_NAME(Vec4<uint16_t>, vec4us);\
fn##_CLASS_NAME(Vec3<uint16_t>, vec3us);\
fn##_CLASS_NAME(Vec2<uint16_t>, vec2us);\
\
fn##_CLASS_NAME(Vec4<float>, vec4f);\
fn##_CLASS_NAME(Vec3<float>, vec3f);\
fn##_CLASS_NAME(Vec2<float>, vec2f);\
\
fn##_CLASS_NAME(Vec4<uint8_t>, vec4b);\
fn##_CLASS_NAME(Vec3<uint8_t>, vec3b);\
fn##_CLASS_NAME(Vec2<uint8_t>, vec2b);\
\
fn##_CLASS_NAME(Mat2<int32_t>, mat2i);\
fn##_CLASS_NAME(Mat3<int32_t>, mat3i);\
fn##_CLASS_NAME(Mat4<int32_t>, mat4i);\
\
fn##_CLASS_NAME(Mat2<uint8_t>, mat2b);\
fn##_CLASS_NAME(Mat3<uint8_t>, mat3b);\
fn##_CLASS_NAME(Mat4<uint8_t>, mat4b);\
\
fn##_CLASS_NAME(Mat2<float>, mat2f);\
fn##_CLASS_NAME(Mat3<float>, mat3f);\
fn##_CLASS_NAME(Mat4<float>, mat4f);\
\
fn##_CLASS_NAME(Mat4<float>, matrix);\
fn##_CLASS_NAME(Mat4<float>, matrix4);\
fn##_CLASS_NAME(Mat3<float>, matrix3);\
fn##_CLASS_NAME(Mat2<float>, matrix2);\
\
fn##_CLASS_NAME(Vec3<float>, vector);\
fn##_CLASS_NAME(Vec4<float>, vector4);\
fn##_CLASS_NAME(Vec3<float>, vector3);\
fn##_CLASS_NAME(Vec2<float>, vector2);\
\
fn##_CLASS(Font);\
fn##_CLASS(TextureAtlas);\
\
fn##_CLASS(AutoRelease);\
\
fn##_CLASS(GcObserver);\
fn##_CLASS(Image);

#define INIT_CLASS(clz) ClassWrap<clz >::expose(global)
#define INIT_CLASS_NAME(clz, name) ClassWrap<clz >::expose(#name, global)
#define DES_CLASS(clz) ClassWrap<clz >::dispose()
#define DES_CLASS_NAME(clz, name) ClassWrap<clz >::dispose()

static void init(const v8::FunctionCallbackInfo<Value>& args) {
    HandleScope scope;
    Local<Object> global = args[0]->ToObject();

	FN_SEQ(INIT);
}
static void destory() {
	FN_SEQ(DES);
}

node::node_module_struct* NativeClass::getModule(node::node_module_struct* t) {
	t = Module::getModule(t);
	t->filename = __FILE__;
	t->modname = "node_nativeclasses";
	t->register_func = init;
	t->release_func = destory;
	return t;
}
