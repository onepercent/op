var _inherit = require('core/inherit.js');

// ==========================
// Selector SuperClass
// ==========================
function Selector() {
    this.position = 0;
    this.index = 0;
    this.group = null;
    this.type = ' ';
}
Selector.prototype.match = function (cssnode) {
    return false;
};
/**
 * next element is exactly after current element
 * @returns {boolean}
 */
Selector.prototype.isTight = function () {
    return false;
};
Selector.prototype.getNodeOffset = function () {
    return this.group.width - this.position;
};
Selector.prototype.getSelectorOffset = function () {
    return this.group.end - this.index - 1;
};
Selector.prototype.getStride = function () {
    return 1;
};

// ==========================
// TypeSelector * | div | image
// ==========================
function TypeSelector(type) {
    Selector.call(this);
    this.type = type;
    if (type == '*') {
        this.any = true;
    }
}
_inherit(TypeSelector, Selector);
TypeSelector.prototype.match = function (cssnode) {
    return this.any || cssnode.node.mTag == this.type;
};
TypeSelector.prototype.toString = function () {
    return '{type:' + this.type + '}';
};

// ==========================
// PseudoSelector :focus
// ==========================
function PseudoSelector(selector, pseudo) {
    Selector.call(this);
    this.selector = selector;
    this.pseudo = pseudo;
}
_inherit(PseudoSelector, Selector);
PseudoSelector.prototype.setPseudo = function (pseudo) {
    this.pseudo = pseudo;
};
PseudoSelector.prototype.matchProp = function () {
    return true;
};
PseudoSelector.prototype.match = function (node) {
    return this.selector.match(node) && this.matchProp(node);
};
PseudoSelector.prototype.toString = function () {
    return this.selector + ':' + this.pseudo;
};

// ==========================
// AttributeSelector []
// ==========================
function AttributeSelector(selector) {
    Selector.call(this);
    this.selector = selector;
    this.attrs = [];
}
_inherit(AttributeSelector, Selector);
AttributeSelector.prototype.matchProp = function () {
    return true;
};
AttributeSelector.prototype.addAttribute = function (attr) {
    this.attrs.push(attr);
};
AttributeSelector.prototype.match = function (node) {
    return this.selector.match(node) && this.matchProp(node);
};
AttributeSelector.prototype.toString = function () {
    var str = this.selector.toString();
    for (var i = -1, attrs = this.attrs, l = attrs.length; ++i < l;) {
        str += '[' + attrs[i] + ']';
    }
    return str;
};

// ==========================
// Adjacentselectors E + F
// ==========================
function Adjacentselectors(selector) {
    Selector.call(this);
    this.selector = selector;
}
_inherit(Adjacentselectors, Selector);
Adjacentselectors.prototype.matchProp = function () {
    return true;
};
Adjacentselectors.prototype.match = function (cssnode) {
    var previous = cssnode.previousSibling();
    return previous && this.selector.match(previous);
};
Adjacentselectors.prototype.toString = function () {
    return this.selector + '+';
};
Adjacentselectors.prototype.isTight = function () {
    return true;
};
Adjacentselectors.prototype.getStride = function () {
    return 0;
};

// ==========================
// ChildSelector E > F
// ==========================
function ChildSelector(selector) {
    Selector.call(this);
    this.selector = selector;
}
_inherit(ChildSelector, Selector);
ChildSelector.prototype.match = function (cssnode) {
    return this.selector.match(cssnode);
};
ChildSelector.prototype.toString = function () {
    return this.selector + '>';
};
ChildSelector.prototype.isTight = function () {
    return true;
};

// ==========================
// ClassSelector .
// ==========================
function ClassSelector(selector, pclass) {
    Selector.call(this);
    this.selector = selector;
    this.mClass = pclass;
}
_inherit(ClassSelector, Selector);
ClassSelector.prototype.setClass = function (clz) {
    this.mClass = clz;
};
ClassSelector.prototype.match = function (cssnode) {
    return cssnode.node.hasClass(this.mClass) && this.selector.match(cssnode);
};
ClassSelector.prototype.toString = function () {
    return this.selector + '.' + this.mClass;
};

// ==========================
// IdSelector #
// ==========================
function IdSelector(selector, id) {
    Selector.call(this);
    this.selector = selector;
    this.id = id;
}
_inherit(IdSelector, Selector);
IdSelector.prototype.setId = function (id) {
    this.id = id;
};
IdSelector.prototype.match = function (cssnode) {
    return cssnode.node.getId() == this.id && this.selector.match(cssnode);
};
IdSelector.prototype.toString = function () {
    return this.selector + '#' + this.id;
};

// ==========================
// GroupSelector
// ==========================
function Group(start) {
    this.start = start;
    this.end = start + 1;
    this.width = 0;
}

function SelectorGroup(selectors) {
    this.selectors = selectors;
    this.width = 0;

    var group = new Group(0);
    for (var i = 0, coll = this.selectors, l = coll.length; i < l; i++) {
        var sel = coll[i];
        sel.index = i;
        sel.group = group;
        sel.position = group.width;
        group.width += sel.getStride();
        group.end = i + 1;

        this.width += sel.getStride();

        // create new selector
        if (!sel.isTight() && i < l - 1) {
            group = new Group(group.end);
        }
    }
}
SelectorGroup.prototype.match = function (path) {
    if (path.length < this.width) {
        return false;
    }

    var sels = this.selectors;
    var selindex = sels.length - 1;
    var sel = sels[selindex];
    var nodeindex = path.length - 1 + sel.getStride();// after the last selector
    var matchbegin = nodeindex;
    var success = true;

    while (selindex > -1) {
        sel = sels[selindex];
        nodeindex -= sel.getStride();// move to current selector
        if (nodeindex == -1) {
            break;
        }

        if (sel.match(path[nodeindex])) {
            selindex--;
        } else {
            nodeindex += sel.getNodeOffset();
            selindex += sel.getSelectorOffset();
            if (nodeindex == matchbegin) {
                success = false;
                break;
            }
            nodeindex--;// previous node
        }
    }
    return success && selindex == -1;
}

exports.Selector = Selector;
exports.IdSelector = IdSelector;
exports.ClassSelector = ClassSelector;
exports.ChildSelector = ChildSelector;
exports.PseudoSelector = PseudoSelector;
exports.TypeSelector = TypeSelector;
exports.AttributeSelector = AttributeSelector;
exports.Adjacentselectors = Adjacentselectors;

exports.SelectorGroup = SelectorGroup;
