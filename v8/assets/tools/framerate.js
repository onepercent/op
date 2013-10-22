var watchTime = 0;
var count = 0;
//var console = require('console');

function update() {
    var t = new Date().getTime();
    if(t - watchTime > 1000) {
        console.log('framerate:' + count);

        count = 0;
        var remain = (t - watchTime) % 1000;
        watchTime = t - remain;
    } else {
        count++;
    }
}
function reset() {
    watchTime = new Date().getTime();
}

exports.update = update;
exports.reset = reset;