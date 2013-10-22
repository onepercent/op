// ==========================
// Node
// ==========================
function Node(data) {
    this.next = this.previous = null;
    this.data = data;
    this.mList = null;
}
Node.prototype.toString = function () {
    return '[LinkedList.Node ' + this.data + ']';
}

// ==========================
// Iterator
// ==========================
//function Iter(group) {
//    // start: element before cursor
//    this.mNext = this.mStart = this.mEnd = null;
//    this.group = group;
//}
///**
// * both start and end are not included
// * @param start
// * @param end
// * @returns {*}
// */
//Iter.prototype.init = function (start, end) {
//    this.mNext = this.mStart = start;
//    this.mEnd = end;
//    return this;
//}
//Iter.prototype.hasNext = function () {
//    this.mStart = this.mNext;
//    return this.mStart.next !== this.mEnd;
//}
//Iter.prototype.next = function () {
//    this.mNext = this.mStart.next;
//    return this.mStart.next;
//}
//Iter.prototype.remove = function () {
//    this.group.remove(this.mStart.next);
//}

function Iter(group) {
    // start: element before cursor
    this.mNext = this.mStart = this.mEnd = null;
    this.group = group;
}
/**
 * ether start nor end are not included
 * @param start
 * @param end
 * @returns {*}
 */
Iter.prototype.init = function (start, end) {
    this.mStart = this.mNext = start.next;
    this.mEnd = end;
    return this;
}
Iter.prototype.hasNext = function () {
    if (this.mNext === this.mEnd) {
        return false;
    }
    this.mStart = this.mNext;
    return true;
}
Iter.prototype.next = function () {
    this.mNext = this.mStart.next;
    return this.mStart;
}
Iter.prototype.remove = function () {
    this.mNext = this.mStart.next;
    this.group.remove(this.mStart);
}

// ==========================
// Group
// ==========================
function LinkedList() {
    this.anchor = new Node(this);
    this.anchor.mList = this;
    this.anchor.previous = this.anchor.next = this.anchor;
    this.iter = new Iter(this);
    this.mCount = 0;
}
LinkedList.prototype.iterator = function () {
    return this.iter.init(this.anchor, this.anchor);
}
LinkedList.prototype.clear = function () {
    var start = this.anchor;
    while ((start = start.next) != this.anchor) {
        start.mList = null;
    }
    this.anchor.previous = this.anchor.next = this.anchor;
    this.mCount = 0;
}
LinkedList.prototype.add = function (cell) {
    var p = cell.mList;
    if (p) {
        if (p === this) {
            return;
        } else {
            // a node can only belongs to one list
            p.remove(cell);
        }
    }
    this.__insert(cell);
}
LinkedList.prototype.__insert = function (cell) {
    var anchor = this.anchor;
    cell.previous = anchor.previous;
    cell.next = anchor;
    anchor.previous.next = cell;
    anchor.previous = cell;
    this.mCount++;

    cell.mList = this;
}
LinkedList.prototype.remove = function (cell) {
    cell.previous.next = cell.next;
    cell.next.previous = cell.previous;

    cell.mList = null;
    this.mCount--;
}
/**
 * move node from one group to another
 * @param g
 */
LinkedList.prototype.merge = function (g) {
    if (g.mCount == 0 || g === this) {
        return;
    }

    var start = g.anchor.next;
    var cell = start;
    var anchor = this.anchor;

    anchor.previous.next = cell;
    cell.previous = anchor.previous;

    cell = g.anchor.previous;
    cell.next = anchor;
    anchor.previous = cell;

    g.anchor.next = g.anchor.previous = g.anchor;
    this.mCount += g.mCount;
    g.mCount = 0;
//    g.onMerge();

    while (start != anchor) {
        start.mList = this;
        start = start.next;
    }
}
LinkedList.prototype.count = function () {
    return this.mCount;
}
LinkedList.prototype.first = function () {
    if (this.mCount == 0) {
        return null;
    }
    return this.anchor.next;
}
LinkedList.prototype.last = function () {
    if (this.mCount == 0) {
        return null;
    }
    return this.anchor.previous;
}
//LinkedList.prototype.onMerge = function () {
//}
LinkedList.prototype.toString = function () {
    return 'group {count:' + this.mCount + '}';
}

module.exports = LinkedList;
module.exports.Node = Node;