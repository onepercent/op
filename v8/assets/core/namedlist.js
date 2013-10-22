var _LinkedList = require('core/linkedlist.js').LinkedList;
var _Node = require('core/linkedlist.js').Node;
var listId = 0;

function Iterator(list) {
    this.list = list;
    this.reset();
}
Iterator.prototype.reset = function () {
    this.nextelement = this.cursor = this.list.first();
}
Iterator.prototype.hasNext = function () {
    return !this.list.isTail(this.cursor = this.nextelement);
}
Iterator.prototype.next = function () {
    this.nextelement = this.cursor._next;
    return this.cursor.data;
}
/**
 * remove current element
 */
Iterator.prototype.remove = function () {
    this.nextelement = this.cursor._next;
    this.list.removeNode(this.cursor);
}

/**
 * named list for scheduled collection
 *
 * @param key
 * @constructor
 */
function NamedList(key) {
    this._list = new _LinkedList();
    this._key = key || '__' + (listId++) + '__';
}
NamedList.prototype.add = function (obj) {
    if (!obj) {
        return;
    }
    var node = obj[this._key];
    if (node) {
        this._list.removeNode(node);
    } else {
        node = new _Node(obj);
    }
    this._list.addNode(node);
}
NamedList.prototype.remove = function (obj) {
    var node = obj && obj[this._key];
    if (node) {
        this._list.removeNode(node);
        delete obj[this._key];
        delete node.data;
    }
}
NamedList.prototype.iterator = function () {
    return new Iterator(this._list);
}

module.exports = NamedList;
