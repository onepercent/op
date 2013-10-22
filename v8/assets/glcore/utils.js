var gl = require('opengl');

function getGLType(type) {
    switch (type) {
        case Uint8Array:
            return gl.UNSIGNED_BYTE;
        case Int8Array:
            return gl.BYTE;
        case Uint16Array:
            return gl.UNSIGNED_SHORT;
        case Int16Array:
            return gl.SHORT;
        case Uint32Array:
            return gl.UNSIGNED_INT;
        case Int32Array:
            return gl.INT;
        case Float32Array:
            return gl.FLOAT;
        default:
        case Float64Array:
            throw("unhandled type:" + type);
    }
}
function getByteSize(type) {
    switch (type) {
        case Uint8Array:
        case Int8Array:
            return 1;
        case Uint16Array:
        case Int16Array:
            return 2;
        case Uint32Array:
        case Int32Array:
        case Float32Array:
            return 4;
        case Float64Array:
            return 8;
    }
    return 1;
}

exports.getGLType = getGLType;
exports.getByteSize = getByteSize;