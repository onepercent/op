var clz = require('nativeclasses');

function atlasImplement() {
    /**
     * @param width
     * @param height
     * @param depth
     * @constructor
     */
    function Atlas(width, height, depth) {
    }

    Atlas.prototype.release = function () {
    };
    return Atlas;
}
function fontImplement() {
    function Font(mAtlas, font, size) {
    }

    Font.prototype.release = function () {
    };
    Font.prototype.outline_type = function () {
    };
    Font.prototype.outline_thickness = function () {
    };
    /**
     * @param {string} text
     * @param {Float32Array} widths
     */
    Font.prototype.measure = function (text, widths) {
    };
    /**
     * @param {string} text
     * @param {Float32Array} widths
     */
    Font.prototype.glyphs = function (text, glyphs) {
    };
    Font.prototype.height = 0;
    Font.prototype.ascender = 0;
    Font.prototype.descender = 0;

    return Font;
}
clz.atlas.bindToUnit = function () {
    console.log('clz.atlas.bindToUnit');
}

exports.font = clz.font || fontImplement();
exports.atlas = clz.atlas || atlasImplement();
