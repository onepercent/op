var clz = require('nativeclasses');

function jsImplement() {
    function File(url) {
    }
    File.prototype.loadAsset = function (path) {
    };
    File.prototype.getContent = function () {
        return '';
    };
    File.prototype.release = function () {
    };
}
var _file = clz.file || jsImplement();

function loadContent(path) {
    var f = new _file();
    f.loadAsset(path);
    var s = f.getContent();
    f.release();

    return s;
}

module.exports = _file;
module.exports.loadContent = loadContent;