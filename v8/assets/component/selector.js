var _Parser = require('component/selector/parser.js');
var _CSSNode = require('component/selector/cssnode.js');
var _NodeIterator = require('component/selector/nodeiterator.js');
var _listener = require('component/selector/nodelistener.js');
var _SelectorListener = _listener.SelecterListener;

var _selector = require('component/selector/selector.js');
var _SelectorGroup = _selector.SelectorGroup;

/**
 * for reusable purpase
 *
 * @param node
 * @constructor
 */
function QueryContext(node) {
    this.root = _CSSNode.wrap(node);
    this.itor = new _NodeIterator();
    this.listener = new _SelectorListener().reset();
}
QueryContext.prototype.querySelector = function (pattern) {
    var selectors = new _Parser().parse(pattern);
    if (selectors.length == 0) {
        return [];
    }
    return this.itor.childFirst(this.root, this.listener.reset(new _SelectorGroup(selectors)));
}
QueryContext.prototype.querySelectorGroup = function (group) {
    return this.itor.childFirst(this.root, this.listener.reset(group));
}

function queryContext(node, query) {
    return new QueryContext(node, query);
}

function querySelector(node, query) {
    var selectors = new _Parser().parse(query);
    if (selectors.length == 0) {
        return [];
    }

    var root = _CSSNode.wrap(node);// take a snap first
    var itor = new _NodeIterator();
    var listener = new _SelectorListener().reset(new _SelectorGroup(selectors));
    itor.childFirst(root, listener);

    return listener.targets;
}

exports.querySelector = querySelector;
exports.queryContext = queryContext;