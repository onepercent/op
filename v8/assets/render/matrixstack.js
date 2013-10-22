var _geometry = require('core/glm.js');
var _Matrix = _geometry.matrix4;
var _glm = _geometry.glm;
var _Buffer = require('glcore/buffer.js').Buffer;
var _inherit = require('core/inherit.js');

var $defaultDepth = 64;

// ========================================================
// Matrix Stack
// ========================================================
function MatrixStack(depth) {
    _Buffer.call(this, {
        stride: 16,
        count: depth || $defaultDepth,
        type: Float32Array,
        element: _Matrix
    });
    this.matrix = new _Matrix();
    this.cursor = 0;
    this.clear();
}
_inherit(MatrixStack, _Buffer);
MatrixStack.prototype.push = function (m) {
    if (this.cursor == this.mCount - 1) {
        throw ('MatrixStac.push depth overflow');
    }
    var cm = this.matrix;
    _glm.mulMM(cm, this.getElement(this.cursor, cm), m);
    this.setElement(++this.cursor, cm);
    return cm;
}
/**
 * push and reset the next matrix
 * @param m
 * @returns {*}
 */
MatrixStack.prototype.pushNext = function (m) {
    if (this.cursor == this.mCount - 1) {
        throw ('MatrixStac.push depth overflow');
    }
    var cm = this.matrix;
    cm.set(m);
    this.setElement(++this.cursor, cm);
    return cm;
}
MatrixStack.prototype.pop = function () {
    if (this.cursor == 0) {
        throw ('MatrixStac.pop minus cursor');
    }
    this.getElement(--this.cursor, this.matrix);
    return this.matrix;
}
MatrixStack.prototype.multip = function (m) {
    var cm = this.matrix;
    var c = this.cursor;
    _glm.mulMM(cm, this.getElement(c, cm), m);
    this.setElement(c, cm);
    return cm;
}
MatrixStack.prototype.identity = function () {
    this.matrix.identity();
    this.setElement(this.cursor, this.matrix);
}
MatrixStack.prototype.clear = function () {
    this.cursor = 0;
    this.identity();
}

// ========================================================
// Alpha Stack
// ========================================================
function AlphaStack(depth) {
    this.data = new Float32Array(depth || $defaultDepth);
    this.cursor = 0;
    this.clear();
}
AlphaStack.prototype.push = function (a) {
    a = this.data[this.cursor] * a;
    return this.data[++this.cursor] = a;
}
AlphaStack.prototype.pop = function () {
    return this.data[--this.cursor];
}
AlphaStack.prototype.clear = function () {
    this.data[this.cursor = 0] = 1;
}
AlphaStack.prototype.alpha = function () {
    return this.data[this.cursor];
}

exports.MatrixStack = MatrixStack;
exports.AlphaStack = AlphaStack;