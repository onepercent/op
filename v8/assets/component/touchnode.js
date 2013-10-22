var _inherit = require('core/inherit.js');
var _geometry = require('core/glm.js');
var _Matrix = _geometry.matrix4;
var _Node = require('component/node.js');
var FlagTouchMatrix = _Node.prototype.FlagTouchMatrix;

var TypeEventNode = 1;
var TypeTouchNode = 1 << 1;

function EventNode(element) {
    this.element = element;
    this.children = [];
    this.parent = null;

    this.matrix = new _Matrix();// matrix to root
    this.matrixInverse = new _Matrix();// reverse of matrix
    this.mTimestamp = 0;
}
EventNode.prototype.type = TypeEventNode;
/**
 * @param time
 * @returns {boolean} return is different
 */
EventNode.prototype.setTimeStamp = function (time) {
    if (this.mTimestamp == time) {
        return false;
    }
    this.mTimestamp = time;
    return true;
}
EventNode.prototype.addChild = function (child) {
    this.children.push(child);
    child.parent = this;
    child.element.addFlag(FlagTouchMatrix);
}
EventNode.prototype.removeChild = function (child) {
    var index = this.indexOf(child);
    if (index != -1) {
        this.children.splice(index, 1);
        child.parent = null;
    }
}
EventNode.prototype.moveToEnd = function (index, child) {
    this.children.splice(index, 1);
    this.children.push(child);
}
EventNode.prototype.indexOf = function (child) {
    for (var i = -1, arr = this.children, l = arr.length; ++i < l;) {
        if (arr[i] === child) {
            return i;
        }
    }
    return -1;
}
EventNode.prototype.enterNode = function (dirty, stack) {
    var nDirty = this.element.getRemove(FlagTouchMatrix);
    if (nDirty || dirty) {
        this.matrix.set(stack.push(this.element.getMatrix(this.matrix)));
    } else {
        stack.pushNext(this.matrix);
    }
    return nDirty;
}
EventNode.prototype.isReachable = function () {
    return this.type == TypeTouchNode || this.children.length > 0;
}
EventNode.prototype.isTouchable = function () {
    return this.type == TypeTouchNode;
}
EventNode.prototype.startItor = function () {
    this.index = -1;
}
EventNode.prototype.head = function () {
    this.index = 0;
}
EventNode.prototype.hasNext = function () {
    return this.index < this.children.length;
}
EventNode.prototype.next = function () {
    return this.children[this.index++];
}
EventNode.prototype.childrenCount = function () {
    return this.children.length;
}
EventNode.prototype.getIndex = function () {
    return this.index;
}
EventNode.prototype.print = function (prefix) {
    prefix = prefix || '';
    if (this.children.length == 0) {
        console.log(prefix + this.element);
        return;
    }

    var header = prefix + '    ';
    console.log(prefix + this.element + ': {');
    for (var i = -1, arr = this.children, l = arr.length; ++i < l;) {
        arr[i].print(header);
    }
    console.log(prefix + '}');
}
/**
 * @param events
 * @param stack
 */
EventNode.prototype.onEvent = function (events, stack) {
    if (!events.hasEvent()) {
        stack.cancelItor();
    }
}

function TouchNode(element, delegate) {
    EventNode.call(this, element);
    this.delegate = delegate || element;
}
_inherit(TouchNode, EventNode);
TouchNode.prototype.type = TypeTouchNode;
TouchNode.prototype.onEvent = function (events, stack) {
    while (events.hasEvent()) {
        var event = events.peek().toLocal(this.matrixInverse);
        if (this.delegate.onTouch(event, stack)) {
            events.pop();
        } else {
            return;
        }
    }
    stack.cancelItor();
}

exports.EventNode = EventNode;
exports.TouchNode = TouchNode;
