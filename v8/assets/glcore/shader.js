var gl = require('opengl');
var file = require('core/file.js');
var _autorelease = require('core/autorelease.js');

function checkShader(shader, id) {
    var compiled = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
	//console.log('checkShader 02', compiled);
    if (!compiled) {
        var log = gl.getShaderInfoLog(shader);
        gl.deleteShader(shader);
        var exp = 'Error compiling shader:' + id + ':' + log;
        throw exp;
    }
}
function shader(id, shaderSource, shaderType) {
    var shader = gl.createShader(shaderType);
    gl.shaderSource(shader, shaderSource);
    gl.compileShader(shader);
    checkShader(shader, id);

    this._id = id;
    this._glid = shader;
    this.__shader__ = _autorelease.releaseGLShader(shader);
    this._release = false;
}
shader.prototype.getGLId = function () {
    return this._glid;
}
shader.prototype.release = function () {
    if (!this._release) {
        gl.deleteShader(this._glid);
        this._release = true;
    }
}

function create(id, shaderSource, shaderType) {
    return new shader(id, shaderSource, shaderType);
}

exports.create = create;
