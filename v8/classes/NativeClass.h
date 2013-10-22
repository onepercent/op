//
//  NativeClass.h
//  v8
//
//  Created by jie on 13-8-17.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__NativeClass__
#define __v8__NativeClass__

#include "../core/Module.h"

class NativeClass : public Module {
public:
	static node::node_module_struct* getModule(node::node_module_struct* t);
};

#endif /* defined(__v8__NativeClass__) */
