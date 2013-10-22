/**
 * stand for a typed buffer, handle detail operation when you work with mesh data
 * @param config
 */
function Buffer(config) {
    // require
    this.mStride = config.stride;// field in one buffer unit
    this.mCount = config.count;

    // optional
    this.mType = config && config.type || Float32Array;
    this.mElement = config && config.element;
    this.mBuffer = config.buffer || new this.mType(this.mStride * this.mCount);
};
Buffer.prototype.getElement = function (index, element) {
    if (!element) {
        element = this.mElement ? new this.mElement() : new this.mType(this.mStride);
    }
    this.mBuffer.get(element, index * this.mStride);
    return element;
};
Buffer.prototype.setElement = function (index, value) {
    this.mBuffer.set(value, index * this.mStride);
};
Buffer.prototype.clone = function () {
    return new Buffer({
        stride: this.mStride,
        count: this.mCount,
        type: this.mType,
        element: this.mElement,
        buffer: new this.mType(this.mBuffer.buffer.slice(0))
    });
}
Buffer.prototype.__defineGetter__('length', function () {
    return this.mCount;
});
/**
 * return inner buffer
 * @returns {*}
 */
Buffer.prototype.buffer = function () {
    return this.mBuffer;
}

/**
 * create an structor with given element type
 * @param config
 * @returns {Buffer}
 */
function createBuffer(config) {
    return new Buffer(config);
}

function createTypedBuffer(stride, count, type, element) {
    return new Buffer({
        stride: stride,
        count: count,
        type: type,
        element: element
    });
}

exports.Buffer = Buffer;
exports.createBuffer = createBuffer;
exports.createTypedBuffer = createTypedBuffer;
