//
//  glmopt.h
//  v8
//
//  Created by jie on 13-8-16.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__glmopt__
#define __v8__glmopt__

#include "../core/ClassBase.h"

/**
 * export glm operations
 */
class Glm : public ClassBase {
public:
    Glm();
    static class_struct* getExportStruct();
    virtual ClassType getClassType();
};

#endif /* defined(__v8__glmopt__) */
