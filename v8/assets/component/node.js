var _Element = require('component/element.js');
var _geometry = require('core/glm.js');
var _glm = _geometry.glm;
var _vec3f = _geometry.vec3f;
var _matrix4 = _geometry.matrix4;
var _inherit = require('core/inherit.js');

var aixz = new _vec3f(0, 0, 1);

/**
 * an objec with given position
 * @constructor
 */
function Node() {
    _Element.call(this);

    // these vector should rener be removed or replace
    this.mAnchor = new _vec3f();// anchor point in percent
    this.mCenter = new _vec3f();// center in size(absoult) coordinate
    this.mSize = new _vec3f();// width height depth
    this.mOffset = new _vec3f();
    this.mPosition = new _vec3f();
    this.mScale = new _vec3f(1, 1, 1);
    this.mMatrix = new _matrix4();
    this.mRotate = 0;

    this.mFlags = this.mFlags;
    this.mVisiable = true;
    this.mParent = null;
}
_inherit(Node, _Element);
Node.prototype.__elementType |= Node.prototype.ElementTypeNode;
Node.prototype.mTag = 'node';

var _flags = {
    FlagMatrix: 1,
    FlagTouchMatrix: 1 << 1,
    FlagSeal: 1 << 2// cannot access child of this element, works with selector
}
for (var i in _flags) {
    Node.prototype[i] = _flags[i];
}

var MatrixDirty = _flags.FlagTouchMatrix | _flags.FlagMatrix;
var FlagMatrix = _flags.FlagMatrix;
Node.prototype.mFlags = -1 & (~_flags.FlagSeal);

Node.prototype.visiable = function () {
    if (arguments.length > 0) {
        this.mVisiable = arguments[0];
        return this;
    }
    return this.mVisiable;
}
Node.prototype.setRotate = function (r) {
    this.mRotate = r;
    this.addFlag(MatrixDirty);
}
Node.prototype.getRotate = function (r) {
    return this.mRotate;
}
Node.prototype.setPosition = function (x, y) {
    if (arguments.length == 1) {
        this.mPosition.set(x);
    } else {
        this.mPosition[0] = x;
        this.mPosition[1] = y;
    }
    this.addFlag(MatrixDirty);
}
Node.prototype.translate = function (offset) {
    this.mPosition.add(offset);
    this.addFlag(MatrixDirty);
}
Node.prototype.setScale = function (sx, sy) {
    if (arguments.length == 1) {
        this.mScale[0] = this.mScale[1] = sx;
    } else {
        this.mScale[0] = sx;
        this.mScale[1] = sy;
    }
    this.addFlag(MatrixDirty);
}
Node.prototype.getPosition = function () {
    return this.mPosition;
}
Node.prototype.setAnchor = function (ax, ay) {
    if (arguments.length == 1) {
        this.mAnchor.set(ax);
    } else {
        this.mAnchor[0] = ax;
        this.mAnchor[1] = ay;
    }
    this.updateOffset();
}
Node.prototype.setSize = function (w, h) {
    if (arguments.length == 1) {
        this.mSize.set(w);
    } else {
        this.mSize[0] = w;
        this.mSize[1] = h;
    }
    this.updateOffset();
}
Node.prototype.getSize = function () {
    return this.mSize;
}
Node.prototype.getCenter = function () {
    return this.mCenter;
}
Node.prototype.setCenter = function (cx, cy) {
    if (arguments.length == 1) {
        this.mCenter.set(cx);
    } else {
        this.mCenter[0] = cx;
        this.mCenter[1] = cy;
    }
    this.updateOffset();
}
Node.prototype.updateOffset = function () {
    this.mOffset.set(
        this.mCenter[0] - this.mAnchor[0] * this.mSize[0],
        this.mCenter[1] - this.mAnchor[1] * this.mSize[1]
    );
}
Node.prototype.width = function () {
    return this.mSize[0];
}
Node.prototype.height = function () {
    return this.mSize[1];
}

function _getMatrix(m) {
    // translate, rotate, scale
    //m.identity();
    //m.translate(this.mPosition);
    //m.rotate(this.mRotate, aixz);
    //m.scale(this.mScale);
    //m.translate(this.mOffset);
    _glm.nodeMatrix(m, this.mPosition, this.mRotate, this.mScale, this.mOffset);
    //console.log('___getMatrix', this.mTag, m, this.mPosition, this.mRotate, this.mScale, this.mOffset);
    return m;
}
/**
 * @returns {boolean} whether or not this node is updated
 */
Node.prototype.updateMatrix = function () {
    var flag = this.mFlags;
    if ((flag & FlagMatrix) == 0) {
        return false;
    }
    _getMatrix.call(this, this.mMatrix);
    this.mFlags = flag & (~FlagMatrix);
    return true;
}
Node.prototype.addFlag = function (flag) {
    this.mFlags |= flag;
}
Node.prototype.removeFlag = function (flag) {
    this.mFlags &= ~flag;
}
Node.prototype.hasFlag = function (flag) {
    return (this.mFlags & flag) > 0;
}
Node.prototype.getRemove = function (flag) {
    if (this.mFlags & flag) {
        this.mFlags &= ~flag;
        return true;
    }
    return false;
}
Node.prototype.getMatrix = function (m) {
    return _getMatrix.call(this, m || new _geometry.matrix4());
}
Node.prototype.draw = function (context) {
}

module.exports = Node;

