var _LinkedList = require('component/selector/linkednode.js');

/**
 * wrap of ui element
 * @param node
 * @param parent
 * @constructor
 */
function CSSNode(node, parent) {
    this.children = new _LinkedList();
    this.init(node, parent);
}
CSSNode.prototype.init = function (node, parent) {
    this.node = node;
    this.parent = parent;
    this.children.clear();
    this.previous = this.next = null;

    this.branches = 0;// child branches of this node
    this.target = false;
}
CSSNode.prototype.previousSibling = function () {
    if (!this.parent) {
        return null;
    }
    var children = this.parent.children;
    if (children.count() <= 1) {
        return null;
    }
    if (children.isAnchor(this.previous)) {
        return null;
    }
    return this.previous;
}
CSSNode.prototype.nextSibling = function () {
    if (!this.parent) {
        return null;
    }
    var children = this.parent.children;
    if (children.count() <= 1) {
        return null;
    }
    if (children.isAnchor(this.next)) {
        return null;
    }
    return this.next;
}
CSSNode.prototype.removeFromParent = function () {
    if (this.parent) {
        this.parent.children.removeNode(this);
        this.parent = null;
    }
}
CSSNode.prototype.print = function () {
    printNode(this, 0);
}
function printNode(root, depth) {
    var prefix = '';
    var empty = '    ';
    depth = depth || 0;
    for (var i = 0; i < depth; i++) {
        prefix += empty;
    }

    if (root.children.isEmpty()) {
        console.log(prefix + root.node + '#' + root.target + '_' + root.branches + ':{}');
    } else {
        console.log(prefix + root.node + '#' + root.target + '_' + root.branches + ':[');
        var itor = root.children.startItor();
        while (itor.hasNext()) {
            printNode(itor.next(), depth + 1);
        }
        console.log(prefix + ']');
    }
}
CSSNode.wrap = function (node) {
    var root = new CSSNode(node, null);
    var task = [root];
    while (task.length > 0) {
        var cssnode = task.pop();
        var children = cssnode.node.children;
        if (!children || children.length == 0) {
            continue;
        } else {
            for (var i = -1, l = children.length; ++i < l;) {
                var child = new CSSNode(children[i], cssnode);
                task.push(child);
                child.parent = cssnode;
                cssnode.children.addNode(child);
            }
        }
    }
    return root;
}

module.exports = CSSNode;
