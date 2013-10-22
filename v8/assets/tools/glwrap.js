var _gl = require('opengl');
var _original = {};
var _wrap = {};

function createWrap(name, fn) {
    return function () {
        console.log('====' + name);
        return fn.apply(this, arguments);
    };
}
for (var i in _gl) {
    if (typeof _gl[i] == 'function') {
        _wrap[i] = createWrap(i, _original[i] = _gl[i]);
    }
}
exports.wrap = function () {
    for (var i in _wrap) {
        _gl[i] = _wrap[i];
    }
    console.log('wrap------>');
}
exports.unwrap = function () {
    for (var i in _wrap) {
        _gl[i] = _original[i];
    }
}