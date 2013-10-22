var _UIContainer = require('component/uicontainer.js');
var _inherit = require('core/inherit.js');

function Scene() {
    _UIContainer.call(this);
    this.mTimestamp = 0;
}
_inherit(Scene, _UIContainer);
Scene.prototype.mTag = 'scene';
Scene.prototype.__elementType |= Scene.prototype.ElementTypeScene;
Scene.prototype.update = function (context) {
}
Scene.prototype.onTouch = function (context) {
}
Scene.prototype.onSizeChange = function (w, h) {
}
Scene.prototype.onRegister = function (global) {
    global.scheduleRender.schedule(this);
    global.scheduleUpdate.schedule(this);
    global.scheduleEvent.schedule(this);
}

/**
 * create a anonymous scene class, this scene may used many times
 *
 * @param init
 * @param props
 * @returns {*}
 */
function createScene(init, props) {
    var clz = _inherit(function (id) {
        Scene.call(this, id);
    }, Scene, props);
    var instance;
    clz.newInstance = function (id) {
        var instance = new clz(id);
        init.apply(instance, arguments.length > 0 ? Array.prototype.slice.call(arguments, 1) : []);
        return instance;
    }
    clz.singleInstance = function () {
        return instance || (instance = clz.newInstance.call(this, arguments));
    }
    return clz;
}

exports.Scene = Scene;
exports.createScene = createScene;