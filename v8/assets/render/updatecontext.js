var _Timer = require('core/timer.js');

/**
 * @param itor
 */
function updateContext(itor) {
    this.mItor = itor;
    this.mTimer = new _Timer();
    this.mStride = 0;
}
/**
 * called before a new update begin
 */
updateContext.prototype.ticktack = function () {
    this.mTimer.ticktack();
    this.mStride = this.mTimer.getTimePass();
}
updateContext.prototype.stride = function () {
    return this.mStride;
}
/**
 * reset the clock
 */
updateContext.prototype.reset = function () {
    this.mTimer.reset();
}
/**
 * cancel the current update
 */
updateContext.prototype.cancel = function () {
    this.mItor.remove();
}

module.exports = updateContext;