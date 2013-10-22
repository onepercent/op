var _stack = require('render/matrixstack.js');
var _geometry = require('core/glm.js');
var _glm = _geometry.glm;

/**
 * drawable context
 *
 * @param camera
 * @constructor
 */
function Context(camera) {
    this.mCamera = camera;
    this.mStack = new _stack.MatrixStack(64);
    this.aStack = new _stack.AlphaStack(64);
    this.pvmCurrent = new _geometry.matrix4();// pvmCurrent = cameraMatrix * matrixStack
    this.matrix = new _geometry.matrix4();
    this.clear();
}
/**
 * give current matirx, return project->view->model matrix
 *
 * @param m
 * @returns {*}
 */
Context.prototype.getMatrix = function (m) {
    _glm.mulMM(this.matrix, this.pvmCurrent, m);
    return this.matrix;
}
Context.prototype.render = function (node, mesh, material) {
    material.use();
    material.bindMatrix(this.getMatrix(node.mMatrix));
    material.bindMesh(mesh);
    mesh.draw();
}
Context.prototype.pushMatrix = function (m) {
    _glm.mulMM(this.pvmCurrent, this.mCamera.pvmMatirx(), this.mStack.push(m));
}
Context.prototype.popMatrix = function () {
    _glm.mulMM(this.pvmCurrent, this.mCamera.pvmMatirx(), this.mStack.pop());
}
Context.prototype.pushAlpha = function (a) {
    this.aStack.push(a);
}
Context.prototype.popAlpha = function () {
    this.aStack.pop(a);
}
Context.prototype.clear = function () {
    this.mStack.clear();
    this.aStack.clear();
    this.pvmCurrent.set(this.mCamera.pvmMatirx());
    this.matrix.set(this.mCamera.pvmMatirx());
}
/**
 * called when camera update
 */
Context.prototype.onChange = function (w, h) {
    _glm.mulMM(this.pvmCurrent, this.mCamera.pvmMatirx(), this.mStack.matrix);
}

module.exports = Context;