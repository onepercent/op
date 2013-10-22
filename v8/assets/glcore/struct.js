var getGLType = require('glcore/utils.js').getGLType;
var getByteSize = require('glcore/utils.js').getByteSize;
var inherit = require('core/inherit.js');
var gl = require('opengl');

/**
 * stands for a struct field
 *
 * @param name
 * @param type
 * @param size
 * @param normalize
 * @constructor
 */
function Accessor(name, type, size, normalize) {
    this.name = name;
    this.type = type;
    this.glType = getGLType(type);
    this.size = size;
    this.normalize = normalize;
    this.byteOffset = 0;
    this.byteLength = getByteSize(type) * size;
}
/**
 * @param {meshBuffer} mesh
 * @param loc
 */
Accessor.prototype.bindVertex = function (mesh, loc) {
    gl.enableVertexAttribArray(loc);
    if (mesh.isVbo()) {
        gl.vertexAttribPointer(loc, this.size, this.glType, this.normalize, mesh.bytestride, this.byteOffset);
    } else {
        gl.vertexAttribPointer(loc, this.size, this.glType, this.normalize, mesh.bytestride, mesh.buffer().subarray(this.byteOffset));
    }
}
Accessor.prototype.createBuffer = function (bytebuffer) {
    return new this.type(bytebuffer, this.byteOffset, this.size);
}
// ========================================================
/**
 * use as super class of anonymous class of instance
 */
function structSuper() {
    var mBuffer = new ArrayBuffer(this.byteLength);
    var arrayAccess = this.arrayAccess;

    var mFieldMap = {};
    for (var i = 0, l = arrayAccess.length; i < l; i++) {
        var p = arrayAccess[i];
        var acc = p.createBuffer(mBuffer);

        mFieldMap[i] = acc;
        mFieldMap[p.name || i] = acc;
    }

    this.mBuffer = mBuffer;
    this.mFieldMap = mFieldMap;
}
structSuper.prototype.field = function (name) {
    return this.mFieldMap[name];
}
structSuper.prototype.fields = function () {
    return this.mFieldMap;
}
structSuper.prototype.buffer = function () {
    return this.mBuffer;
}
// ========================================================
/**
 * unit of a mix buffer
 *
 * @param byteLength
 * @param parts
 */
function structInst(byteLength, arrayAccess) {
    this.byteLength = byteLength;
    this.arrayAccess = arrayAccess;
    structSuper.call(this);
}
inherit(structInst, structSuper);
// ========================================================
/**
 * helper class of creating a mix buffer structor
 */
function structBuilder() {
    this.accessor = [];
}
/**
 *
 * @param name
 * @param type the container type
 * @param size
 * @returns {*}
 */
structBuilder.prototype.add = function (name, type, size, nor) {
    this.accessor.push(new Accessor(name, type, size, nor || false));
    return this;
}
/**
 * calculate the elemenet's strides
 *
 * @param bufAccess
 * @param arrayAccess
 * @returns {number}
 */
structBuilder.prototype.initBufMap = function () {
    var byteLength = 0;
    for (var i = 0, as = this.accessor, length = as.length; i < length; i++) {
        var p = as[i];
        p.byteOffset = byteLength;
        byteLength += p.byteLength;
    }
    return byteLength;
}
/**
 * create an instance of current class, you must hold this refer for futher use.
 * @returns {structInst}
 */
structBuilder.prototype.create = function () {
    var byteLength = this.initBufMap();
    var arrayAccess = this.accessor;
    return new structInst(byteLength, arrayAccess);
}
/**
 * return a class of this mix buffer
 * @returns {*}
 */
structBuilder.prototype.createClass = function () {
    var byteLength = this.initBufMap();
    var arrayAccess = this.accessor;
    var clz = inherit(
        function () {
            structSuper.call(this);
        }, structSuper, {
            byteLength: byteLength,
            arrayAccess: arrayAccess
        }
    );
    return clz;
}
structBuilder.prototype.createBuffer = function (count) {
    return this.createClass().createBuffer.apply(this, arguments);
}

function createStruct() {
    return new structBuilder();
}

exports.createStruct = createStruct;// used as basic of mesh buffer
