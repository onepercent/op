var clz = require('nativeclasses');
function jsImplement() {
    function Image(url) {
    }
    Image.prototype.internalFormat = 1;
    Image.prototype.type = 1;
    Image.prototype.format = 1;
    Image.prototype.width = 1;
    Image.prototype.height = 1;
}
module.exports = clz.image || jsImplement();