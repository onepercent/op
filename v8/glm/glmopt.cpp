//
//  glmopt.cpp
//  v8
//
//  Created by jie on 13-8-16.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include "glmopt.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>

#include "../core/v8Utils.h"
#include "../core/ClassWrap.h"
#include "matrix.h"
#include "vector.h"

Glm::Glm() {
}

#define $V(size) Vec##size<float>
#define $M(size) Mat##size<float>
#define ALL_FN(name) \
name(2)\
name(3)\
name(4)

#define DOT_VEC(size) \
METHOD_BEGIN(dotVec##size, info) {\
    HandleScope scope;\
    $V(size)* v1 = internalArg<$V(size)>(info[0]);\
    $V(size)* v2 = internalArg<$V(size)>(info[1]);\
    float res = glm::dot(v1->mVec, v2->mVec);\
    info.GetReturnValue().Set(Number::New(res));\
}
ALL_FN(DOT_VEC);

#define MUL_VEC(size) \
METHOD_BEGIN(mulVec##size, info) {\
    HandleScope scope;\
    $V(size)* v1 = internalArg<$V(size)>(info[1]);\
    $V(size)* v2 = internalArg<$V(size)>(info[2]);\
    internalArg<$V(size)>(info[0])->mVec = v1->mVec * v2->mVec;\
}
ALL_FN(MUL_VEC);
//MUL_VEC(2);
//MUL_VEC(4);
//METHOD_BEGIN(mulVec3, info) {
//    HandleScope scope;
//    Vec3<float>* v1 = internalArg<Vec3<float>>(info[1]);
//    Vec3<float>* v2 = internalArg<Vec3<float>>(info[2]);
//    glm::vec3 v = v1->mVec * v2->mVec;
//    internalArg<Vec3<float>>(info[0])->mVec = v1->mVec * v2->mVec;
//}

#define ADD_VEC(size) \
METHOD_BEGIN(addVec##size, info) {\
    HandleScope scope;\
    $V(size)* v1 = internalArg<$V(size)>(info[1]);\
    $V(size)* v2 = internalArg<$V(size)>(info[2]);\
    v1->mVec * v2->mVec;\
    internalArg<$V(size)>(info[0])->mVec = v1->mVec + v2->mVec;\
}
ALL_FN(ADD_VEC);

#define SUB_VEC(size) \
METHOD_BEGIN(subVec##size, info) {\
    HandleScope scope;\
    $V(size)* v1 = internalArg<$V(size)>(info[1]);\
    $V(size)* v2 = internalArg<$V(size)>(info[2]);\
    internalArg<$V(size)>(info[0])->mVec = v1->mVec - v2->mVec;\
}
ALL_FN(SUB_VEC);

// dest v scale
#define SCALE_VEC(size) \
METHOD_BEGIN(scaleVec##size##f, info) {\
    HandleScope scope;\
\
    $V(size)* des = internalArg<$V(size)>(info[0]);\
    float s = classtype::unwrap<float>(info[1]);\
    $V(size)* v = internalArg<$V(size)>(info[2]);\
    glm::vec##size scaleV;\
    for(int i = 0; i < size; i++) {\
        scaleV[i] = s;\
    }\
    des->mVec = v->mVec * scaleV;\
}
ALL_FN(SCALE_VEC);

METHOD_BEGIN(mulMV3, info) {
    HandleScope scope;

    Vector* des = internalArg<Vector>(info[0]);
    Matrix* m = internalArg<Matrix>(info[1]);
    Vector* v = internalArg<Vector>(info[2]);

    des->mVec = glm::swizzle<glm::X, glm::Y, glm::Z>(m->mMatrix * glm::vec4(v->mVec, 1));
}
METHOD_BEGIN(mulMV4, info) {
    HandleScope scope;
    
    Vector4* des = internalArg<Vector4>(info[0]);
    Matrix* m = internalArg<Matrix>(info[1]);
    Vector4* v = internalArg<Vector4>(info[2]);

    des->mVec = m->mMatrix * v->mVec;
}
METHOD_BEGIN(inverse, info) {
    HandleScope scope;
    
    Matrix* res = internalArg<Matrix>(info[0]);
    Matrix* m = internalArg<Matrix>(info[1]);

    res->mMatrix = glm::inverse(m->mMatrix);
}
METHOD_BEGIN(mulMM, info) {
    HandleScope scope;
    
    Matrix* res = internalArg<Matrix>(info[0]);
    Matrix* m1 = internalArg<Matrix>(info[1]);
    Matrix* m2 = internalArg<Matrix>(info[2]);
    
    res->mMatrix = m1->mMatrix * m2->mMatrix;
}
METHOD_BEGIN(transpose, info) {
    HandleScope scope;
    
    Matrix* res = internalArg<Matrix>(info[0]);
    Matrix* m = internalArg<Matrix>(info[1]);

    res->mMatrix = glm::transpose(m->mMatrix);
}
/**
 * @param m
 * @param Vector
 */
METHOD_BEGIN(setTranslation, info) {
    HandleScope scope;
    
    Matrix* m = internalArg<Matrix>(info[0]);
    Vector* v = internalArg<Vector>(info[1]);

    m->mMatrix[3] = glm::vec4(v->mVec, 1);
}
METHOD_BEGIN(identity, info) {
    HandleScope scope;
    
    Matrix* m = internalArg<Matrix>(info[0]);
    m->mMatrix = glm::mat4(1);
}
/**
 * float fovy, 
 * float aspect, 
 * float zNear, 
 * float zFar
 * 
 * Vector
 */
METHOD_BEGIN(perspective, info) {
    HandleScope scope;
    
    Matrix* m = internalArg<Matrix>(info[0]);
    if(info.Length() == 2) {
        glm::vec4& v4 = internalArg<Vector4>(info[0])->mVec;
        m->mMatrix = glm::perspective(v4.x, v4.y, v4.z, v4.w);
    } else {
        m->mMatrix = glm::perspective(V_2F(1), V_2F(2), V_2F(3), V_2F(4));
    }
}
/**
 * Computes a 4-by-4 othogonal transformation matrix given the left, right,
 * bottom, and top dimensions of the near clipping plane as well as the
 * near and far clipping plane distances.
 * @param {!tdl.fast.Matrix} dst Output matrix.
 * @param {number} left Left side of the near clipping plane viewport.
 * @param {number} right Right side of the near clipping plane viewport.
 * @param {number} bottom Bottom of the near clipping plane viewport.
 * @param {number} top Top of the near clipping plane viewport.
 * @param {number} near The depth (negative z coordinate)
 *     of the near clipping plane.
 * @param {number} far The depth (negative z coordinate)
 *     of the far clipping plane.
 * @return {!tdl.fast.Matrix} The perspective matrix.
 */
METHOD_BEGIN(ortho, info) {
    HandleScope scope;

    Matrix* m = internalArg<Matrix>(info[0]);
    m->mMatrix = glm::ortho(V_2F(1), V_2F(2), V_2F(3), V_2F(4), V_2F(5), V_2F(6));
}
/**
 * Computes a 4-by-4 perspective transformation matrix given the left, right,
 * top, bottom, near and far clipping planes. The arguments define a frustum
 * extending in the negative z direction. The arguments near and far are the
 * distances to the near and far clipping planes. Note that near and far are not
 * z coordinates, but rather they are distances along the negative z-axis. The
 * matrix generated sends the viewing frustum to the unit box. We assume a unit
 * box extending from -1 to 1 in the x and y dimensions and from 0 to 1 in the z
 * dimension.
 * @param {number} left The x coordinate of the left plane of the box.
 * @param {number} right The x coordinate of the right plane of the box.
 * @param {number} top The y coordinate of the right plane of the box.
 * @param {number} bottom The y coordinate of the bottom plane of the box.
 * @param {number} near The negative z coordinate of the near plane of the box.
 * @param {number} far The negative z coordinate of the far plane of the box.
 * @return {!tdl.fast.Matrix} The perspective projection matrix.
 */
METHOD_BEGIN(frustum, info) {
    HandleScope scope;

    Matrix* m = internalArg<Matrix>(info[0]);
    m->mMatrix = glm::frustum(V_2F(1), V_2F(2), V_2F(3), V_2F(4), V_2F(5), V_2F(6));
}
/**
 detail::tVector<T> const & eye,
 detail::tVector<T> const & center,
 detail::tVector<T> const & up
 */
METHOD_BEGIN(lookAt, info) {
    HandleScope scope;
    
    Matrix* m = internalArg<Matrix>(info[0]);
    if(info.Length() == 2) {
        glm::mat3 m3 = internalArg<Matrix3>(info[1])->mMatrix;
        m->mMatrix = glm::lookAt(m3[0], m3[1], m3[2]);
    } else {
        Vector* eye = internalArg<Vector>(info[1]);
        Vector* center = internalArg<Vector>(info[2]);
        Vector* up = internalArg<Vector>(info[3]);
        m->mMatrix = glm::lookAt(eye->mVec, center->mVec, up->mVec);
    }
}
METHOD_BEGIN(translate, info) {
    HandleScope scope;

    Matrix* m = internalArg<Matrix>(info[0]);
    Vector* v = internalArg<Vector>(info[1]);

    m->mMatrix = glm::translate(m->mMatrix, v->mVec);
}
METHOD_BEGIN(rotateX, info) {
    HandleScope scope;
    
    Matrix* m = internalArg<Matrix>(info[0]);
    m->mMatrix = glm::rotate(m->mMatrix, (float)V_2F(1), glm::vec3(1, 0, 0));
}
METHOD_BEGIN(rotateY, info) {
    HandleScope scope;
    
    Matrix* m = internalArg<Matrix>(info[0]);
    m->mMatrix = glm::rotate(m->mMatrix, (float)V_2F(1), glm::vec3(0, 1, 0));
}
METHOD_BEGIN(rotateZ, info) {
    HandleScope scope;
    
    Matrix* m = internalArg<Matrix>(info[0]);
    m->mMatrix = glm::rotate(m->mMatrix, (float)V_2F(1), glm::vec3(0, 0, 1));
}
/**
 detail::tmat4x4<T> const & m,
 T const & angle,
 detail::tVector<T> const & v
 */
METHOD_BEGIN(rotate, info) {
    HandleScope scope;

    Matrix* m = internalArg<Matrix>(info[0]);
    Vector* aix = internalArg<Vector>(info[2]);
    m->mMatrix = glm::rotate(m->mMatrix, (float)V_2F(1), aix->mVec);
}
METHOD_BEGIN(scale, info) {
    HandleScope scope;

    Matrix* m = internalArg<Matrix>(info[0]);
    Vector* v = internalArg<Vector>(info[1]);
    m->mMatrix = glm::scale(m->mMatrix, v->mVec);
}

/**
 * return a matrix only contains translation information
 */
METHOD_BEGIN(translation, info) {
    HandleScope scope;
    
    Matrix* m = internalArg<Matrix>(info[0]);
    Vector* t = internalArg<Vector>(info[1]);
    m->mMatrix = glm::mat4(1);
    m->mMatrix[3] = glm::vec4(t->mVec, 1);
}
/**
 * creating a matrix only contains scaling information
 */
METHOD_BEGIN(scaling, info) {
    HandleScope scope;
    
    Matrix* m = internalArg<Matrix>(info[0]);
    Vector* t = internalArg<Vector>(info[1]);
    m->mMatrix = glm::mat4(1);
    m->mMatrix[0][0] = t->mVec.x;
    m->mMatrix[1][1] = t->mVec.y;
    m->mMatrix[2][2] = t->mVec.z;
}
METHOD_BEGIN(crossVec3, info) {
    HandleScope scope;

    Vector* dest = internalArg<Vector>(info[0]);
    Vector* v1 = internalArg<Vector>(info[1]);
    Vector* v2 = internalArg<Vector>(info[2]);
    dest->mVec = glm::cross(v1->mVec, v2->mVec);
}
METHOD_BEGIN(nodeMatrix, info) {
    HandleScope scope;

    Matrix* m = internalArg<Matrix>(info[0]);
    Vector* position = internalArg<Vector>(info[1]);
    float angle = info[2]->NumberValue();
    Vector* scale = internalArg<Vector>(info[3]);
    Vector* offset = internalArg<Vector>(info[4]);

    m->mMatrix = glm::mat4(1);
    m->mMatrix[3] = glm::vec4(position->mVec, 1);
    m->mMatrix = glm::rotate(m->mMatrix, angle, glm::vec3(0, 0, 1));
    m->mMatrix = glm::scale(m->mMatrix, scale->mVec);
    m->mMatrix = glm::translate(m->mMatrix, offset->mVec);
}

static v8::Local<v8::Function> initClass(v8::Handle<v8::FunctionTemplate>& temp) {
    HandleScope scope;

    Local<ObjectTemplate> obj = temp->PrototypeTemplate();
    EXPOSE_METHOD(obj, dotVec2, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, dotVec3, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, dotVec4, ReadOnly | DontDelete);

    EXPOSE_METHOD(obj, mulVec2, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, mulVec3, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, mulVec4, ReadOnly | DontDelete);

    EXPOSE_METHOD(obj, crossVec3, ReadOnly | DontDelete);

    EXPOSE_METHOD(obj, addVec2, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, addVec3, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, addVec4, ReadOnly | DontDelete);
    
    EXPOSE_METHOD(obj, subVec2, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, subVec3, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, subVec4, ReadOnly | DontDelete);

    EXPOSE_METHOD(obj, scaleVec2f, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, scaleVec3f, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, scaleVec4f, ReadOnly | DontDelete);

    EXPOSE_METHOD(obj, mulMV4, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, mulMV3, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, inverse, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, mulMM, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, setTranslation, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, identity, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, perspective, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, ortho, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, frustum, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, lookAt, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, translate, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, rotateX, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, rotateY, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, rotateZ, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, rotate, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, scale, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, transpose, ReadOnly | DontDelete);

    EXPOSE_METHOD(obj, translation, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, scaling, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, nodeMatrix, ReadOnly | DontDelete);

    return scope.Close(temp->GetFunction());
}
class_struct* Glm::getExportStruct() {
    static class_struct mTemplate = {
        initClass, "glm", CLASS_GLM
    };
    return &mTemplate;
}
ClassType Glm::getClassType() {
    return CLASS_GLM;
}
