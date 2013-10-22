var _autorelease = require('core/autorelease.js');
var _gl = require('opengl');
var _glm = require('core/glm.js');
var _Buffer = require('glcore/buffer.js').Buffer;
var _getGLType = require('glcore/utils.js').getGLType;
var _inherit = require('core/inherit.js');

/**
 * Global flag
 * @type {boolean}
 */
var supportVbo = true;

/**
 * stand for a vbo buffer, handle detail operation when you work with mesh data
 * @param config
 */
function glBuffer(config) {
    _Buffer.call(this, config);

    // optional
    this.mGLtype = config && config.gltype || _getGLType(this.mType);
    this.mNormalize = config && config.normalize || false;
    this.mTarget = config && config.target || _gl.ARRAY_BUFFER;

    this.mIsVbo = config && config.isvbo || supportVbo;
    this.mVboId = 0;
    if (this.mIsVbo) {
        this.mVboId = _gl.createBuffer();
        this.__vboId = _autorelease.releaseGLBuffer(this.mVboId);
    }
};
_inherit(glBuffer, _Buffer);
glBuffer.prototype.clone = function () {
    return new glBuffer({
        stride: this.mStride,
        count: this.mCount,
        type: this.mType,
        gltype: this.mGLtype,
        element: this.mElement,
        normalize: this.mNormalize,
        buffer: new this.mType(this.mBuffer.buffer.slice(0))
    });
}
/**
 * regenerate a vbo id
 */
glBuffer.prototype.reload = function () {
    if (this.mIsVbo) {
        this.mVboId = _gl.createBuffer();
    }
}
/**
 * vbo should load as your need
 */
glBuffer.prototype.upload = function () {
    if (this.mIsVbo) {
        _gl.bindBuffer(this.mTarget, this.mVboId);
        _gl.bufferData(this.mTarget, this.mBuffer, _gl.STATIC_DRAW);
    } else {
        _gl.bindBuffer(this.mTarget, this.mBuffer);
    }
}
/**
 * make this vbo the current buffer.
 */
glBuffer.prototype.bindBuffer = function () {
    if (this.mIsVbo) {
        _gl.bindBuffer(this.mTarget, this.mVboId);
    }
}
glBuffer.prototype.isVbo = function () {
    return this.mIsVbo;
}
/**
 * util method
 * bind this.mBuffer as an vertex variable
 *
 * @param indx vertex index in shader
 */
glBuffer.prototype.bindVertex = function (indx) {
    _gl.enableVertexAttribArray(indx);
    if (this.mIsVbo) {
        _gl.bindBuffer(this.mTarget, this.mVboId);
        _gl.vertexAttribPointer(indx, this.mStride, this.mGLtype, this.mNormalize, 0, 0);
    } else {
        _gl.vertexAttribPointer(indx, this.mStride, this.mGLtype, this.mNormalize, 0, this.mBuffer);
    }
}
glBuffer.prototype.target = function () {
    if (arguments.length == 0) {
        return this.mTarget;
    }
    this.mTarget = arguments[0];
}

/**
 * create an index buffer
 * @param count triggles
 * @returns {glBuffer}
 */
function createIndexBuffer(stride, count) {
    if (arguments.length == 1) {
        count = stride;
        stride = 1;
    }
    return new glBuffer({
        stride: stride,
        count: count,
        type: Uint16Array,
        normalize: false,
        target: _gl.ELEMENT_ARRAY_BUFFER
    });
}
/**
 * create vector buffer
 *
 * @param stride
 * @param count
 * @returns {glBuffer}
 */
function createVectorBuffer(stride, count) {
    return new glBuffer({
        stride: stride,
        count: count,
        type: Float32Array,
        element: _glm['vector' + stride],
        normalize: false
    });
}
/**
 * create an structor with given element type
 * @param config
 * @returns {glBuffer}
 */
function createBuffer(config) {
    return new glBuffer(config);
}

/**
 * set the default behaivour of all buffers
 */
exports.enableVBO = function () {
    supportVbo = true;
}
exports.disableVBO = function () {
    supportVbo = false;
}

exports.glBuffer = glBuffer;
exports.createBuffer = createBuffer;
exports.createVectorBuffer = createVectorBuffer;
exports.createIndexBuffer = createIndexBuffer;
