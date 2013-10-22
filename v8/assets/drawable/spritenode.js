var _gl = require('opengl');
var _geometry = require('core/glm.js');
var _MeshNode = require('drawable/meshnode.js');
var _createMesh = require('glcore/meshbuffer.js').createMesh;
var _inherit = require('core/inherit.js');

var _glm = _geometry.glm;
var _v2 = _geometry.vec2f;
var _v3 = _geometry.vec3f;
var _order = require('glcore/constance.js').STRIP_ORDER;

/**
 * @param material
 * @param frame texture frame
 * @constructor
 */
function SpriteNode(material, frame) {
    _MeshNode.call(this, this.createMesh(), material);

    this.mFrame = frame;
    this.setSize(frame.width(), frame.height());
    this.initMesh();
}
_inherit(SpriteNode, _MeshNode);
SpriteNode.prototype.createMesh = function () {
    return _createMesh('p3t2', 4, _gl.TRIANGLE_STRIP);
}
SpriteNode.prototype.initMesh = function () {
    var t = new _v2();
    var v = new _v3();
    var b = this.mBuffer;

    var accp = b.accessor('p');
    var acct = b.accessor('t');
    var f = this.mFrame;
    var m = f.getMatrix();

    for (var i = 0; i < 8; i += 2) {
        f.getVisiablePoint(v, t, _order[i], _order[i + 1]);
        _glm.mulMV3(v, m, v);

        acct.set(t);
        accp.set(v);
        b.push(i / 2);
    }

    b.upload();
}

module.exports = SpriteNode;