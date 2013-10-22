var _gl = require('opengl');
var _geometry = require('core/glm.js');
var _MeshNode = require('drawable/meshnode.js');
var _createMesh = require('glcore/meshbuffer.js').createMesh;
var _inherit = require('core/inherit.js');
var _rectangle = require('glcore/textureframe.js').rectangle;

var _glm = _geometry.glm;
var _v3 = _geometry.vec3f;
var _order = require('glcore/constance.js').STRIP_ORDER;

/**
 * @param material
 * @param frame texture frame
 * @constructor
 */
function ColorNode(material, w, h) {
    _MeshNode.call(this, this.createMesh(), material);
    if (arguments.length == 3) {
        this.setSize(w, h);
    } else {
        this.setSize(0, 0);
    }
}
_inherit(ColorNode, _MeshNode);
ColorNode.prototype.setSize = function (w, h) {
    _MeshNode.prototype.setSize.call(this, w, h);
    this.initMesh();
}
ColorNode.prototype.createMesh = function () {
    return _createMesh('p3', 4, _gl.TRIANGLE_STRIP);
}
ColorNode.prototype.initMesh = function () {
    var v = new _v3();
    var b = this.mBuffer;

    var accp = b.accessor('p');
    var m = _rectangle(this.mSize[0], this.mSize[1]);

    for (var i = 0; i < 8; i += 2) {
        v.set(_order[i], _order[i + 1], 0);
        _glm.mulMV3(v, m, v);
        accp.set(v);
        b.push(i / 2);
    }

    b.upload();
}
ColorNode.prototype.setColor = function (color) {
    this.mMaterial.setColor(color);
}
ColorNode.prototype.draw = function (context) {
    if (!this.mVisiable) {
        return;
    }
    _gl.blendFunc(_gl.SRC_ALPHA, _gl.ONE_MINUS_SRC_ALPHA);
    this.updateMatrix();
    context.render(this, this.mBuffer, this.mMaterial);
    _gl.blendFunc(_gl.ONE, _gl.ONE_MINUS_SRC_ALPHA);
}

module.exports = ColorNode;