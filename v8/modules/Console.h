//
//  Console.h
//  v8
//
//  Created by jie on 13-8-4.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __v8__Console__
#define __v8__Console__

#include "../app/node.h"
#include "../core/Module.h"

class Console : public Module {
public:
	static node::node_module_struct* getModule(node::node_module_struct* t);
};

#endif /* defined(__v8__Console__) */
