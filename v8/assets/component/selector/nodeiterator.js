// ==========================
// Node Iterator
// ==========================
function NodeIterator() {
    this.status = 0;// 0 return curent point, 1 find child, if any
    this.nodes = 0;
    this.reached = 0;
    this.stack = [];
}
NodeIterator.prototype.init = function (cssroot) {
    this.status = 0;
    this.nodes = 1;
    this.reached = 0;
    this.stack = [cssroot];
}
NodeIterator.prototype.push = function (listener, node) {
    this.stack.push(node);
    listener.onPush(node);
}
NodeIterator.prototype.pop = function (listener) {
    var p = this.stack.pop();
    listener.onPop(p);
    return p;
}
NodeIterator.prototype.peek = function () {
    return this.stack[this.stack.length - 1];
}
NodeIterator.prototype.hasNext = function () {
    return this.nodes > this.reached;
}
NodeIterator.prototype.nodeFirst = function (cssroot, listener) {
    this.init(cssroot);
    listener.onPush(cssroot);

    while (this.hasNext()) {
        var n = this.peek();

        switch (this.status) {
            case 0:
                this.reached++;
                listener.onNode(n);
                n.children.startItor();
                this.nodes += n.children.count();
                this.status = 1;
                break;

            case 1:
                if (n.children.hasNext()) {
                    var child = n.children.next();
                    if (!listener.onVisit(child)) {
                        continue;
                    }
                    this.push(listener, child);
                    this.status = 0;
                    continue;
                }

                this.pop(listener);
                break;
        }
    }
    while (this.stack.length > 0) {
        this.pop(listener);
    }
}
NodeIterator.prototype.childFirst = function (cssroot, listener) {
    this.init(cssroot);
    listener.onPush(cssroot);

    while (this.hasNext()) {
        var n = this.peek();

        switch (this.status) {
            case 0:
                n.children.startItor();
                this.nodes += n.children.count();
                this.status = 1;
                break;

            case 1:
                if (n.children.hasNext()) {
                    var child = n.children.next();
                    if (!listener.onVisit(child)) {
                        continue;
                    }
                    this.push(listener, child);
                    this.status = 0;
                    continue;
                }
                listener.onNode(n);
                this.pop(listener);
                this.reached++;
                break;
        }
    }
}

module.exports = NodeIterator;
