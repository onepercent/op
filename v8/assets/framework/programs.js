var _collection = require('core/collection.js');
var _shader = require('glcore/shader.js');
var _program = require('glcore/program.js');
var _file = require('core/file.js');
var _gl = require('opengl');
var _materials = require('framework/materials.js');

function getFileId(p) {
    var start = p.lastIndexOf('/') + 1;
    return p.slice(start);
}

/**
 * all shaders
 * @type {*}
 */
var shaders = _collection.createCollection({
    create: function (id, path, type) {
        return _shader.create(id, _file.loadContent(path), type);
    }
});

/**
 * all programs
 * @type {*}
 */
var programs = _collection.createCollection({
    create: function (id, vpath, fpath, material) {
        var vShader = shaders.findopt(getFileId(vpath), vpath, _gl.VERTEX_SHADER);
        var fShader = shaders.findopt(getFileId(fpath), fpath, _gl.FRAGMENT_SHADER);
        if (!vShader || !fShader) {
            return null;
        }

        // bind a material creator with closure
        var p = _program.create(id, vShader, fShader);
        var slice = Array.prototype.slice;
        p.material = function () {
            var mpro = material.prototype;
            return mpro.create.apply(mpro, [this].concat(slice.call(arguments, 0)));
        }
        return p;
    }
});

exports.positionTexture = programs.findopt('positionTexture', 'shader/position_texture.vert', 'shader/position_texture.frag', _materials.positionTexture);
exports.positionColor = programs.findopt('positionColor', 'shader/position_color.vert', 'shader/position_color.frag', _materials.positionColor);
exports.textColor = programs.findopt('textColor', 'shader/position_color_texture.vert', 'shader/position_color_texturealpha.frag', _materials.positionColor);
exports.textBlack = programs.findopt('textBlack', 'shader/position_texture.vert', 'shader/position_texturealpha.frag', _materials.textBlack);
