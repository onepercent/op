var Node = require('component/node.js');
var _inherit = require('core/inherit.js');

/**
 * @param frame texture frame
 * @constructor
 */
function MeshNode(mesh, material) {
    Node.call(this);

    this.mBuffer = mesh;
    this.mMaterial = material;
}
_inherit(MeshNode, Node);
MeshNode.prototype.draw = function (context) {
    if (!this.mVisiable) {
        return;
    }
    this.updateMatrix();
    context.render(this, this.mBuffer, this.mMaterial);
}

module.exports = MeshNode;