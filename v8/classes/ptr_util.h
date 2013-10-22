//
//  ptr_util.h
//  v8
//
//  Created by jie on 13-8-18.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef v8_ptr_util_h
#define v8_ptr_util_h

#include "../core/ClassBase.h"
#include "../core/ClassWrap.h"
#include "../core/v8Utils.h"
#include "../core/bytebuffer.h"
#include "../typedbuffer/arraybufferview.h"
#include "../typedbuffer/typedbuffer.h"

static void argValue(const v8::FunctionCallbackInfo<Value> &info, int index, float* slot) {
    *slot = info[index]->NumberValue();
}
static void argValue(const v8::FunctionCallbackInfo<Value> &info, int index, uint8_t* slot) {
    *slot = info[index]->Uint32Value();
}
static void argValue(const v8::FunctionCallbackInfo<Value> &info, int index, int32_t* slot) {
    *slot = info[index]->Int32Value();
}
static void argValue(const v8::FunctionCallbackInfo<Value> &info, int index, int16_t* slot) {
    *slot = info[index]->Int32Value();
}
static void argValue(const v8::FunctionCallbackInfo<Value> &info, int index, uint16_t* slot) {
    *slot = info[index]->Uint32Value();
}

/**
 * create a data structor for init the object
 */
template <typename T>
static void flatVector(const v8::FunctionCallbackInfo<Value> &info, T* values, int length) {
    int copyed = 0;

    int alen = info.Length();
    for(int i = 0; i < alen; i++) {
        if(copyed == length) {
            break;
        }

        Local<Value> infoV = info[i];
        if(info[i].IsEmpty() || infoV->IsUndefined()) {
            continue;
        }

        if(info[i]->IsArray()) {
            Handle<Array> array = Handle<Array>::Cast(info[i]);
            int arrayPop = populateValues<T>(values + copyed, array, length - copyed);
            copyed += arrayPop;
            continue;
        }

        ClassBase* p = internalArg<ClassBase>(info[i]);
        if(p == 0) {
            if(infoV->IsNumber()) {
                argValue(info, i, values + copyed);
                copyed++;
            }
        } else {
            ByteBuffer fPtr;
            p->getUnderlying(&fPtr);
            int plen = fPtr.typedLength();

            // memory copy or value conver one by one
            int maxCp = plen > length - copyed ? length - copyed : plen;
            ClassType vType = classtype::getClassType<T>();
            if(classtype::isTheSameType(vType, fPtr.mElement)) {
                memcpy(values + copyed, fPtr.value_ptr(), sizeof(T) * maxCp);
            } else {
                populateValues(values + copyed, fPtr.value_ptr(), fPtr.mElement, maxCp);
            }
            copyed += maxCp;
        }
    }

    while (copyed < length) {
        values[copyed++] = 0;
    }
}

/**
 * translate between vector/matirx and ArrayBuffer, return a data structor of the curent object
 *
 * @param eUnit sizeof field
 */
static void _valueFn(const v8::FunctionCallbackInfo<Value>& args, int eUnit, char* mPtr, int eSize) {
    while (1) {
        if(args.Length() == 0) {
            break;
        }
        ClassBase* destPtr = internalArg<ClassBase>(args[0]);
        if(destPtr == 0) {
            break;
        }

        ClassType type = destPtr->getClassType();
        if(type == CLASS_ArrayBuffer) {
            NodeBuffer* bufPtr = static_cast<NodeBuffer*>(destPtr);
            bufPtr->_writeDatas(0, eUnit, mPtr, eSize);
            
            args.GetReturnValue().Set(args[0]);
            return;
            
        } else if(NodeBuffer::isView(type)) {
            // copy value dispite buffer type.
            NodeBufferView* viewPtr = static_cast<NodeBufferView*>(destPtr);
            viewPtr->writeBytes(0, mPtr, eSize * eUnit);
            
            args.GetReturnValue().Set(args[0]);
            return;
            
        } else {
            args.GetReturnValue().Set(ThrowException(String::New("_value args[0] cannot be treated as a buffer obejct.")));
        }
        break;
    }

    Handle<Object> byteArray = ClassWrap<NodeBuffer>::newInstance(Integer::NewFromUnsigned(eSize * eUnit));
    NodeBuffer* bPtr = internalPtr<NodeBuffer>(byteArray);
    bPtr->_writeDatas(0, eUnit, mPtr, eSize);
    args.GetReturnValue().Set(ClassWrap<TypedBuffer<float> >::newInstance(byteArray));
}

template <typename T>
void fill_value_ptr(T* dest, T* from, int size) {
    for(int i = 0;i < size; i++) {
        dest[i] = from[i];
    }
}

static int int2str(char* dest, int value) {
    return sprintf(dest, "%d", value);
}
static int float2str(char* dest, float value) {
    return sprintf(dest, "%f", value);
}

#define PTR_TOSTRING(T, fn) \
static const char* printValue(const char* name, T* ptr, int length, int step=0) {\
    char us[30];\
    std::string buf;\
    buf.append("[");\
    buf.append(name);\
    buf.append("{");\
    if(step != 0) {\
        buf.append("\n");\
    }\
    int plen = 0;\
    if(length > 0) {\
        plen = fn(us, ptr[0]);\
        buf.append(us, plen);\
        for (int i = 1; i < length; i++) {\
            buf.append(", ");\
            if(step > 0 && (i % step) == 0) {\
                buf.append("\n");\
            }\
            plen = fn(us, ptr[i]);\
            buf.append(us, plen);\
        }\
    }\
    buf.append("}]");\
    return buf.c_str();\
}

//static const char* printValue(const char* name, float* ptr, int length, int step=0) {\
//    std::string buf;
//    buf.append("[");
//    buf.append(name);
//    buf.append("{");
//    if(step != 0) {
//        buf.append("\n");
//    }
//    if(length > 0) {
//        buf.append(float2str(ptr[0]));
//        for (int i = 1; i < length; i++) {
//            buf.append(", ");
//            if(step > 0 && (i % step) == 0) {
//                buf.append("\n");
//            }
//            buf.append(float2str(ptr[i]));
//        }
//    }
//    buf.append("}]");
//    return buf.c_str();
//}

PTR_TOSTRING(int32_t, int2str);
PTR_TOSTRING(float, float2str);
PTR_TOSTRING(uint8_t, int2str);
PTR_TOSTRING(int16_t, int2str);
PTR_TOSTRING(uint16_t, int2str);

//namespace bytetools {
//    void move(char* dest, char* from, long length);
//}

#endif
