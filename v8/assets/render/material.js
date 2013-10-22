/**
 * used as a program's attrbute setter
 * @constructor
 */
function Material(program) {
    this.program = program;
}
Material.prototype.use = function () {
    this.program.use();
}
Material.prototype.bindMesh = function (mesh) {
}
Material.prototype.bindMatrix = function (matrix) {
}

module.exports = Material;