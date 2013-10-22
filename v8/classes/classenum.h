//
//  classenum.h
//  v8
//
//  Created by jie on 13-8-15.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef v8_classenum_h
#define v8_classenum_h

enum ClassType {
    CLASS_NULL = 0,
    CLASS_GC,
    CLASS_GLM,
    CLASS_POINT,
    CLASS_MATRIX2,
    CLASS_MATRIX3,
    CLASS_MATRIX4,
    CLASS_VEC2,
    CLASS_VEC3,
    CLASS_VEC4,
    CLASS_FILE,
    CLASS_IMAGE,
    CLASS_EVENT,

    CLASS_ArrayBuffer,// byte
    CLASS_Int8Array,
    CLASS_Uint8Array,
    CLASS_Int16Array,
    CLASS_Uint16Array,
    CLASS_Int32Array,
    CLASS_Uint32Array,
    CLASS_Float32Array,
    CLASS_Float64Array,

    CLASS_Font,
    CLASS_Atlas,
    
    CLASS_AutoRelease// auto release
};

#endif
