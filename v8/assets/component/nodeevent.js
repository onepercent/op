var _Element = require('component/element.js').prototype;

// ========================================================
// Listener
// ========================================================
function onNodeAdd(parent, child) {
    var childTouch = child.__touchnode__;
    if (!childTouch.isReachable()) {
        return;
    }

    var parentTouch = parent.__touchnode__;
    parentTouch.addChild(childTouch);

    parent = parent.mParent;
    childTouch = parentTouch;
    while (parent) {
        // if parent contains child
        parentTouch = parent.__touchnode__;
        if (parentTouch.indexOf(childTouch) != -1) {
            break;
        }

        // if parent is already touchable or parent is root of event dispatch
        if (parentTouch.isReachable() || parent.isElementType(_Element.ElementTypeScene)) {
            parentTouch.addChild(childTouch);
            break;
        }

        // make sure parent has a reference of child, and check parent's parent
        parentTouch.addChild(childTouch);
        childTouch = parentTouch;
        parent = parent.mParent;
    }
}
function onNodeRemove(parent, child) {
    var parentTouch = parent.__touchnode__;
    var childTouch = child.__touchnode__;
    parentTouch.removeChild(childTouch);

    parent = parent.mParent;
    childTouch = parentTouch;
    while (parent) {
        if (childTouch.isReachable()) {
            break;
        }

        parentTouch = parent.__touchnode__;
        parentTouch.removeChild(childTouch);
        childTouch = parentTouch;
        parent = parent.mParent;
    }
}
function onNodeMove(from, to, child) {
    if (from === to) {
        var childTouch = child.__touchnode__;
        var parentTouch = parent.__touchnode__;
        var index = parentTouch.indexOf(childTouch);
        if (index != -1) {
            parentTouch.moveToEnd(index, childTouch);
        }
    } else {
        onNodeRemove(from, child);
        onNodeAdd(to, child);
    }
}

exports.onNodeAdd = onNodeAdd;
exports.onNodeRemove = onNodeRemove;
exports.onNodeMove = onNodeMove;