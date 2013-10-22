#include "v8Utils.h"

#define TYPE_MAP_IMPL(T, type) \
template<> ClassType getClassType<T>() {\
    return CLASS_##type;\
}

namespace classtype {
	TYPE_MAP_IMPL(int8_t, Int8Array)
	TYPE_MAP_IMPL(uint8_t, Uint8Array)
	TYPE_MAP_IMPL(int16_t, Int16Array)
	TYPE_MAP_IMPL(uint16_t, Uint16Array)
	TYPE_MAP_IMPL(int32_t, Int32Array)
	TYPE_MAP_IMPL(uint32_t, Uint32Array)
	TYPE_MAP_IMPL(float, Float32Array)
	TYPE_MAP_IMPL(double, Float64Array)
}

bool classtype::isTheSameType(ClassType t1, ClassType t2) {
    if(t1 == t2) {
        return true;
    }
    if(t1 == CLASS_Int8Array) {
        return t2 == CLASS_ArrayBuffer;
    } else if(t2 == CLASS_Int8Array) {
        return t1 == CLASS_ArrayBuffer;
    }
    return false;
}
