var _inherit = require('core/inherit.js');
var _clz = require('nativeclasses');

function eventInterface() {
    function EventAccessor() {
    }

    EventAccessor.prototype = {
        /**
         * @param {Int32Array} buf
         * @return {Number}
         */
        getEvent: function (buf) {
        },
        /**
         * @param {Int32Array} buf
         * @return {Number}
         */
        getEvents: function (buf) {
        }
    }
    return EventAccessor;
}

// bridge, that can get event from cpp
var mTouchEvent = new _clz.EventAccess(16, 64) || eventInterface();
var mKeyEvent = new _clz.EventAccess(12, 64) || eventInterface();

exports.touchEvent = mTouchEvent;
exports.keyEvent = mKeyEvent;
