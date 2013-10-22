function Node() {
    this.previous = this.next = this;
}
// ==========================
// LinkedList
// ==========================
function LinkedList() {
    this.anchor = new Node();
    this.anchor.previous = this.anchor.next = this.anchor;
    this.mCount = 0;

    this.startItor();
}
LinkedList.prototype.clear = function () {
    this.anchor.previous = this.anchor.next = this.anchor;
    this.mCount = 0;
}
LinkedList.prototype.addNode = function (cell) {
    var anchor = this.anchor;
    cell.previous = anchor.previous;
    cell.next = anchor;
    anchor.previous.next = cell;
    anchor.previous = cell;

    this.mCount++;
}
LinkedList.prototype.removeNode = function (cell) {
    cell.previous.next = cell.next;
    cell.next.previous = cell.previous;

    this.mCount--;
}
LinkedList.prototype.isAnchor = function (cell) {
    return cell === this.anchor;
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
LinkedList.prototype.toString = function () {
    return 'group {count:' + this.mCount + '}';
}
LinkedList.prototype.isEmpty = function () {
    return this.mCount == 0;
}
// ==========================
// Iterator
// ==========================
LinkedList.prototype.startItor = function () {
    this.mCursor = this.mNext = this.anchor.next;
    return this;
}
LinkedList.prototype.hasNext = function () {
    if (this.mNext === this.anchor) {
        return false;
    }
    this.mCursor = this.mNext;
    return true;
}
LinkedList.prototype.next = function () {
    this.mNext = this.mCursor.next;
    return this.mCursor;
}
LinkedList.prototype.remove = function () {
    this.mNext = this.mCursor.next;
    this.remove(this.mCursor);
}

module.exports = LinkedList;
module.exports.Node = Node;