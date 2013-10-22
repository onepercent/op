var _geometry = require('core/glm.js');
var _glm = _geometry.glm;
var _vec3f = _geometry.vec3f;
var _matrix = _geometry.mat4f;
var _gl = require('opengl');

function Camera() {
    this.mModelViewMatrix = new _matrix();
    this.mProjectMatrix = new _matrix();
    this.mProjectModelViewMatirx = new _matrix();
    this.mTouchMatrix = new _matrix();
    this.mDirty = true;
    this.mTimestamp = 0;
    this.mProjectParam = {
        left: 0,
        right: 1,
        bottom: 0,
        top: 1,
        near: 1,
        far: 2
    };
    this.mViewportParam = {
        x: 0,
        y: 0,
        width: 1,
        height: 1
    }
    this.setViewport(0, 0, 1, 1);
}
Camera.prototype.viewport = function () {
    var p = this.mViewportParam;
    _gl.viewport(p.x, p.y, p.width, p.height);
}
Camera.prototype.setViewport = function (x, y, width, height) {
    if (arguments.length == 2) {
        this.mViewportParam = {
            x: 0,
            y: 0,
            width: x,
            height: y
        }
    } else {
        this.mViewportParam = {
            x: x,
            y: y,
            width: width,
            height: height
        }
    }
    this.updatePVM();
}

// ========================================================
// modelview
// ========================================================
Camera.prototype.lookAt = function (eye, center, up) {
    this.mModelViewMatrix.identity();
    _glm.lookAt(this.mModelViewMatrix, new _vec3f(eye), new _vec3f(center), new _vec3f(up));
    this.updatePVM();
    return this;
}

// ========================================================
// projection
// ========================================================
Camera.prototype.frustum = function (left, right, bottom, top, near, far) {
    this.mProjectMatrix.identity();
    _glm.ortho.frustum(this, [this.mProjectMatrix].concat(Array.prototype.splice.call(arguments, 0)));
    this.updatePVM();
    return this;
}
Camera.prototype.ortho = function (left, right, bottom, top, near, far) {
    this.mProjectParam = {
        left: left,
        right: right,
        bottom: bottom,
        top: top,
        near: near,
        far: far
    };
    this.mProjectMatrix.identity();
    _glm.ortho.apply(this, [this.mProjectMatrix].concat(Array.prototype.splice.call(arguments, 0)));
    this.updatePVM();
    return this;
}
Camera.prototype.perspective = function (fovy, aspect, zNear, zFar) {
    this.mProjectMatrix.identity();
    _glm.perspective(this.mProjectMatrix, fovy, aspect, zNear, zFar);
    this.updatePVM();
    return this;
}

Camera.prototype.updatePVM = function () {
    this.mDirty = true;
    this.mTimestamp = new Date().getTime();
    _glm.mulMM(this.mProjectModelViewMatirx, this.mProjectMatrix, this.mModelViewMatrix);

    var port_p = this.mViewportParam;
    var proj_p = this.mProjectParam;
    var pw = proj_p.right - proj_p.left;
    var ph = proj_p.top - proj_p.bottom;
    var scalex = pw / port_p.width;
    var scaley = ph / port_p.height;
    var centerx = -proj_p.left / pw;
    var centery = -proj_p.bottom / ph;

    var touchM = this.mTouchMatrix;
    touchM.identity();
    touchM.scale(new _vec3f(scalex, scaley, 1));
    touchM.translate(new _vec3f(-port_p.width * centerx - port_p.x, -port_p.height * centery - port_p.y, 0));
}
Camera.prototype.pvmMatirx = function () {
    return this.mProjectModelViewMatirx;
}
Camera.prototype.modelViewMatrix = function () {
    return this.mModelViewMatrix;
}
Camera.prototype.isDirty = function () {
    return this.mDirty;
}
Camera.prototype.clean = function () {
    this.mDirty = false;
}

Camera.prototype.updateTouchMatrix = function (inverse, model) {
    _glm.mulMM(inverse, this.mModelViewMatrix, model);
    inverse.inverse();
    _glm.mulMM(inverse, inverse, this.mTouchMatrix);
}

exports.createCamera = function () {
    return new Camera();
}