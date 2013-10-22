var PI = Math.PI;

/**
 * Converts degrees to radians.
 * @param {number} degrees A value in degrees.
 * @return {number} the value in radians.
 */
function radians(degrees) {
    return degrees * PI / 180;
}
function degrees(radians) {
    return 180 * radians / PI;
}
function isPowOf2(value) {
    return (value & (value - 1)) == 0;
};
function powOf2(num) {
    if(num <= 0) {
        return 1;
    }
    num--;
    var offset = 1;
    while ((num & (num + 1)) != 0) {
        num |= num >> offset;
        offset = offset << 1;
    }
    return num + 1;
}

exports.radians = radians;
exports.degrees = degrees;
exports.isPowOf2 = isPowOf2;
exports.powOf2 = powOf2;