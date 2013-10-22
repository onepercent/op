//var _LinkedList = require('core/linkedlist.js').LinkedList;
var _event = require('core/event.js');
var _Matrix = require('core/glm.js').matrix4;
var _vec3f = require('core/glm.js').vec3f;
var _MatrixStack = require('render/matrixstack.js').MatrixStack;

var mTouchEvent = _event.touchEvent;// get events data from native
var mKeyEvent = _event.keyEvent;

// ========================================================
// TouchEvent
// ========================================================
function TouchEvent(time) {
//    this._prev = null;// previous
//    this._next = null;// next
    this.time = time;
    this.button = 0;
    this.state = 0;
    this._vector = new _vec3f();
    this.vector = new _vec3f();
}
TouchEvent.prototype.init = function (buffer, offset) {
    this.button = buffer[offset];
    this.state = buffer[offset + 1];
    this._vector[0] = buffer[offset + 2];
    this._vector[1] = buffer[offset + 3];
    return this;
}
TouchEvent.prototype.toString = function () {
    return '{' + [
        'time:' + this.time,
        'button:' + this.button,
        'state:' + this.state,
        'x:' + this.vector[0],
        'y:' + this.vector[1]
    ].join(',') + '}';
}
/**
 * mapping click point from screen coordinate to node coordinate
 * @param m
 */
TouchEvent.prototype.toLocal = function (m) {
    _glm.mulMV3(this.vector, m, this._vector);
    return this;
}

// ========================================================
// Dirty Stack
// ========================================================
function DirtyStack(depth) {
    this.data = new Array(depth);
    this.cursor = 0;
    this.value = false;
    this.clear();
}
DirtyStack.prototype.push = function (a) {
    return this.data[++this.cursor] = this.value &= a;
}
DirtyStack.prototype.pop = function () {
    return this.value = this.data[--this.cursor];
}
DirtyStack.prototype.clear = function () {
    this.data[this.cursor = 0] = this.value = false;
}
DirtyStack.prototype.set = function (b) {
    this.data[this.cursor] = this.value = b;
}

// ========================================================
// Node Iterator
// ========================================================
function NodeStack() {
    this.matrix = new _Matrix();

    this.depth = 0;
    this.nodes = [];
    this.nodesCount = 0;
    this.nodesAccess = 0;
    this.matrixStack = new _MatrixStack(64);
    this.dirtyStack = new DirtyStack(64);
}
NodeStack.prototype.push = function (node) {
    node.startItor();
    this.nodes[this.depth++] = node;
    this.dirtyStack.push(node.enterNode(this.dirtyStack.value, this.matrixStack));
}
NodeStack.prototype.pop = function () {
    this.depth--;
    this.dirtyStack.pop();
    this.matrixStack.pop();
}
NodeStack.prototype.clear = function () {
    this.depth = 0;
    this.matrix.identity();
    this.dirtyStack.clear();
    this.matrixStack.clear();
}
NodeStack.prototype.startItor = function (rootnode) {
    this.clear();
    this.nodesCount = 1;
    this.nodesAccess = 0;
    this.push(rootnode);
}
NodeStack.prototype.cancelItor = function () {
    this.nodesAccess = this.nodesCount;
}
NodeStack.prototype.isEmpty = function () {
    return this.nodesAccess == this.nodesCount;
}
NodeStack.prototype.next = function () {
    var node = this.nodes[this.depth - 1];
    if (node.getIndex() == -1) {
        node.head();
        this.nodesCount += node.childrenCount();
        this.nodesAccess++;
        return node;
    }

    if (node.hasNext()) {
        this.push(node.next());
        return this.next();
    }

    this.pop();
    return this.next();
}

// ========================================================
// EventContext
// ========================================================
var _Node = require('component/node.js');
var _geometry = require('core/glm.js');
var _glm = _geometry.glm;
var _vec3f = _geometry.vec3f;

function EventContext() {
    this.events = [];
    this.buffer = new Int32Array(4 * 64);
    this.mStack = new NodeStack();
}
EventContext.prototype.startTouch = function () {
    this.pullEvents();
}
EventContext.prototype.endTouch = function () {
    this.events = [];
    this.mDirty = false;
}
EventContext.prototype.onEvent = function (camera, scene) {
    if (!this.events.length) {
        return;
    }

    var timestamp = camera.mTimestamp;

    // go throught all nodes
    var stack = this.mStack;
    stack.startItor(scene.__touchnode__);

    while (!stack.isEmpty()) {
        var node = stack.next();
        if (node.setTimeStamp(timestamp)) {
            camera.updateTouchMatrix(node.matrixInverse, node.matrix);
        }
        node.onEvent(this, stack);
    }
}
/**
 * get event as much as possiable
 */
EventContext.prototype.pullEvents = function () {
    var buf = this.buffer;
    var count = mTouchEvent.getEvents(buf);
    for (var i = 0; i < count; i++) {
        this.events.push(new TouchEvent(0).init(buf, 4 * i));
    }
}
EventContext.prototype.hasEvent = function () {
    return this.events.length > 0;
}
EventContext.prototype.pop = function () {
    return this.events.shift();
}
EventContext.prototype.peek = function () {
    return this.events[0];
}

module.exports = EventContext;