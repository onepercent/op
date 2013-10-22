//
//  Module.h
//  v8
//
//  Created by jie on 13-8-14.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef v8_Module_h
#define v8_Module_h

#include "../app/node.h"
#include <v8.h>

class Module {
public:
    static node::node_module_struct* getModule(node::node_module_struct* t) {
    	t->version = NODE_MODULE_VERSION;
    	t->filename = "";
    	t->modname = "";
    	t->register_func = 0;
    	t->release_func = 0;
    	return t;
    }
};

#endif
