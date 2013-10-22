//
//  Vec4.h
//  v8
//
//  Created by jie on 13-8-15.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__Vec4__
#define __v8__Vec4__

#include "../core/ClassBase.h"
#include "../core/v8Utils.h"
#include <glm/glm.hpp>

#define VECTOR_DECLEAR(clzName, size)\
template <typename T>\
class clzName : public ClassBase {\
public:\
    clzName();\
    virtual ~clzName();\
    static class_struct* getExportStruct();\
    virtual ClassType getClassType();\
    virtual const char* toString();\
    virtual void getUnderlying(ByteBuffer* feature);\
    virtual void init(const v8::FunctionCallbackInfo<Value> &info);\
    virtual void setValue(const v8::FunctionCallbackInfo<Value> &info);\
\
    glm::detail::tvec##size<T> mVec;\
}

VECTOR_DECLEAR(Vec2, 2);
VECTOR_DECLEAR(Vec3, 3);
VECTOR_DECLEAR(Vec4, 4);

typedef Vec3<float> Vector;
typedef Vec2<float> Vector2;
typedef Vec3<float> Vector3;
typedef Vec4<float> Vector4;

#endif /* defined(__v8__Vec4__) */
