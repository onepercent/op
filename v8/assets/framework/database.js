var mStore = {};
mStore.levelConf = [
    {name: 'Magnet', icon: 'item_06', price: 4000, level: 6},
    {name: 'Fireball', icon: 'item_05', price: 5000, level: 6},
    {name: 'Blazo', icon: 'item_04', price: 6000, level: 6}
]
mStore.toolsConf = [
    {name: 'Turbo', icon: ''},
    {name: 'Jetpack', icon: ''},
    {name: 'Ferrzo', icon: ''}
]


var global = this;
function DB() {
}
DB.prototype.getData = function (name) {
    return mStore[name];
}

module.exports = new DB();