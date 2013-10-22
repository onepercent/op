var _inherit = require('core/inherit.js');

// ==========================
// SelecterListener
// ==========================
function SelecterListener() {
}
SelecterListener.prototype.reset = function (selectors) {
    this.mCount = 0;
    this.selectors = selectors;
    this.targets = [];
    this.path = [];
    return this;
}
SelecterListener.prototype.onVisit = function (cssnode) {
    return true;
}
SelecterListener.prototype.onNode = function (cssnode) {
    if (cssnode.depth == -1) {
        return;
    }
    if (this.selectors.match(this.path)) {
        this.targets.push(cssnode.node);
        cssnode.target = true;
    }
}
SelecterListener.prototype.onPush = function (cssnode) {
    cssnode.branches = this.mCount;
    this.path.push(cssnode);
}
SelecterListener.prototype.onPop = function (cssnode) {
    this.path.pop();
    cssnode.branches = this.mCount - cssnode.branches;
//    if (cssnode.branches == 0) {
//        return;
//    }
    if (cssnode.target) {
        cssnode.branches--;
    }
}

exports.SelecterListener = SelecterListener;
