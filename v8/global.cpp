//
//  global.h
//  v8
//
//  Created by jie on 13-8-11.
//  Copyright (c) 2013年 jie. All rights reserved.
//
#include "global.h"

void global::testValue(v8::Handle<v8::Value> value) {
    LOGI("value.IsEmpty %d", value.IsEmpty());
    LOGI("value.IsArray %d", value->IsArray());
    LOGI("value.IsUndefined %d", value->IsUndefined());
    LOGI("value.IsNull %d", value->IsNull());
    LOGI("value.IsTrue %d", value->IsTrue());
    LOGI("value.IsFalse %d", value->IsFalse());
    LOGI("value.IsString %d", value->IsString());
    LOGI("value.IsSymbol %d", value->IsSymbol());
    LOGI("value.IsFunction %d", value->IsFunction());
    LOGI("value.IsObject %d", value->IsObject());
    LOGI("value.IsBoolean %d", value->IsBoolean());
    LOGI("value.IsNumber %d", value->IsNumber());
    LOGI("value.IsExternal %d", value->IsExternal());
    LOGI("value.IsInt32 %d", value->IsInt32());
    LOGI("value.IsUint32 %d", value->IsUint32());
    LOGI("value.IsDate %d", value->IsDate());
    LOGI("value.IsBooleanObject %d", value->IsBooleanObject());
    LOGI("value.IsNumberObject %d", value->IsNumberObject());
    LOGI("value.IsStringObject %d", value->IsStringObject());
    LOGI("value.IsSymbolObject %d", value->IsSymbolObject());
    LOGI("value.IsNativeError %d", value->IsNativeError());
    LOGI("value.IsRegExp %d", value->IsRegExp());
    LOGI("value.IsArrayBuffer %d", value->IsArrayBuffer());
    LOGI("value.IsArrayBufferView %d", value->IsArrayBufferView());
    LOGI("value.IsTypedArray %d", value->IsTypedArray());
    LOGI("value.IsUint8Array %d", value->IsUint8Array());
    LOGI("value.IsUint8ClampedArray %d", value->IsUint8ClampedArray());
    LOGI("value.IsInt8Array %d", value-> IsInt8Array());
    LOGI("value.IsUint16Array %d", value->IsUint16Array());
    LOGI("value.IsInt16Array %d", value->IsInt16Array());
    LOGI("value.IsUint32Array %d", value->IsUint32Array());
    LOGI("value.IsInt32Array %d", value->IsInt32Array());
    LOGI("value.IsFloat32Array %d", value->IsFloat32Array());
    LOGI("value.IsFloat64Array %d", value->IsFloat64Array());
    LOGI("value.IsDataView %d", value->IsDataView());
}
