var gl = require('opengl');
var clz = require('nativeclasses');
var inherit = require('core/inherit.js');
var shader = require('glcore/shader.js');
var _autorelease = require('core/autorelease.js');

var SHOW_UNDEFINED = true;

/**
 * handle for vector and matrix parameters
 *
 * @param loc param location
 * @param glfn glUniform function name
 */
function shaderParam(loc, glfn) {
    this.loc = loc;
    this.fn = glfn;
    this._data = 0;
}
shaderParam.prototype.data = function () {
    if (arguments.length > 0) {
        this._data = arguments[0];
    } else {
        return this._data;
    }
}
shaderParam.prototype.upload = function (d) {
    if (d != undefined) {
        this.fn(this.loc, this._data = d);
    } else {
        this.fn(this.loc, this._data);
    }
}
shaderParam.prototype.loc = function () {
    return this.loc;
}

/**
 * a specific transpose param with uniformMatrix*fv
 *
 * @param loc
 * @param glfn
 * @param transpose
 */
function matrixParam(loc, glfn, transpose) {
    shaderParam.apply(this, arguments);
    this.transpose = transpose || false;
}
inherit(matrixParam, shaderParam);
matrixParam.prototype.upload = function (d) {
    if (d != undefined) {
        this.fn(this.loc, this.transpose, this._data = d);
    } else {
        this.fn(this.loc, this.transpose, this._data);
    }
}
/**
 * textureParam was created with a textureId with it
 *
 * @param loc
 * @param glfn
 * @param unit
 */
function textureParam(loc, glfn, unit) {
    shaderParam.apply(this, arguments);
    this.unit = unit;
}
inherit(textureParam, shaderParam);
textureParam.prototype.upload = function (d) {
    this.fn(this.loc, this.unit);
    if (d != undefined) {
        (this._data = d).bindToUnit(this.unit);
    } else {
        this._data.bindToUnit(this.unit);
    }
}

// ==========================
// --- attribute param ------
// ==========================
/**
 * class used for binding buffer as attribute of program
 * attribute is very large, and it may be changed frqnenctly, so there is no need to cash them
 * @param index
 */
function attributeParam(index) {
    this.index = index;
}
attributeParam.prototype.loc = function () {
    return this.index;
}
attributeParam.prototype.upload = function (b) {
    b.bindVertex(this.index);
}
/**
 * key an array of attribute location
 *
 * @param names
 * @param locs
 */
function attrSet(names, locs) {
    this.names = names;
    this.locs = locs;
}
attrSet.prototype.loc = function () {
    return this.locs;
}
attrSet.prototype.upload = function (mesh) {
    mesh.bindVertex(this.locs);
}

/**
 * upload data to shader struct
 */
function structParam() {
    this.fields = {};
}
structParam.prototype.setField = function (name, setter) {
    var index = name.indexOf('.');
    if (index != -1) {
        var n = name.slice(0, index);
        var f = this.fields[n] || (this.fields[n] = new structParam());
        f.setField(name.slice(index + 1), setter);
    } else {
        this.fields[name] = setter;
    }
}
structParam.prototype.getField = function (name) {
    var index = name.indexOf('.');
    if (index != -1) {
        var n = name.slice(0, index);
        return this.fields[n] && this.fields[n].getField(name.slice(index + 1));
    } else {
        return this.fields[name];
    }
}
structParam.prototype.data = function (data) {
    for (var i in data) {
        var f = this.fields[i];
        if (f) {
            f.data(data[i]);
        }
    }
}
structParam.prototype.upload = function (data) {
    for (var i in data) {
        var f = this.fields[i];
        if (f) {
            f.upload(data[i]);
        }
    }
}

/**
 * bind a new attribute getter
 * @param obj
 * @param name
 * @param setter
 */
function setUniform(obj, name, setter) {
    obj[name] = setter;
    var index = name.indexOf('.');
    if (index == -1) {
        return;
    }

    var n = name.slice(0, index);
    var s = obj[n] || (obj[n] = new structParam());
    s.setField(name.slice(index + 1), setter);
}
function getUniform(obj, name) {
    var f = obj[name];
    if (f) {
        return f;
    }
    var index = name.indexOf('.');
    if (index == -1) {
        return null;
    }

    var n = name.slice(0, index);
    return obj[n] && obj[n].getField(name.slice(index + 1));
}

function createUniformSetter(program, info) {
    var loc = gl.getUniformLocation(program, info.name);
    var s;
    var size = info.size;
    var textureCount = 0;
    switch (info.type) {
        // -----------------
        case gl.FLOAT:
            if (size > 1) {
                s = new shaderParam(loc, gl.uniform1fv);
            } else {
                s = new shaderParam(loc, gl.uniform1f);
            }
            break;
        case gl.FLOAT_VEC2:
            s = new shaderParam(loc, gl.uniform2fv);
            break;
        case gl.FLOAT_VEC3:
            s = new shaderParam(loc, gl.uniform3fv);
            break;
        case gl.FLOAT_VEC4:
            s = new shaderParam(loc, gl.uniform4fv);
            break;
        // -----------------
        case gl.INT:
        case gl.BOOL:
            if (size > 1) {
                s = new shaderParam(loc, gl.uniform1iv);
            } else {
                s = new shaderParam(loc, gl.uniform1i);
            }
            break;
        case gl.INT_VEC2:
        case gl.BOOL_VEC2:
            s = new shaderParam(loc, gl.uniform2iv);
            break;
        case gl.INT_VEC3:
        case gl.BOOL_VEC3:
            s = new shaderParam(loc, gl.uniform3fv);
            break;
        case gl.INT_VEC4:
        case gl.BOOL_VEC4:
            s = new shaderParam(loc, gl.uniform4fv);
            break;
        // -----------------
        case gl.FLOAT_MAT2:
            s = new matrixParam(loc, gl.uniformMatrix2fv);
            break;
        case gl.FLOAT_MAT3:
            s = new matrixParam(loc, gl.uniformMatrix3fv);
            break;
        case gl.FLOAT_MAT4:
            s = new matrixParam(loc, gl.uniformMatrix4fv);
            break;
        // -----------------
        case gl.SAMPLER_2D:
        case gl.SAMPLER_CUBE:
            if (size > 1) {
                console.log('not supported TEXUTURE array');
            } else {
                s = new textureParam(loc, gl.uniform1i, textureCount++);
            }
            break;
        default:
            break;
    }
    return s;
}
function initUniform(program, uniforms, textures) {
    var numUniforms = gl.getProgramParameter(program, gl.ACTIVE_UNIFORMS);
    for (var ii = 0; ii < numUniforms; ++ii) {
        var info = gl.getActiveUniform(program, ii);
        if (!info) {
            break;
        }

        var name = info.name;
        var setter = createUniformSetter(program, info);
        setUniform(uniforms, name, setter);
        if (info.type == gl.SAMPLER_2D || info.type == gl.SAMPLER_CUBE) {
            textures[name] = setter;
        }
    }
}
function initAttribute(program, attribs) {
    var numAttribs = gl.getProgramParameter(program, gl.ACTIVE_ATTRIBUTES);
    for (var ii = 0; ii < numAttribs; ++ii) {
        var info = gl.getActiveAttrib(program, ii);
        if (!info) {
            break;
        }
        if (info.size != 1) {
            throw("arrays of attribs not handled");
        }

        var index = gl.getAttribLocation(program, info.name);
        attribs[info.name] = new attributeParam(index);
    }
    return attribs;
}

/**
 * check if there is any link error
 * @param program
 */
function checkProgram(program) {
    var linked = gl.getProgramParameter(program, gl.LINK_STATUS);
    if (!linked) {
        var log = gl.getProgramInfoLog(program);
        throw('Error in program linking:' + log);
    }
}
/**
 *
 * @param id
 * @param vShader shader object
 * @param fShader shader object
 */
function program(id, vShader, fShader) {
    var program = gl.createProgram();

    gl.attachShader(program, vShader.getGLId());
    gl.attachShader(program, fShader.getGLId());
    gl.linkProgram(program);
    checkProgram(program);

    this._id = id;
    this._glid = program;
    this.__program__ = _autorelease.releaseGLProgram(program);
    this._release = false;

    this.createSetters();
}
program.prototype.getGLId = function () {
    return this._id;
}
program.prototype.release = function () {
    if (!this._release) {
        gl.deleteProgram(this._glid);
        this._release = true;
    }
}
program.prototype.createSetters = function () {
    initAttribute(this._glid, this.attrib = {});
    initUniform(this._glid, this.uniforms = {}, this.textures = {});
}
/**
 * uniform
 * @param name
 * @returns {*}
 */
program.prototype.getAttrib = function (name) {
    return this.attrib[name];
}
program.prototype.getAttribArray = function (key) {
    if (this.mVarSet[key]) {
        return this.mVarSet[key];
    }

    var res = [];
    var keys = [];
    var a;
    for (var i = 1, l = arguments.length; i < l; i++) {
        (a = this.attrib[arguments[i]]) && res.push(a);
        keys.push(arguments[i]);
    }
    if (res.length != arguments.length - 1) {
        console.log('program.getAttribs some arguments names not found.');
    }
    return this.mVarSet[key] = new attrSet(res, keys);
}
program.prototype.setAttrib = function (name, value) {
    var setter = this.attrib[name];
    if (setter) {
        setter.upload(value);
    } else if (SHOW_UNDEFINED) {
        console.log('attrib not found:' + name);
    }
}
/**
 * add a custom attribute
 * @param name
 */
program.prototype.addMeshAttrib = function (id) {
    var locs = [];
    var names = Array.prototype.slice.call(arguments, 1);
    var a;
    for (var i = 0, l = names.length; i < l; i++) {
        (a = this.attrib[names[i]]) && locs.push(a.loc());
    }
    if (locs.length != names.length) {
        console.log('program.getAttribs some arguments names not found.');
    }
    this.attrib[id] = new attrSet(names, locs);
}
/**
 * attirbute
 * @param name
 * @returns {*}
 */
program.prototype.getUniform = function (name) {
    return getUniform(this.uniforms, name);
}
program.prototype.setUniform = function (name, value) {
    var setter = getUniform(this.uniforms, name);
    if (setter) {
        setter.upload(value);
    } else if (SHOW_UNDEFINED) {
        console.log('uniform not found:' + name);
    }
}
program.prototype.use = function () {
    gl.useProgram(this._glid);
};

function getFileName(p) {
    var start = p.lastIndexOf('/') + 1;
    return p.slice(start, p.indexOf('.', start));
}
function create(id, vShader, fShader) {
    return new program(id, vShader, fShader);
}

exports.create = create;
