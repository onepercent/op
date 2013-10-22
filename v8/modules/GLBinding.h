//
//  GLBinding.h
//  jstest
//
//  Created by jie on 13-7-25.
//  Copyright (c) 2013年 jie. All rights reserved.
//

#ifndef __js11__GLBinding__
#define __js11__GLBinding__

#include <v8.h>
#include "../app/node.h"
#include "../core/Module.h"

using namespace v8;

#define DEFINE_GL(name) static void name##Callback(const v8::FunctionCallbackInfo<Value>& args)

class GLBinding :public Module {
public:
    DEFINE_GL(getAttachedShaders);
    DEFINE_GL(getSupportedExtensions);
    DEFINE_GL(getExtension);
    DEFINE_GL(activeTexture);
    DEFINE_GL(attachShader);
    DEFINE_GL(bindAttribLocation);
    DEFINE_GL(bindBuffer);
    DEFINE_GL(bindFramebuffer);
    DEFINE_GL(bindRenderbuffer);
    DEFINE_GL(bindTexture);
    DEFINE_GL(blendColor);
    DEFINE_GL(blendEquation);
    DEFINE_GL(blendEquationSeparate);
    DEFINE_GL(blendFunc);
    DEFINE_GL(blendFuncSeparate);
    DEFINE_GL(bufferData);
    DEFINE_GL(bufferSubData);
    DEFINE_GL(checkFramebufferStatus);
    DEFINE_GL(clear);
    DEFINE_GL(clearColor);
    DEFINE_GL(clearDepth);
    DEFINE_GL(clearStencil);
    DEFINE_GL(colorMask);
    DEFINE_GL(compileShader);
    DEFINE_GL(copyTexImage2D);
    DEFINE_GL(copyTexSubImage2D);
    DEFINE_GL(createBuffer);
    DEFINE_GL(createFramebuffer);
    DEFINE_GL(createProgram);
    DEFINE_GL(createRenderbuffer);
    DEFINE_GL(createShader);
    DEFINE_GL(createTexture);
    DEFINE_GL(cullFace);
    DEFINE_GL(deleteBuffer);
    DEFINE_GL(deleteFramebuffer);
    DEFINE_GL(deleteProgram);
    DEFINE_GL(deleteRenderbuffer);
    DEFINE_GL(deleteShader);
    DEFINE_GL(deleteTexture);
    DEFINE_GL(depthFunc);
    DEFINE_GL(depthMask);
    DEFINE_GL(depthRange);
    DEFINE_GL(detachShader);
    DEFINE_GL(disable);
    DEFINE_GL(disableVertexAttribArray);
    DEFINE_GL(drawArrays);
    DEFINE_GL(drawElements);
    DEFINE_GL(enable);
    DEFINE_GL(enableVertexAttribArray);
    DEFINE_GL(finish);
    DEFINE_GL(flush);
    DEFINE_GL(framebufferRenderbuffer);
    DEFINE_GL(framebufferTexture2D);
    DEFINE_GL(frontFace);
    DEFINE_GL(generateMipmap);
    DEFINE_GL(getActiveAttrib);
    DEFINE_GL(getActiveUniform);
    DEFINE_GL(getAttribLocation);
    DEFINE_GL(getParameter);
    DEFINE_GL(getBufferParameter);
    DEFINE_GL(getError);
    DEFINE_GL(getFramebufferAttachmentParameter);
    DEFINE_GL(getProgramParameter);
    DEFINE_GL(getProgramInfoLog);
    DEFINE_GL(getRenderbufferParameter);
    DEFINE_GL(getShaderParameter);
    DEFINE_GL(getShaderInfoLog);
    DEFINE_GL(getShaderSource);
    DEFINE_GL(getTexParameter);
    DEFINE_GL(getUniform);
    DEFINE_GL(getUniformLocation);
    DEFINE_GL(getVertexAttrib);
    DEFINE_GL(getVertexAttribOffset);
    DEFINE_GL(hint);
    DEFINE_GL(isBuffer);
    DEFINE_GL(isEnabled);
    DEFINE_GL(isFramebuffer);
    DEFINE_GL(isProgram);
    DEFINE_GL(isRenderbuffer);
    DEFINE_GL(isShader);
    DEFINE_GL(isTexture);
    DEFINE_GL(lineWidth);
    DEFINE_GL(linkProgram);
    DEFINE_GL(pixelStorei);
    DEFINE_GL(polygonOffset);
    DEFINE_GL(readPixels);
    DEFINE_GL(renderbufferStorage);
    DEFINE_GL(sampleCoverage);
    DEFINE_GL(scissor);
    DEFINE_GL(shaderSource);
    DEFINE_GL(stencilFunc);
    DEFINE_GL(stencilFuncSeparate);
    DEFINE_GL(stencilMask);
    DEFINE_GL(stencilMaskSeparate);
    DEFINE_GL(stencilOp);
    DEFINE_GL(stencilOpSeparate);
    DEFINE_GL(texImage2D);
    DEFINE_GL(internalTexImage2D);
    DEFINE_GL(texParameterf);
    DEFINE_GL(texParameteri);
    DEFINE_GL(texSubImage2D);
    DEFINE_GL(internalTexSubImage2D);
    DEFINE_GL(uniform1f);
    DEFINE_GL(uniform1fv);
    DEFINE_GL(uniform1i);
    DEFINE_GL(uniform1iv);
    DEFINE_GL(uniform2f);
    DEFINE_GL(uniform2fv);
    DEFINE_GL(uniform2i);
    DEFINE_GL(uniform2iv);
    DEFINE_GL(uniform3f);
    DEFINE_GL(uniform3fv);
    DEFINE_GL(uniform3i);
    DEFINE_GL(uniform3iv);
    DEFINE_GL(uniform4f);
    DEFINE_GL(uniform4fv);
    DEFINE_GL(uniform4i);
    DEFINE_GL(uniform4iv);
    DEFINE_GL(uniformMatrix2fv);
    DEFINE_GL(uniformMatrix3fv);
    DEFINE_GL(uniformMatrix4fv);

    DEFINE_GL(useProgram);
    DEFINE_GL(validateProgram);
    DEFINE_GL(vertexAttrib1f);
    DEFINE_GL(vertexAttrib1fv);
    DEFINE_GL(vertexAttrib2f);
    DEFINE_GL(vertexAttrib2fv);
    DEFINE_GL(vertexAttrib3f);
    DEFINE_GL(vertexAttrib3fv);
    DEFINE_GL(vertexAttrib4f);
    DEFINE_GL(vertexAttrib4fv);
    DEFINE_GL(vertexAttribPointer);
    DEFINE_GL(viewport);

    static node::node_module_struct* getModule(node::node_module_struct* t);
};

#undef DEFINE_GL
#endif /* defined(__jstest__GLBinding__) */
