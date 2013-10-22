(function (process) {
    var global = this;
    var print__ = this.print;
    var module_depth = 0;

    function startup() {
        startup.globalExtend();
        startup.globalRequire();
        startup.globalConsole();
        startup.globalTypedArray();
//        startup.globalGL();
    }

    startup.globalExtend = function() {
        function _extends(sub, super_, props) {
            sub.prototype = Object.create(super_.prototype);
            if (props) {
                for (var i in props) {
                    sub.prototype[i] = props[i];
                }
            }
            sub.prototype.constructor = sub;
            return sub;
        }
        global.__defineGetter__('_extends', function () {
            return _extends;
        });
    }
    startup.globalRequire = function () {
        global.__defineGetter__('require', function () {
            return NativeModule.require;
        });
    }
    startup.globalConsole = function () {
        global.__defineGetter__('console', function () {
            return NativeModule.require('console');
        });
    };
    /**
     * may triggered with a -typed_array condition
     */
    startup.globalTypedArray = function() {
        var clz = NativeModule.require('nativeclasses');
        global.ArrayBuffer = clz.ArrayBuffer;

        global.Int8Array = clz.Int8Array;
        global.Uint8Array = clz.Uint8Array;
        global.Int16Array = clz.Int16Array;
        global.Uint16Array = clz.Uint16Array;
        global.Int32Array = clz.Int32Array;
        global.Uint32Array = clz.Uint32Array;
        global.Float32Array = clz.Float32Array;
        global.Float64Array = clz.Float64Array;
    };
//    startup.globalGL = function() {
//        var gl = require('opengl');
//        gl.checkGLError = function(msg) {
//            var err = gl.getError();
//            if(err != 0) {
//                console.log('error found:' + err, msg);
//            };
//        }
//    }

    /**
     * 表示预先定义的 js 模块
     *
     * @param id
     * @constructor
     */
    function NativeModule(id) {
        this.filename = id;
        this.id = id;
        this.exports = {};
        this.loaded = false;
    }

    NativeModule._cache = {};

    /**
     * 请求一个模块
     *
     * @param id
     * @returns {*}
     */
    NativeModule.require = function (id) {
        if (id == 'native_module') {
            return NativeModule;
        }

        var cached = NativeModule.getCached(id);
        if (cached) {
            return cached.exports;
        }

        var nativeModule = new NativeModule(id);
        nativeModule.cache();
        nativeModule.compile();

        return nativeModule.exports;
    };

    NativeModule.getCached = function (id) {
        return NativeModule._cache[id];
    }

    /**
     * 把代码包装成一个函数, 这样能够代理到对象的执行
     * @param script
     * @returns {*}
     */
    NativeModule.wrap = function (script) {
        return NativeModule.wrapper[0] + script + NativeModule.wrapper[1];
    };
 NativeModule.release = function (script) {
 console.log(' NativeModule.release');
  NativeModule._cache = {};
 };


    NativeModule.prototype.compile = function () {
//    	print__((module_depth++) + ' require-->' + this.id);
    	try {
	        var fn = process.binding(this.id);
	        fn.call(this, this.exports, NativeModule.require, this, this.filename);
	        this.loaded = true;
    	} catch(e) {
    		print__('exception:' + e);
    	}
//        print__((--module_depth) + ' require<--' + this.id);
    };

    NativeModule.prototype.cache = function () {
        NativeModule._cache[this.id] = this;
    };

    startup();
});
