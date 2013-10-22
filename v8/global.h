//
//  global.h
//  v8
//
//  Created by jie on 13-8-11.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef v8_global_h
#define v8_global_h

#include <string>
#include <v8.h>

extern std::string source_root;

#define  LOGD(...)  printf(__VA_ARGS__);printf("\n")
#define  LOGI(...)  printf(__VA_ARGS__);printf("\n")
#define  LOGE(...)  printf(__VA_ARGS__);printf("\n")

#define NS_NODE_BEGIN namespace node {
#define NS_NODE_END }

#define CC_PROPERTY_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);

#define CC_PROPERTY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);\
public: virtual void set##funName(varType var);

#define CC_SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }

#define CC_BREAK_IF(cond) if(cond) break

#define CC_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = 0; } } while(0)

#define CC_SAFE_FREE(p)                do { if(p) { free(p); (p) = 0; } } while(0)

#define glDepthRangef glDepthRange
#define glClearDepthf glClearDepth

#define OS_MAC

namespace global {
    void testValue(v8::Handle<v8::Value> value);
}

#endif
