//
//  FeatureArray.h
//  v8
//
//  Created by jie on 13-8-22.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__FeatureArray__
#define __v8__FeatureArray__

#include <v8.h>
#include "../core/ClassBase.h"
#include "../core/v8Utils.h"
#include "../core/bytebuffer.h"

namespace globalfn {
    namespace array {
        using namespace v8;

        template <typename T>
        void getter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info);
        template <typename T>
        void setter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info);
        void length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        
        template <typename T>
        void getter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info) {
            HandleScope scope;
            
            ClassBase* c = internalPtr<ClassBase>(info);
            if(c == 0) {
                return;
            }
            ByteBuffer buf;
            c->getUnderlying(&buf);
            if(index >= buf.typedLength()) {
                LOGI("ArrayOutOfBoundException get[] type:%d, length:%d, index:%d", buf.mElement, buf.typedLength(), index);
                return;
            }

            info.GetReturnValue().Set(*buf.value_ptr<T>(index));
        }
        template <typename T>
        void setter(uint32_t index, Local<Value> value, const PropertyCallbackInfo<Value>& info) {
            HandleScope scope;
            info.GetReturnValue().Set(value);
            
            ClassBase* c = internalPtr<ClassBase>(info);
            if(c == 0) {
                return;
            }
            ByteBuffer buf;
            c->getUnderlying(&buf);
            if(index >= buf.typedLength()) {
                LOGI("ArrayOutOfBoundException set[] type:%d, length:%d, index:%d", buf.mElement, buf.typedLength(), index);
                return;
            }

            *(buf.value_ptr<T>(index)) = classtype::unwrap<T>(value);
        }
    }
}

#endif /* defined(__v8__FeatureArray__) */
