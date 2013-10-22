var _gl = require('opengl');
var _geometry = require('core/glm.js');
var _MeshNode = require('drawable/meshnode.js');
var _createMesh = require('glcore/meshbuffer.js').createMesh;
var _inherit = require('core/inherit.js');

var _matrix4 = _geometry.matrix4;
var _v2 = _geometry.vec2f;
var _v3 = _geometry.vec3f;
var _glm = _geometry.glm;

/**
 * underlying detail operation
 *
 * @param buf
 * @param frame
 * @constructor
 */
function Filler(buf, frame) {
    this.buf = buf;
    this.frame = frame;
    this.v = new _v3();
    this.t = new _v2();
    this.accp = buf.accessor('p');
    this.acct = buf.accessor('t');

    this.fmatrix = new _matrix4();
    frame.getMatrix(this.fmatrix);
    // matrix used for texture point mapping
    this.matrix = new _matrix4(this.fmatrix);
}
Filler.prototype.copyPoint = function (from, to) {
    this.buf.copy(from, to, 1);
}
/**
 * set the texture matrix translation
 *
 * @param x
 * @param y
 */
Filler.prototype.translation = function (x, y) {
    this.v.set(x, y, 0);
    _glm.translation(this.matrix, this.v);
    _glm.mulMM(this.matrix, this.matrix, this.fmatrix);
}
Filler.prototype.fillPoint = function (x, y, start) {
    var v = this.v;
    this.frame.getPointAbs(v, this.t, x, y);
    _glm.mulMV3(v, this.matrix, v);

    this.acct.set(this.t);
    this.accp.set(this.v);
    this.buf.push(start);
}

/**
 * bind getter setter to prototype
 * @param pro
 * @param name
 */
function addProp(pro, name) {
    var pname = '_' + name;
    pro[name] = function (d) {
        if (arguments.length == 0) {
            return this[pname];
        }
        this[pname] = d;
        return this;
    }
}
function transform(dest, m, x, y) {
}
/**
 * @param material
 * @param frame texture frame
 * @constructor
 */
function NinePatch(material, frame) {
    _MeshNode.call(this, this.createMesh(), material);
    this.mFrame = frame;

    this.init();
    this.setSize(frame.width(), frame.height());
    this.updateMesh();
}
_inherit(NinePatch, _MeshNode);
NinePatch.prototype.createMesh = function () {
}
NinePatch.prototype.updateMesh = function () {
}
NinePatch.prototype.init = function () {
}
NinePatch.prototype.setSize = function (w, h) {
    _MeshNode.prototype.setSize.call(this, w, h);
    return this;
}

function Border(material, frame) {
    NinePatch.call(this, material, frame);
}
_inherit(Border, NinePatch);
Border.prototype.init = function () {
    if (arguments.length == 0) {
        this._left = this._top = this._right = this._bottom = 0;
    } else if (arguments.length == 1) {
        this._left = this._top = this._right = this._bottom = arguments[0];
    } else if (arguments.length == 2) {
        this._left = arguments[0];
        this._right = arguments[1];
        this._top = this._bottom = 0;
    } else if (arguments.length == 4) {
        this._left = arguments[0];
        this._top = arguments[1];
        this._right = arguments[2];
        this._bottom = arguments[3];
    }
}
Border.prototype.fill_h = function (acc, cell) {
    var current = cell * 6;
    var pre = current - 6;
    var next = current + 6;
    acc.copyPoint(pre + 2, current);
    acc.copyPoint(pre + 5, current + 1);
    acc.copyPoint(next, current + 2);
    acc.copyPoint(next, current + 3);
    acc.copyPoint(pre + 5, current + 4);
    acc.copyPoint(next + 1, current + 5);
}
Border.prototype.fill_v = function (acc, cell) {
    var current = cell * 6;
    var pre = current - 18;
    var next = current + 18;
    acc.copyPoint(pre + 4, current);
    acc.copyPoint(next, current + 1);
    acc.copyPoint(pre + 5, current + 2);
    acc.copyPoint(pre + 5, current + 3);
    acc.copyPoint(next, current + 4);
    acc.copyPoint(next + 2, current + 5);
}
Border.prototype.fillRect = function (acc, x1, y1, x2, y2, start) {
    acc.fillPoint(x1, y1, start);
    acc.fillPoint(x1, y2, start + 1);
    acc.fillPoint(x2, y1, start + 2);
    acc.copyPoint(start + 2, start + 3);
    acc.copyPoint(start + 1, start + 4);
    acc.fillPoint(x2, y2, start + 5);
}
Border.prototype.updateMesh = function () {
    var fw = this.mFrame.width();
    var fh = this.mFrame.height();
    var w = this.mSize[0];
    var h = this.mSize[1];
    var acc = new Filler(this.mBuffer, this.mFrame);

    this.fillRect(acc, 0, fh - this._bottom, this._left, fh, 36);// left bottom
    acc.translation(0, h - fh);
    this.fillRect(acc, 0, 0, this._left, this._top, 0);// left top
    acc.translation(w - fw, 0);
    this.fillRect(acc, fw - this._right, fh - this._bottom, fw, fh, 48);// right bottom
    acc.translation(w - fw, h - fh);
    this.fillRect(acc, fw - this._right, 0, fw, this._top, 12);// right top

    this.fill_h(acc, 1);
    this.fill_h(acc, 7);
    this.fill_v(acc, 3);
    this.fill_v(acc, 4);
    this.fill_v(acc, 5);

    this.mBuffer.upload();
    return this;
}
Border.prototype.createMesh = function () {
    return _createMesh('p3t2', 54, _gl.TRIANGLES);
}
addProp(Border.prototype, 'left');
addProp(Border.prototype, 'top');
addProp(Border.prototype, 'right');
addProp(Border.prototype, 'bottom');

/**
 * vertical 9patch sprite
 *
 * @param material
 * @param frame
 * @constructor
 */
function Vertical(material, frame) {
    NinePatch.call(this, material, frame);
}
_inherit(Vertical, NinePatch);
Vertical.prototype.setSize = function (w, h) {
    _MeshNode.prototype.setSize.call(this, this.mFrame.width(), h);
    return this;
}
Vertical.prototype.init = function () {
    if (arguments.length == 0) {
        this._top = this._bottom = 0;
    } else if (arguments.length == 1) {
        this._top = this._bottom = arguments[0];
    } else if (arguments.length == 2) {
        this._top = arguments[0];
        this._bottom = arguments[1];
    }
}
Vertical.prototype.fillRect = function (acc, x1, y1, x2, y2, start) {
    acc.fillPoint(x1, y2, start);
    acc.fillPoint(x2, y2, start + 1);
    acc.fillPoint(x1, y1, start + 2);
    acc.fillPoint(x2, y1, start + 3);
}
Vertical.prototype.updateMesh = function () {
    var fw = this.mFrame.width();
    var fh = this.mFrame.height();
    var h = this.mSize[1];
    var acc = new Filler(this.mBuffer, this.mFrame);

    this.fillRect(acc, 0, fh - this._bottom, fw, fh, 0);// bottom
    acc.translation(0, h - fh, 0);
    this.fillRect(acc, 0, 0, fw, this._top, 4);// top

    this.mBuffer.upload();
    return this;
}
Vertical.prototype.createMesh = function () {
    return _createMesh('p3t2', 8, _gl.TRIANGLE_STRIP);
}
addProp(Vertical.prototype, 'top');
addProp(Vertical.prototype, 'bottom');

/**
 * horizontal 9patch sprite
 *
 * @param material
 * @param frame
 * @constructor
 */
function Horizontal(material, frame) {
    NinePatch.call(this, material, frame);
}
_inherit(Horizontal, NinePatch);
Horizontal.prototype.setSize = function (w, h) {
    _MeshNode.prototype.setSize.call(this, w, this.mFrame.height());
    return this;
}
Horizontal.prototype.init = function () {
    if (arguments.length == 0) {
        this._left = this._right = 0;
    } else if (arguments.length == 1) {
        this._left = this._right = arguments[0];
    } else if (arguments.length == 2) {
        this._left = arguments[0];
        this._right = arguments[1];
    }
}
Horizontal.prototype.fillRect = function (acc, x1, y1, x2, y2, start) {
    acc.fillPoint(x1, y1, start);
    acc.fillPoint(x1, y2, start + 1);
    acc.fillPoint(x2, y1, start + 2);
    acc.fillPoint(x2, y2, start + 3);
}
Horizontal.prototype.updateMesh = function () {
    var fw = this.mFrame.width();
    var fh = this.mFrame.height();
    var w = this.mSize[0];
    var acc = new Filler(this.mBuffer, this.mFrame);

    this.fillRect(acc, 0, 0, this._left, fh, 0);// left
    acc.translation(w - fw, 0);
    this.fillRect(acc, fw - this._right, 0, fw, fh, 4);// right

    this.mBuffer.upload();
    return this;
}
Horizontal.prototype.createMesh = function () {
    return _createMesh('p3t2', 8, _gl.TRIANGLE_STRIP);
}
addProp(Horizontal.prototype, 'left');
addProp(Horizontal.prototype, 'right');

/**
 * create a general nine patch
 *
 * @param material
 * @param frame
 */
function create9Patch(material, frame) {
    var nine = new Border(material, frame);
    if (arguments.length > 2) {
        nine.init.apply(nine, Array.prototype.slice.call(arguments, 2));
        nine.updateMesh();
    }
    return nine;
}
function create9Patch_h(material, frame) {
    var nine = new Horizontal(material, frame);
    if (arguments.length > 2) {
        nine.init.apply(nine, Array.prototype.slice.call(arguments, 2));
        nine.updateMesh();
    }
    return nine;
}
function create9Patch_v(material, frame) {
    var nine = new Vertical(material, frame);
    if (arguments.length > 2) {
        nine.init.apply(nine, Array.prototype.slice.call(arguments, 2));
        nine.updateMesh();
    }
    return nine;
}

module.exports.create9Patch = create9Patch;
module.exports.create9Patch_h = create9Patch_h;
module.exports.create9Patch_v = create9Patch_v;