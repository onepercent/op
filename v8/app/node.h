//
//  node.h
//  v8
//
//  Created by jie on 13-8-4.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef v8_node_h
#define v8_node_h

#include <v8.h>

extern v8::Isolate* node_isolate;

namespace node {

#define NODE_MODULE_VERSION 0x000C /* v0.12 */
    
    typedef void (*addon_register_func) (const v8::FunctionCallbackInfo<v8::Value>& args);
    typedef void (*addon_release_func) ();
    
    struct node_module_struct {
        int version;
        void *dso_handle;
        const char *filename;
        node::addon_register_func register_func;
        node::addon_release_func release_func;
        const char *modname;
    };
    
#define NODE_STANDARD_MODULE_STUFF \
          NODE_MODULE_VERSION,     \
          NULL,                    \
        __FILE__

#define NODE_JS_MODULE(modname)                             \
    new node_js_module_struct(NODE_MODULE_VERSION, #modname, "lib/")

#define NODE_MODULE(classname, modname, regfunc)            \
    node_module_struct classname::_Module = {               \
            NODE_STANDARD_MODULE_STUFF,                     \
            (node::addon_register_func) regfunc,            \
            modname                                         \
        };                                                  \
    
    // Used to be a macro, hence the uppercase name.
    template <typename TypeName>
    inline void NODE_SET_METHOD(const TypeName& recv,
                                const char* name,
                                v8::FunctionCallback callback) {
        v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(callback);
        v8::Local<v8::Function> fn = t->GetFunction();
        v8::Local<v8::String> fn_name = v8::String::New(name);
        fn->SetName(fn_name);
        recv->Set(fn_name, fn);
    }
#define NODE_SET_METHOD node::NODE_SET_METHOD
    
    // Used to be a macro, hence the uppercase name.
    // Not a template because it only makes sense for FunctionTemplates.
    inline void NODE_SET_PROTOTYPE_METHOD(v8::Handle<v8::FunctionTemplate> recv,
                                          const char* name,
                                          v8::FunctionCallback callback) {
        v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(callback);
        recv->PrototypeTemplate()->Set(v8::String::New(name), t->GetFunction());
    }
#define NODE_SET_PROTOTYPE_METHOD node::NODE_SET_PROTOTYPE_METHOD
    
}

#endif
