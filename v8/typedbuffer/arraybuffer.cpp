//
//  ArrayBuffer.cpp
//  v8
//
//  Created by jie on 13-8-20.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "arraybuffer.h"
#include "../core/v8Utils.h"
#include "../core/ClassWrap.h"
#include "../functions/clone.h"

NodeBuffer::NodeBuffer() : mData(0), mLength(0) {
//    LOGI("ArrayBuffer.create");
}
NodeBuffer::NodeBuffer(int length) : mData(0), mLength(0) {
//    LOGI("ArrayBuffer.create");
    if(length > 0) {
        mData = new char[mLength = length];
    }
}
NodeBuffer::~NodeBuffer() {
    if(mLength > 0) {
        delete[] mData;
    }
//    LOGI("ArrayBuffer.~release");
}
void NodeBuffer::init(const v8::FunctionCallbackInfo<Value> &args) {
    if (args.Length() == 1) {
        long size = args[0]->IntegerValue();
        if(size > 0) {
            allocate(size);
        }
    }
}
void NodeBuffer::reset(const v8::FunctionCallbackInfo<Value> &args) {
}
void NodeBuffer::allocate(long size) {
    mData = new char[mLength = size];
    std::fill_n(mData, mLength, 0);
}
bool NodeBuffer::isView(ClassType type) {
    if(type >= CLASS_Int8Array && type <= CLASS_Float64Array) {
        return true;
    }
    return false;
}

static void byteLength(Local<String> property, const PropertyCallbackInfo<Value>& info) {
    ClassBase* t = internalPtr<ClassBase>(info);
    if(t == 0 || t->getClassType() != CLASS_ArrayBuffer) {
        info.GetReturnValue().Set(Integer::New(0));
        return;
    }
    NodeBuffer* p = static_cast<NodeBuffer*>(t);
    info.GetReturnValue().Set(Integer::New(p->mLength));
}
METHOD_BEGIN(isView, info) {
    ClassBase* t = 0;
    if(info.Length() > 0) {
        t = internalArg<ClassBase>(info[0]);
    } else {
        t = internalPtr<ClassBase>(info);
    }
    if(t != 0) {
        ClassType tType = t->getClassType();
        if(NodeBuffer::isView(tType)) {
            info.GetReturnValue().Set(v8::True());
            return;
        }
    }
    info.GetReturnValue().Set(v8::False());
}
METHOD_BEGIN(slice, info) {
    int acount = info.Length();
    if(acount == 0) {
        return;
    }
    
    ClassBase* t = internalPtr<ClassBase>(info);
    if(t == 0 || t->getClassType() != CLASS_ArrayBuffer) {
        return;
    }

    NodeBuffer* thiz = static_cast<NodeBuffer*>(t);
    long start = info[0]->IntegerValue();
    long end = 0;
    if(acount == 1) {
        end = thiz->mLength;
    } else if(acount == 2) {
        end = info[1]->IntegerValue();
    }
    
    if(start < 0) {
        start += thiz->mLength;
    }
    if(end < 0) {
        end += thiz->mLength;
    }
    
    Handle<Object> dest = ClassWrap<NodeBuffer>::newInstance();
    
    if(start < 0 || start >= thiz->mLength || start >= end) {
        info.GetReturnValue().Set(dest);
        return;
    }

    if(end > thiz->mLength) {
        end = thiz->mLength;
    }
    NodeBuffer* bufPtr = internalPtr<NodeBuffer>(dest);
    bufPtr->allocate(end - start);
    bufPtr->writeBytes(0, thiz->value_ptr(start) , end - start);
    info.GetReturnValue().Set(dest);
}
static v8::Local<v8::Function> initClass(v8::Handle<v8::FunctionTemplate>& temp) {
    HandleScope scope;

    Local<ObjectTemplate> obj = temp->PrototypeTemplate();
    obj->SetAccessor(String::New("byteLength"), byteLength);
    EXPOSE_METHOD(obj, slice, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, isView, ReadOnly | DontDelete);
    obj->Set(String::New("clone"), FunctionTemplate::New(ClassWrap<NodeBuffer>::clone));

    return scope.Close(temp->GetFunction());
}
class_struct* NodeBuffer::getExportStruct() {
    static class_struct mTemplate = {
        initClass, "ArrayBuffer", CLASS_ArrayBuffer
    };
    return &mTemplate;
}
ClassType NodeBuffer::getClassType() {
    return NodeBuffer::getExportStruct()->mType;
}

void NodeBuffer::getUnderlying(ByteBuffer* feature) {
    feature->init(mData, mLength, CLASS_ArrayBuffer);
}
void NodeBuffer::onClone(Local<Object> jsCurrent, NodeBuffer& current, Local<Object> jsFrom, const NodeBuffer& from) {
    if(from.mLength == 0) {
        return;
    }
    current.mData = new char[current.mLength = from.mLength];
}
long NodeBuffer::writeBytes(long offset, char* bytes, long length) {
    if(offset < 0 || offset + length > mLength) {
        LOGE("ArrayBuffer write excceed from:%ld write:%ld length:%ld", offset, length, mLength);
        if(offset < 0) {
            offset = 0;
        }
        if(offset + length > mLength) {
            length = mLength - offset;
        }
    }
    memcpy(mData + offset, bytes, length);
    return length;
}
long NodeBuffer::readBytes(long offset, char* dest, long length) {
    if(offset < 0 || offset + length > mLength) {
        LOGE("ArrayBuffer write excceed from:%ld write:%ld length:%ld", offset, length, mLength);
        if(offset < 0) {
            offset = 0;
        }
        if(offset + length > mLength) {
            length = mLength - offset;
        }
    }
    memcpy(dest, mData + offset, length);
    return length;
}
char* NodeBuffer::value_ptr(long offset) {
    return mData + offset;
}
