//
//  ReleaseImpl.cpp
//  v8
//
//  Created by jie on 13-9-27.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#include "ReleaseImpl.h"
#include "../global.h"

using namespace v8;
bool ReleaseTask::debug = true;
bool ReleaseTask::glAvaiable = false;

ReleaseTask* ReleaseTask::createTask(int type) {
    ReleaseTask* task = 0;
    switch (type) {
        case RELEASE_GLBuffer:
            task = new ReleaseGLBuffer();
            break;
        case RELEASE_GLTexture:
            task = new ReleaseGLTexture();
            break;
        case RELEASE_GLProgram:
            task = new ReleaseGLProgram();
            break;
        case RELEASE_GLShader:
            task = new ReleaseGLShader();
            break;
    }
    if(task == 0) {
        LOGI("ReleaseTask type %d implement not found", type);
    }
    return task;
}
ReleaseTask* ReleaseTask::createTaskEnum(ReleaseType type) {
    return createTask(type);
}
ReleaseTask::ReleaseTask() {
}
ReleaseTask::~ReleaseTask() {
}

// ==========================
// GLBuffer
// ==========================
ReleaseGLBuffer::ReleaseGLBuffer() : mBuffer(0) {
}
ReleaseGLBuffer::~ReleaseGLBuffer() {
    release();
}
void ReleaseGLBuffer::init(const v8::FunctionCallbackInfo<v8::Value> &args) {
    if(args.Length() > 0) {
        mBuffer = args[0]->Uint32Value();
        //LOGI("ReleaseGLBuffer:%d", mBuffer);
    }
}
void ReleaseGLBuffer::release() {
    if(mBuffer) {
        if(ReleaseTask::debug) {
            LOGI("~ReleaseGLBuffer:%d", mBuffer);
        }
        if (glAvaiable) {
            glDeleteBuffers(1, &mBuffer);
        }
        mBuffer = 0;
    }
}

// ==========================
// GLBuffer
// ==========================
ReleaseGLTexture::ReleaseGLTexture() : mTexture(0) {
}
ReleaseGLTexture::~ReleaseGLTexture() {
    release();
}
void ReleaseGLTexture::init(const v8::FunctionCallbackInfo<v8::Value> &args) {
    if(args.Length() > 0) {
        mTexture = args[0]->Uint32Value();
        std::string s(*String::Utf8Value(args[1]->ToString()));
        mUrl = s;
        //LOGI("ReleaseGLTexture:%d %s", mTexture, mUrl.c_str());
    }
}
void ReleaseGLTexture::release() {
    if(mTexture) {
        if(ReleaseTask::debug) {
            LOGI("~ReleaseGLTexture:%d %s", mTexture, mUrl.c_str());
        }
        if (glAvaiable) {
            glDeleteTextures(1, &mTexture);
        }
        mTexture = 0;
    }
}

// ==========================
// GLBuffer
// ==========================
ReleaseGLProgram::ReleaseGLProgram() : mProgram(0) {
}
ReleaseGLProgram::~ReleaseGLProgram() {
    release();
}
void ReleaseGLProgram::init(const v8::FunctionCallbackInfo<v8::Value> &args) {
    if(args.Length() > 0) {
        mProgram = args[0]->Uint32Value();
        //LOGI("ReleaseGLTexture:%d %s", mTexture, mUrl.c_str());
    }
}
void ReleaseGLProgram::release() {
    if(mProgram) {
        if(ReleaseTask::debug) {
            LOGI("~ReleaseGLProgram:%d", mProgram);
        }
        if (glAvaiable) {
            glDeleteProgram(mProgram);
        }
        mProgram = 0;
    }
}

// ==========================
// GLBuffer
// ==========================
ReleaseGLShader::ReleaseGLShader() : mShader(0) {
}
ReleaseGLShader::~ReleaseGLShader() {
    release();
}
void ReleaseGLShader::init(const v8::FunctionCallbackInfo<v8::Value> &args) {
    if(args.Length() > 0) {
        mShader = args[0]->Uint32Value();
        //LOGI("ReleaseGLTexture:%d %s", mTexture, mUrl.c_str());
    }
}
void ReleaseGLShader::release() {
    if(mShader) {
        if(ReleaseTask::debug) {
            LOGI("~ReleaseGLShader:%d", mShader);
        }
        if (glAvaiable) {
            glDeleteShader(mShader);
        }
        mShader = 0;
    }
}
