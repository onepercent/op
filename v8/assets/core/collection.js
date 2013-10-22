var _inherit = require('core/inherit.js');

/**
 * used as cpp template
 *
 * @constructor
 */
function Collection() {
    this.DB = {};
}

/**
 * find or create
 * @param id
 * @returns {*}
 */
Collection.prototype.findopt = function (id) {
    var obj = this.find(id);
    if (!obj) {
        this.add(id, obj = this.create.apply(this, arguments));
    }
    return obj;
}
Collection.prototype.find = function (id) {
    return this.DB[id];
}
Collection.prototype.erase = function (id) {
    delete this.DB[id];
}

Collection.prototype.add = function (id, obj) {
    this.DB[id] = obj;
}
Collection.prototype.remove = function (id) {
    var obj = this.find(id);
    if (obj) {
        this.erase(id);
        this.onRemove(obj);
    }
}

/**
 * subclass must implement this
 * @param id
 */
Collection.prototype.create = function (id) {
}
Collection.prototype.onRemove = function (obj) {
}

function createCollection(prop) {
    var db = new Collection();
    for (var i in prop) {
        db[i] = prop[i];
    }
    return db;
}
/**
 * return as template
 *
 * @param prop
 * @returns {*}
 */
function createTemplate(prop) {
    return _inherit(function () {
        Collection.call(this);
    }, Collection, prop);
}

exports.createCollection = createCollection;
exports.createTemplate = createTemplate;