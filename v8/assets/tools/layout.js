var _geometry = require('core/glm.js');
var _glm = _geometry.glm;
var _v3 = _geometry.vec3f;

// ========================================================
// Point to Local
// ========================================================
function getPointRelativeToLocal(node, v) {
    v = new _v3(v);
    v.mul(node.mSize);
    v.sub(node.mCenter);
    return v;
}
function getPointAbsoultToLocal(node, v) {
    v = new _v3(v);
    v.sub(node.mCenter);
    return v;
}
// ========================================================
// Local Point to World
// ========================================================
function localToWorld(node, v) {
    var m = node.getMatrix();
    _glm.mulMV3(v, m, v);
    return v;
}


// ========================================================
// Adapters
// ========================================================
function getPointRelative(node, v) {
    v = getPointRelativeToLocal(node, v);
    return localToWorld(node, v);
}
function getPointAbsoult(node, v) {
    v = getPointAbsoultToLocal(node, v);
    return localToWorld(node, v);
}
function getPointLocal(node, v) {
    return localToWorld(node, new _v3(v));
}

// ========================================================
// Export Adapters
// ========================================================
function getLayoutTo(transSrc, transDest) {
    if (!transDest) {
        transDest = transSrc;
    }
    function layoutTo(fnode, v1, tnode, v2, voffset) {
        var fromVec = transSrc(fnode, v1);
        var toVec = transDest(tnode, v2);
        toVec.sub(fromVec);
        if (voffset) {
            toVec.add(voffset);// append offset
        }

        fnode.translate(toVec);
    }

    // function (fnode, v1, tnode, v2)
    // function (fnode, v1, tnode, v2, v3)
    // function (fnode, x1, y1, tnode, x2, y2)
    // function (fnode, x1, y1, tnode, x2, y2, ox, oy)
    return function () {
        var a = arguments;
        switch (arguments.length) {
            case 4:
            case 5:
                layoutTo.apply(this, arguments);
                break;
            case 6:
                layoutTo(a[0], new _v3(a[1], a[2]), a[3], new _v3(a[4], a[5]));
                break;
            case 8:
                layoutTo(a[0], new _v3(a[1], a[2]), a[3], new _v3(a[4], a[5]), new _v3(a[6], a[7]));
                break;
            default :
                console.log('LayoutUtil.layoutTo arguments number [' + a.length + '] not supported, please check');
                break;
        }
    }
}
function getLayout(trans) {
    // function (fnode, p1, p2)
    // function (fnode, x1, y1, x2, y2)
    function layout(fnode, pFrom, pTo) {
        var fromVec = trans(fnode, pFrom);
        fromVec.sub(pTo);
        fromVec.scale(-1);
        fnode.translate(fromVec);
    }

    // function (fnode, p1, p2)
    // function (fnode, x1, y1, x2, y2)
    return function () {
        var a = arguments;
        switch (arguments.length) {
            case 3:
                layout(a[0], a[1], a[2]);
                break;
            case 5:
                layout(a[0], new _v3(a[1], a[2]), new _v3(a[3], a[4]));
                break;
            default :
                console.log('LayoutUtil.layout arguments number [' + a.length + '] not supported, please check');
                break;
        }
    }
}
function pointToLocal(getLocal) {
    // function (fnode, p)
    // function (fnode, x, y)
    return function () {
        var a = arguments;
        switch (arguments.length) {
            case 2:
                return getLocal(a[0], a[1]);
            case 3:
                return getLocal(a[0], new _v3(a[1], a[2]));
            default :
                console.log('LayoutUtil.localPoint arguments number not supported, please check');
                break;
        }
    }
}
function pointToWorld(getLocal) {
    // function (fnode, p)
    // function (fnode, x, y)
    return function () {
        var a = arguments;
        switch (arguments.length) {
            case 2:
                return localToWorld(a[0], getLocal(a[0], a[1]));
            case 3:
                return localToWorld(a[0], getLocal(a[0], new _v3(a[1], a[2])));
            default :
                console.log('LayoutUtil.localPoint arguments number not supported, please check');
                break;
        }
    }
}

// layout using world coordinate
exports.relative = {
    layoutTo: getLayoutTo(getPointRelative),
    layout: getLayout(getPointRelative),
    localPoint: pointToLocal(getPointRelativeToLocal),
    worldPoint: pointToWorld(getPointRelativeToLocal),
    local: {
        layoutTo: getLayoutTo(getPointRelative, getPointRelativeToLocal)
    }
}
exports.absolute = {
    layoutTo: getLayoutTo(getPointAbsoult),
    layout: getLayout(getPointAbsoult),
    localPoint: pointToLocal(getPointAbsoultToLocal),
    worldPoint: pointToWorld(getPointAbsoultToLocal),
    local: {
        layoutTo: getLayoutTo(getPointAbsoult, getPointAbsoultToLocal)
    }
}
// layout using object coordinate
exports.local = {
    layoutTo: getLayoutTo(getPointLocal),
    layout: getLayout(getPointLocal),
    worldPoint: pointToWorld(getPointRelativeToLocal)
}
