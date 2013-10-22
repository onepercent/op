function inherit(sub, super_, props) {
    sub.prototype = Object.create(super_.prototype);
    if (props) {
        for (var i in props) {
            sub.prototype[i] = props[i];
        }
    }
    sub.prototype.constructor = sub;
    return sub;
}
module.exports = inherit;