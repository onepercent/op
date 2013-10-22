var _CSSNode = require('component/selector/cssnode.js');
var _NodeIterator = require('component/selector/nodeiterator.js');
var _NodeListener = require('component/selector/nodelistener.js');
var _matcher = require('component/selector/selector.js');
var _parser = require('component/selector/parser.js');
var _selector = require('component/selector.js');

function parseSelector(str) {
    var p = new _parser();
    p.parse(str);
}

//parseSelector('div   >   image');
//parseSelector('div>image');
//parseSelector('a>image>ui');
//parseSelector('a >image>ui');
//parseSelector('a > image>ui');
//parseSelector('a  >  image>ui');
//parseSelector('a   >   image>ui');
//parseSelector('a   >   image > ui');
//parseSelector('a   >   image  >  ui');
//parseSelector('a#abc');
//parseSelector('#abc');
//parseSelector('#abc>div');
//parseSelector('div>#abc');
//parseSelector('div > #abc>div');
//parseSelector('div > table#abc > div');
//parseSelector('div > table#abc>div.red');
//parseSelector('a>div.red');
//parseSelector('div.red>div.blue');
//parseSelector('div.red>div#blue');
//parseSelector('div.red>#blue');
//parseSelector('div.red>#blue');
//parseSelector('div[a=b]');
//parseSelector('div.red>div[a=b]');
//parseSelector('div[a=b] div');
//parseSelector('div[a=b] +div');
//parseSelector('div[a=b]+div');
//parseSelector('div#user_name image:focus');
//parseSelector('div#user_name>image:focus');
//parseSelector('div#user_name  >  image:focus');

function spiltProp(p) {
    var prop = {};
    if (!p || p.length == 0) {
        return prop;
    }
    for (var i = -1, coll = p.split(','), l = coll.length; ++i < l;) {
        var namevalue = coll[i].split('=');
        prop[namevalue[0]] = namevalue[1];
    }
    return prop;
}
function TestNode(type, prop, children) {
    this.type = type;
    this.mTag = type;
    this.properties = spiltProp(prop);
    this.children = children;
}
TestNode.prototype.getPropStr = function () {
    var str = [];
    for (var i in this.properties) {
        str.push(i + ':' + this.properties[i]);
    }
    return '{' + str.join(',') + '}';
}
TestNode.prototype.toString = function () {
    return this.type + this.getPropStr();
}

var tree = new TestNode('root', '', [
    new TestNode('div', 'id=div1', [
        new TestNode('image', 'id=d_image1', [
            new TestNode('div', 'id=div1', [
                new TestNode('u', 'id=image22', [
                    new TestNode('image', 'id=image22', [
                        new TestNode('div', 'id=image22', [
                            new TestNode('a', 'id=a', [
                            ]),
                        ]),
                    ]),
                ]),
            ]),
        ]),
        new TestNode('image', 'id=image1', [
        ]),
        new TestNode('image', 'id=image2', [
        ]),
        new TestNode('image', 'id=image3', [
        ])
    ]),
//    new TestNode('image', 'id=middiv1'),
//    new TestNode('image', 'id=middiv2'),
//    new TestNode('div', 'id=div2', [
//        new TestNode('div', 'id=divinner', [
//            new TestNode('image', 'id=image4', [
//                new TestNode('image', 'id=image5', [
//                ]),
//                new TestNode('image', 'id=image8', [
//                ])
//            ]),
//            new TestNode('div', 'id=div2', [
//                new TestNode('image', 'id=image6', [
//                ])
//            ])
//        ])
//    ]),
//    new TestNode('image', 'id=middiv'),
//    new TestNode('div', 'id=div3')
]);
var root = _CSSNode.wrap(tree);
root.print();

function querySelector(tree, pattern) {
    var res = _selector.querySelector(tree, pattern);
    console.log('-------');
    console.log(Array.prototype.slice.call(res, 0).join('\n'));
}
//querySelector(tree, 'div>div>image');
//querySelector(tree, 'div>div>image>image');
//querySelector(tree, 'div>div image');
//querySelector(tree, 'div>image');
//querySelector(tree, 'div image');
//querySelector(tree, 'image');
//querySelector(tree, '*');
//querySelector(tree, 'image + image');
//querySelector(tree, 'div>image>div *');
//querySelector(tree, 'div>image>div image *');
//querySelector(tree, 'div>image>div a');
//querySelector(tree, '* a');
//querySelector(tree, 'a');
