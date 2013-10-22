#ifndef __v8__Vec4_inl__
#define __v8__Vec4_inl__

#include "../classes/ptr_util.h"

#define VERTEX_UNDERLYING(clzName, T, fType, size)\
template<> void clzName<T>::getUnderlying(ByteBuffer* feature) {\
    feature->mPtr = (char*)glm::value_ptr(mVec);\
    feature->mByteLength = size * sizeof(T);\
    feature->mElement = fType;\
    feature->mElementSize = sizeof(T);\
}

#define VERTEX_IMPL(clzName, size) \
template <typename T>\
ClassType clzName<T>::getClassType() {\
    return getExportStruct()->mType;\
}\
template <typename T>\
const char* clzName<T>::toString() {\
    return printValue(#clzName, glm::value_ptr(mVec), size);\
}\
template <typename T>\
void clzName<T>::init(const v8::FunctionCallbackInfo<Value> &info) {\
    HandleScope scope;\
    if(info.Length() == 0) {\
        return;\
    }\
    T values[size];\
    flatVector<T>(info, values, size);\
    fill_value_ptr<T>(glm::value_ptr(mVec), values, size);\
}\
template <typename T>\
void clzName<T>::setValue(const v8::FunctionCallbackInfo<Value> &info) {\
    T values[size];\
    flatVector<T>(info, values, size);\
    fill_value_ptr<T>(glm::value_ptr(mVec), values, size);\
}\
VERTEX_UNDERLYING(clzName, float, CLASS_Float32Array, size)\
VERTEX_UNDERLYING(clzName, int32_t, CLASS_Int32Array, size)\
VERTEX_UNDERLYING(clzName, uint8_t, CLASS_Uint8Array, size)\
VERTEX_UNDERLYING(clzName, int16_t, CLASS_Uint8Array, size)\
VERTEX_UNDERLYING(clzName, uint16_t, CLASS_Uint8Array, size)

namespace glm_vector {
    /**
     * init current object with Array or ArrayBufferView|TypedBuffer
     */
    template <class M>
    void set(const v8::FunctionCallbackInfo<Value> &info);
    template <typename T>
    void cross(const v8::FunctionCallbackInfo<Value> &info);
    template <typename T>
    void scaleVec2(const v8::FunctionCallbackInfo<Value> &info);
    template <typename T>
    void scaleVec3(const v8::FunctionCallbackInfo<Value> &info);
    template <typename T>
    void scaleVec4(const v8::FunctionCallbackInfo<Value> &info);
    template <class M, typename T>
    void mul(const v8::FunctionCallbackInfo<Value> &info);
    template <class M, typename T>
    void add(const v8::FunctionCallbackInfo<Value> &info);
    template <class M, typename T>
    void sub(const v8::FunctionCallbackInfo<Value> &info);
    template <class M, typename T>
    static v8::Local<v8::Function> initVector2(v8::Handle<v8::FunctionTemplate>& temp);
    template <class M, typename T>
    static v8::Local<v8::Function> initVector3(v8::Handle<v8::FunctionTemplate>& temp);
    template <class M, typename T>
    static v8::Local<v8::Function> initVector4(v8::Handle<v8::FunctionTemplate>& temp);
}

VERTEX_IMPL(Vec2, 2);
VERTEX_IMPL(Vec3, 3);
VERTEX_IMPL(Vec4, 4);

template <typename T>
class_struct* Vec2<T>::getExportStruct() {
    static class_struct mTemplate = {
        glm_vector::initVector2<Vec2<T>, T>, "vec2", CLASS_VEC2
    };
    return &mTemplate;
}
// vector3 has different constructor
template <typename T>
class_struct* Vec3<T>::getExportStruct() {
    static class_struct mTemplate = {
        glm_vector::initVector3<Vec3<T>, T>, "vec3", CLASS_VEC3
    };
    return &mTemplate;
}
template <typename T>
class_struct* Vec4<T>::getExportStruct() {
    static class_struct mTemplate = {
        glm_vector::initVector4<Vec4<T>, T>, "vec4", CLASS_VEC4
    };
    return &mTemplate;
}

template <typename T>
Vec2<T>::Vec2() : mVec(0,0) {
}
template <typename T>
Vec3<T>::Vec3() : mVec(0,0,0) {
}
template <typename T>
Vec4<T>::Vec4() : mVec(0,0,0,0) {
}
template <typename T>
Vec2<T>::~Vec2() {
//    LOGI("~Vec2");
}
template <typename T>
Vec3<T>::~Vec3() {
//    LOGI("~Vec3");
}
template <typename T>
Vec4<T>::~Vec4() {
//    LOGI("~Vec4");
}

/**
 * init current object with Array or ArrayBufferView|TypedBuffer
 */
template <class M>
void glm_vector::set(const v8::FunctionCallbackInfo<Value> &info) {
    HandleScope scope;
    ClassBase* c = internalPtr<ClassBase>(info, M::getExportStruct()->mType);
    if(c == 0) {
        return;
    }
    M* thiz = static_cast<M*>(c);
    thiz->setValue(info);
}
template <class M, typename T>
void glm_vector::mul(const v8::FunctionCallbackInfo<Value> &info) {
    HandleScope scope;

    M* thiz = internalPtr<M>(info, M::getExportStruct()->mType);
    if(thiz == 0) {
        return;
    }
    M* param = internalArg<M>(info[0], M::getExportStruct()->mType);
    if(param == 0) {
        return;
    }
    thiz->mVec = thiz->mVec * param->mVec;
}
template <typename T>
void glm_vector::cross(const v8::FunctionCallbackInfo<Value> &info) {
    HandleScope scope;
    
    Vec3<T>* thiz = internalPtr<Vec3<T> >(info, Vec3<T>::getExportStruct()->mType);
    if(thiz == 0) {
        return;
    }
    Vec3<T>* param = internalArg<Vec3<T> >(info[0], Vec3<T>::getExportStruct()->mType);
    if(param == 0) {
        return;
    }
    thiz->mVec = glm::cross(thiz->mVec, param->mVec);
}

#define DUPLICATE_2(name) name,name
#define DUPLICATE_3(name) name,name,name
#define DUPLICATE_4(name) name,name,name,name
#define SCALE_VEC(size) \
template <typename T>\
void glm_vector::scaleVec##size(const v8::FunctionCallbackInfo<Value> &info) {\
    HandleScope scope;\
    Vec##size<T>* thiz = internalPtr<Vec##size<T> >(info, Vec3<T>::getExportStruct()->mType);\
    if(thiz == 0) {\
        return;\
    }\
    T scale = classtype::unwrap<T>(info[0]);\
    glm::detail::tvec##size<T> sVec(DUPLICATE_##size(scale));\
    thiz->mVec = thiz->mVec * sVec;\
}
SCALE_VEC(2);
SCALE_VEC(3);
SCALE_VEC(4);

template <class M, typename T>
void glm_vector::add(const v8::FunctionCallbackInfo<Value> &info) {
    HandleScope scope;
    
    M* thiz = internalPtr<M>(info, M::getExportStruct()->mType);
    if(thiz == 0) {
        return;
    }
    M* param = internalArg<M>(info[0], M::getExportStruct()->mType);
    if(param == 0) {
        return;
    }
    thiz->mVec = thiz->mVec + param->mVec;
}
template <class M, typename T>
void glm_vector::sub(const v8::FunctionCallbackInfo<Value> &info) {
    HandleScope scope;

    M* thiz = internalPtr<M>(info, M::getExportStruct()->mType);
    if(thiz == 0) {
        return;
    }
    M* param = internalArg<M>(info[0], M::getExportStruct()->mType);
    if(param == 0) {
        return;
    }
    thiz->mVec = thiz->mVec - param->mVec;
}
template <class M, typename T>
static v8::Local<v8::Function> glm_vector::initVector2(v8::Handle<v8::FunctionTemplate>& temp) {
    HandleScope scope;
    
    Local<ObjectTemplate> obj = temp->PrototypeTemplate();
    obj->SetAccessor(String::New("length"), globalfn::array::length);
    EXPOSE_METHOD_NAME(obj, set, set<M>, ReadOnly | DontDelete);
    obj->Set(String::New("add"), FunctionTemplate::New(add<M, T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("sub"), FunctionTemplate::New(sub<M, T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("mul"), FunctionTemplate::New(mul<M, T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("scale"), FunctionTemplate::New(scaleVec2<T>), PropertyAttribute(ReadOnly | DontDelete));

    Local<ObjectTemplate> ins = temp->InstanceTemplate();
    ins->SetIndexedPropertyHandler(globalfn::array::getter<T>, globalfn::array::setter<T>);
    
    return scope.Close(temp->GetFunction());
}
template <class M, typename T>
static v8::Local<v8::Function> glm_vector::initVector3(v8::Handle<v8::FunctionTemplate>& temp) {
    HandleScope scope;

    Local<ObjectTemplate> obj = temp->PrototypeTemplate();
    obj->SetAccessor(String::New("length"), globalfn::array::length);
    EXPOSE_METHOD_NAME(obj, set, set<M>, ReadOnly | DontDelete);
    obj->Set(String::New("add"), FunctionTemplate::New(add<M, T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("sub"), FunctionTemplate::New(sub<M, T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("mul"), FunctionTemplate::New(mul<M, T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("scale"), FunctionTemplate::New(scaleVec3<T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("cross"), FunctionTemplate::New(cross<T>), PropertyAttribute(ReadOnly | DontDelete));// only vector3 can cross

    Local<ObjectTemplate> ins = temp->InstanceTemplate();
    ins->SetIndexedPropertyHandler(globalfn::array::getter<T>, globalfn::array::setter<T>);
    
    return scope.Close(temp->GetFunction());
}
template <class M, typename T>
static v8::Local<v8::Function> glm_vector::initVector4(v8::Handle<v8::FunctionTemplate>& temp) {
    HandleScope scope;
    
    Local<ObjectTemplate> obj = temp->PrototypeTemplate();
    obj->SetAccessor(String::New("length"), globalfn::array::length);
    EXPOSE_METHOD_NAME(obj, set, set<M>, ReadOnly | DontDelete);
    obj->Set(String::New("add"), FunctionTemplate::New(add<M, T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("sub"), FunctionTemplate::New(sub<M, T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("mul"), FunctionTemplate::New(mul<M, T>), PropertyAttribute(ReadOnly | DontDelete));
    obj->Set(String::New("scale"), FunctionTemplate::New(scaleVec4<T>), PropertyAttribute(ReadOnly | DontDelete));

    Local<ObjectTemplate> ins = temp->InstanceTemplate();
    ins->SetIndexedPropertyHandler(globalfn::array::getter<T>, globalfn::array::setter<T>);
    
    return scope.Close(temp->GetFunction());
}

#endif /* defined(__v8__Vec4_inl__) */
