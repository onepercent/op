//
//  matrix.h
//  v8
//
//  Created by jie on 13-8-18.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef v8_matrix11_h
#define v8_matrix11_h

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../core/ClassBase.h"
#include "../core/sturctures.h"

#include <v8.h>
using namespace v8;

class ByteBuffer;

#define MATRIX_DECLEAR(clzName, size) \
template<typename T>\
class clzName : public ClassBase {\
public:\
    clzName();\
    virtual ~clzName();\
\
    static class_struct* getExportStruct();\
    virtual ClassType getClassType();\
    virtual const char* toString();\
    virtual void getUnderlying(ByteBuffer* feature);\
    virtual void init(const v8::FunctionCallbackInfo<v8::Value> &args);\
    virtual void setValue(const v8::FunctionCallbackInfo<Value> &info);\
\
    glm::detail::tmat##size##x##size<T> mMatrix;\
}

MATRIX_DECLEAR(Mat2, 2);
MATRIX_DECLEAR(Mat3, 3);
MATRIX_DECLEAR(Mat4, 4);

typedef Mat4<float> Matrix;
typedef Mat2<float> Matrix2;
typedef Mat3<float> Matrix3;
typedef Mat4<float> Matrix4;

#endif
