var _material = require('render/material.js');
var _inherit = require('core/inherit.js');
var _gl = require('opengl');

// create user defined material
/**
 * sprite material
 * @type {*}
 */
exports.positionTexture = _inherit(function (program, frame) {
    _material.call(this, program);
    this.frame = frame;
    this.texture = frame.texture;
}, _material, {
    use: function () {
        this.program.use();
        this.program.setUniform('u_texture', this.texture);
    },
    bindMesh: function (mesh) {
        var p = this.program;
        mesh.bindBuffer();
        p.setAttrib('a_position', mesh.accessor('p'));
        p.setAttrib('a_texCoord', mesh.accessor('t'));
    },
    bindMatrix: function (matrix) {
        this.program.setUniform('u_pvmMatrix', matrix);
    },
    create: function (program, frame) {
        return new this.constructor(program, frame);
    }
});

/**
 * drawing color block
 * @type {*}
 */
exports.positionColor = _inherit(function (program, color) {
    _material.call(this, program);
    this.color = new Float32Array(color);
}, _material, {
    use: function () {
        this.program.use();
        this.program.setUniform('u_color', this.color);
    },
    bindMesh: function (mesh) {
        var p = this.program;
        mesh.bindBuffer();
        p.setAttrib('a_position', mesh.accessor('p'));
    },
    bindMatrix: function (matrix) {
        this.program.setUniform('u_pvmMatrix', matrix);
    },
    create: function (program, color) {
        return new this.constructor(program, color);
    },
    setColor: function (color) {
        this.color.set(color);
    }
});

/**
 * position color texture
 * @type {*}
 */
exports.textColor = _inherit(function (program, atlas) {
    _material.call(this, program);
    this.atlas = atlas;
}, _material, {
    use: function () {
        this.program.use();
        this.program.setUniform('u_texture', this.atlas);
    },
    bindMesh: function (mesh) {
        mesh.bindBuffer();

        var p = this.program;
        p.setAttrib('a_position', mesh.accessor('p'));
        p.setAttrib('a_color', mesh.accessor('c'));
        p.setAttrib('a_texCoord', mesh.accessor('t'));
    },
    bindMatrix: function (matrix) {
        this.program.setUniform('u_pvmMatrix', matrix);
    },
    create: function (program, atlas) {
        return new this.constructor(program, atlas);
    }
});

/**
 * draw black text
 * position texture
 * @type {*}
 */
exports.textBlack = _inherit(function (program, atlas) {
    _material.call(this, program);
    this.atlas = atlas;
}, _material, {
    use: function () {
        this.program.use();
        this.program.setUniform('u_texture', this.atlas);
    },
    bindMesh: function (mesh) {
        mesh.bindBuffer();

        var p = this.program;
        p.setAttrib('a_position', mesh.accessor('p'));
        p.setAttrib('a_texCoord', mesh.accessor('t'));
    },
    bindMatrix: function (matrix) {
        this.program.setUniform('u_pvmMatrix', matrix);
    },
    create: function (program, atlas) {
        return new this.constructor(program, atlas);
    }
});