var _glm = require('core/glm.js');
var glm = _glm.glm;
var matrix4 = _glm.matrix4;
var vector = _glm.vector3;

/**
 * @param texture ref
 * @param rotate
 * @param w width of picture
 * @param h height
 * @param ox offsetx, current picture x to the underlying texture x(0)
 * @param oy offsety
 * @param vx visiable x in picture rectangle
 * @param vy
 * @param vw visiable area width
 * @param vh
 */
function textureFrame(texture, w, h, ox, oy, vx, vy, vw, vh) {
    this.rotate = false;
    this.texture = texture;
    this.tw = texture.width();
    this.th = texture.height();

    if (arguments.length == 1) {
        this.w = this.vw = texture.picWidth();
        this.h = this.vh = texture.picHeight();
        this.ox = this.vx = 0;
        this.oy = this.vy = 0;
    } else if (arguments.length == 3) {
        this.w = this.vw = w;
        this.h = this.vh = h;
        this.ox = this.vx = 0;
        this.oy = this.vy = 0;
    } else if (arguments.length == 5) {
        this.w = this.vw = w;
        this.h = this.vh = h;
        this.ox = ox;
        this.oy = oy;
        this.vx = 0;
        this.vy = 0;
    } else {
        this.w = w;
        this.h = h;
        this.ox = ox;
        this.oy = oy;
        this.vx = vx;
        this.vy = vy;
        this.vw = vw;
        this.vh = vh;
    }
}
textureFrame.prototype.width = function () {
    return this.rotate ? this.h : this.w;
}
textureFrame.prototype.height = function () {
    return this.rotate ? this.w : this.h;
}
/**
 * get mapping matrix form texture coordinate to
 * @param m
 */
textureFrame.prototype.getMatrix = function (m) {
    if (!m) {
        m = new matrix4();
    } else {
        m.identity();
    }

    var v = new vector();
    if (this.rotate) {
        v.set(0, 0, 1);
        glm.rotate(m, -90, v);
        v.set(-1, 1, 1);
        glm.scale(m, v);
    } else {
        v.set(1, -1, 1);
        glm.scale(m, v);
        v.set(0, -this.h, 0);
        glm.translate(m, v);
    }
    return m;
}
/**
 * get xy position in percent
 *
 * @param v vector position relative to current texture
 * @param t text coordinate
 * @param rx texture coordinate
 * @param ry
 */
textureFrame.prototype.getPoint = function (v, t, rx, ry) {
    if (this.rotate) {
        var _t = ry;
        ry = rx;
        rx = 1 - _t;
    }
    var x = rx * this.w;
    var y = ry * this.h;
    v.set(x, y);
    t.set((x + this.ox) / this.tw, (y + this.oy) / this.th);
}
/**
 * rx, ry is relative to visiable area
 * @param v
 * @param t
 * @param rx
 * @param ry
 */
textureFrame.prototype.getVisiablePoint = function (v, t, rx, ry) {
    if (this.rotate) {
        var _t = ry;
        ry = rx;
        rx = 1 - _t;
    }
    var x = rx * this.vw + this.vx;
    var y = ry * this.vh + this.vy;
    v.set(x, y);
    t.set((x + this.ox) / this.tw, (y + this.oy) / this.th);
}
/**
 * get texture coordinate by given the absoult xy
 * @param v
 * @param t
 * @param x
 * @param y
 */
textureFrame.prototype.getPointAbs = function (v, t, x, y) {
    if (this.rotate) {
        var _t = y;
        y = x;
        x = this.w - _t;
    }
    v.set(x, y);
    t.set((x + this.ox) / this.tw, (y + this.oy) / this.th);
}

/**
 * return an matrix that can map 1,1 to width,height
 *
 * @param w
 * @param h
 * @param m
 * @returns {*}
 */
function rectangle(w, h, m) {
    if (!m) {
        m = new matrix4();
    } else {
        m.identity();
    }

    var v = new vector();
    v.set(w, -h, 1);
    glm.scale(m, v);
    v.set(0, -1, 0);
    glm.translate(m, v);
    return m;
}

module.exports = textureFrame;
module.exports.rectangle = rectangle;