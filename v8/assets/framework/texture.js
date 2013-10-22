function iterator(coll, root, textures) {
    for (var i in coll) {
        if (typeof coll[i] === 'object') {
            iterator(coll[i], root + '/' + i, textures);
        } else {
            textures.push(root + '/' + i + coll[i]);
            coll[i] = textures.length - 1;
        }
    }
}
var R = {
    game: {
        bg: '.png',
        block_big: '.png',
        block_small: '.png',
        block_column: '.png'
    },
    ui: {
        add: '.png',
        auto: '.png',
        column: '.png',
        counter: '.png',
        minus: '.png',
        ok: '.png',
        points_bg: '.png',
        slide: '.png'
    },
    upgrade: {
        archive_line_01: '.png',
        archive_line_02: '.png',
        arrow_level: '.png',
        b_01: '.png',
        b_02: '.png',
        b_addcoins: '.png',
        b_close: '.png',
        b_close_01: '.png',
        b_close_02: '.png',
        b_select: '.png',
        b_shop_01: '.png',
        b_shop_02: '.png',
        b_shop_selected: '.png',
        b_unlock_01: '.png',
        b_unlock_02: '.png',
        b_z_achievement: '.png',
        b_z_avatar: '.png',
        b_z_shop: '.png',
        b_z_skill: '.png',
        b_z_upgrade: '.png',
        bg_coins_star: '.png',
        bg_level_01: '.png',
        bg_level_02: '.png',
        bg_start_gains_1: '.png',
        bg_start_gains_2: '.png',
        bg_unlock: '.png',
        bg: '.png',
        bg_upgrade: '.png',
        bg_upgrade_coins: '.png',
        coin_bg: '.png',
        icon_archive_01: '.png',
        icon_archive_02: '.png',
        icon_boy_clothes: '.png',
        icon_boy_gloves: '.png',
        icon_boy_hat: '.png',
        icon_boy_shoes: '.png',
        icon_girl_clothes: '.png',
        icon_girl_gloves: '.png',
        icon_girl_hat: '.png',
        icon_girl_shoes: '.png',
        icon_tools_01: '.png',
        icon_tools_02: '.png',
        icon_tools_selected: '.png',
        item_01: '.png',
        item_02: '.png',
        item_03: '.png',
        item_04: '.png',
        item_05: '.png',
        item_06: '.png',
        j_01: '.png',
        j_02: '.png',
        j_03: '.png',
        j_04: '.png',
        kid_boy: '.png',
        kid_boy_01: '.png',
        kid_boy_02: '.png',
        kid_girl: '.png',
        kid_girl_01: '.png',
        kid_girl_02: '.png',
        p_coin: '.png',
        p_star: '.png',
        paper_01: '.png',
        paper_02: '.png',
        query: '.png',
        query_cp: '.png',
        scroll_01: '.png',
        scroll_02: '.png',
        split_h: '.png',
        split_v: '.png',
        z_iceball: '.png',
        z_magnet: '.png',
        z_turbo: '.png',
        z_unlock: '.png',
        others: {
            b_unlock_01: '.png',
            b_unlock_02: '.png',
            bg_unlock: '.png',
            j_01: '.png',
            j_02: '.png',
            j_03: '.png',
            j_04: '.png',
            kid_boy: '.png',
            kid_boy_01: '.png',
            kid_boy_02: '.png',
            kid_girl: '.png',
            kid_girl_01: '.png',
            kid_girl_02: '.png',
            z_unlock: '.png'
        }
    },

    pngnow: '.png',
    test: '.png',
    word: '.png'
}

var textures = [];
var root = 'images';
iterator(R, '', textures);

// managing texture frame creation
var _textures = require('glcore/textures.js');
var _frame = require('glcore/textureframe.js');

function getPath(id) {
    return root + textures[id];
}
function createFrame(id) {
    var path = getPath(id);
    if (!path) {
        console.log('createFrame image not found:' + id);
    }
    if (path.frame) {
        return path.frame;
    }

    var t = _textures.createTexture2D(path);
    return path.frame = new _frame(t);
}

//var gcobserver = require('nativeclasses').gcobserver;
//var gc1 = new gcobserver('~~~~~~~texture.js');
//console.log('gcobserver:', gcobserver, gc1);
_textures.createTexture2D('images/upgrade/b_addcoins.png');
_textures.createTexture2D('images/upgrade/b_addcoins.png');
_textures.createTexture2D('images/upgrade/b_addcoins.png');

exports.R = R;
exports.createFrame = createFrame;