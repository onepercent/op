function Listener() {
}
Listener.prototype.reset = function (fn, bind) {
    this.fn = fn;
    this.bind = bind || null;
    return this;
}
Listener.prototype.trigger = function (target) {
    this.fn.call(this.bind, target);
}

function Event() {
}
Event.prototype.on = function (name, fn, bind) {
    var e = this[name] || (this[name] = new Listener());
    e.reset(fn, bind);
}
Event.prototype.off = function (name) {
    delete this[name];
}
Event.prototype.fire = function (name, target) {
    var e = this[name];
    if (e) {
        e.trigger(target);
    }
}

module.exports = Event;