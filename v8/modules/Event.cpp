//
//  Event.cpp
//  v8
//
//  Created by jie on 13-9-10.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include "Event.h"
#include "../core/v8Utils.h"
#include "../typedbuffer/arraybuffer.h"

METHOD_BEGIN(getEvent, info) {
    HandleScope scope;
    if(info.Length() == 0) {
        return;
    }
    EventAccessor* event = internalPtr<EventAccessor>(info, CLASS_EVENT);
    if(event == 0) {
        return;
    }

    // dest buffer
    ClassBase* ptr = internalArg<ClassBase>(info[0]);
    if(ptr == 0) {
        ThrowException(String::New("EventAccessor.getEvent's arguments[0] is not typeof TypedArray or Vector"));
        return;
    }
    ByteBuffer buf;
    ptr->getUnderlying(&buf);

    // copy data to info[0], return event count remain
    DataRange* range = event->mStruct->startRead();
    int remain = range->readOne(buf.value_ptr());
    range->end();

    info.GetReturnValue().Set(remain);
}
METHOD_BEGIN(getEvents, info) {
    HandleScope scope;
    if(info.Length() == 0) {
        return;
    }
    EventAccessor* event = internalPtr<EventAccessor>(info, CLASS_EVENT);
    if(event == 0) {
        return;
    }

    // dest buffer
    ClassBase* ptr = internalArg<ClassBase>(info[0]);
    if(ptr == 0 || !NodeBuffer::isView(ptr->getClassType())) {
        ThrowException(String::New("EventAccessor.getEvent's arguments[0] is not typeof TypedArray"));
        return;
    }
    ByteBuffer buf;
    ptr->getUnderlying(&buf);

    // copy data to info[0], return event count remain
    DataRange* range = event->mStruct->startRead();
    int readed = range->read(buf.value_ptr(), (int)buf.length());
    range->end();

    info.GetReturnValue().Set(readed);
}
static v8::Local<v8::Function> initClass(v8::Handle<v8::FunctionTemplate>& temp) {
    HandleScope scope;

    Local<ObjectTemplate> obj = temp->PrototypeTemplate();
    EXPOSE_METHOD(obj, getEvent, ReadOnly | DontDelete);
    EXPOSE_METHOD(obj, getEvents, ReadOnly | DontDelete);

    return scope.Close(temp->GetFunction());
}

EventAccessor::EventAccessor() : mStruct(0) {
}
EventAccessor::~EventAccessor() {
    if(mStruct != 0) {
        delete mStruct;
    }
}
void EventAccessor::init(const v8::FunctionCallbackInfo<Value> &args) {
    if(args.Length() != 2) {
        ThrowException(String::New("EventAccessor.init arguments number error, 2 expected"));
    }
    int stride = args[0]->Uint32Value();
    int count = args[1]->Uint32Value();
    mStruct = new RingBuffer(stride, count);
}
class_struct* EventAccessor::getExportStruct() {
    static class_struct mTemplate = {
        initClass, "EventAccess", CLASS_EVENT
    };
    return &mTemplate;
}
ClassType EventAccessor::getClassType() {
    return getExportStruct()->mType;
}

TouchEvent::TouchEvent(Handle<Object> obj) : JSObject(obj) {
    EventAccessor* acc = internalPtr<EventAccessor>(obj, CLASS_EVENT);
    if(acc == 0) {
        LOGE("TouchEvent init error, object is not instance of EventAccessor");
        return;
    }
    mStruct = acc->mStruct;
}
TouchEvent::~TouchEvent() {
    mStruct = 0;
}
int TouchEvent::appendMouseTouch(int button, int state, int x, int y) {
    DataRange* target = mStruct->startWrite();
    if(target->isEmpty()) {
        return -1;
    }

    int* dest = target->value_ptr<int>();
    dest[0] = button;
    dest[1] = state;
    dest[2] = x;
    dest[3] = y;
    int remain = target->next();
    target->end();
    return remain;
}
int TouchEvent::appendMouseMove(int x, int y) {
    DataRange* target =mStruct->startWrite();
    if(target->isEmpty()) {
        return -1;
    }

    int* dest = target->value_ptr<int>();
    dest[0] = -1;
    dest[1] = 2;// move
    dest[2] = x;
    dest[3] = y;
    int remain = target->next();
    target->end();
    return remain;
}
int TouchEvent::appendKeyPress(unsigned char key, int x, int y) {
    DataRange* target = mStruct->startWrite();
    if(target->isEmpty()) {
        return -1;
    }

    int* dest = target->value_ptr<int>();
    dest[0] = key;
    dest[1] = x;
    dest[2] = y;
    int remain = target->next();
    target->end();
    return remain;
}
