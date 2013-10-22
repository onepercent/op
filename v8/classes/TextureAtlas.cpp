//
//  TextureAtlas.cpp
//  v8
//
//  Created by jie on 13-9-24.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "TextureAtlas.h"
#include "../utils/AssetUtil.h"
#include "../core/v8Utils.h"

METHOD_BEGIN(bindToUnit, info) {
    HandleScope scope;
    TextureAtlas* fontAtlas = internalPtr<TextureAtlas>(info, CLASS_Atlas);
    if(fontAtlas == 0) {
        return;
    }

    texture_atlas_t *atlas = fontAtlas->atlas;
    if(atlas->id)
    {
        int unit = info[0]->Uint32Value();
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, atlas->id);
    }
}

static v8::Local<v8::Function> initClass(v8::Handle<v8::FunctionTemplate>& temp) {
    HandleScope scope;

    Local<ObjectTemplate> obj = temp->PrototypeTemplate();
    EXPOSE_METHOD(obj, bindToUnit, ReadOnly | DontDelete);

    return scope.Close(temp->GetFunction());
}

TextureAtlas::TextureAtlas() {
}
TextureAtlas::~TextureAtlas() {
    release();
}
/**
 * @width
 * @height
 * @depth
 */
void TextureAtlas::init(const v8::FunctionCallbackInfo<Value> &args) {
    int width = args[0]->Uint32Value();
    int height = args[1]->Uint32Value();
    int depth = args[2]->Uint32Value();
    atlas = texture_atlas_new(width, height, depth);
}

class_struct* TextureAtlas::getExportStruct() {
    static class_struct mTemplate = {
        initClass, "atlas", CLASS_Atlas
    };
    return &mTemplate;
}
ClassType TextureAtlas::getClassType() {
    return getExportStruct()->mType;
}
void TextureAtlas::doRelease() {
	LOGI("--->texture_atlas_t.id %d", atlas->id);
    texture_atlas_delete(atlas);
}
