var _selector = require('component/selector/selector.js');
var _TypeSelector = _selector.TypeSelector;
var _PseudoSelector = _selector.PseudoSelector;
var _AttributeSelector = _selector.AttributeSelector;
var _IdSelector = _selector.IdSelector;
var _ClassSelector = _selector.ClassSelector;
var _ChildSelector = _selector.ChildSelector;
var _Adjacentselectors = _selector.Adjacentselectors;

function Content(str) {
    this.string = str;
    this.index = 0;
    this.length = str.length;
    this.token = 0;
}
Content.prototype.findToken = function (token) {
    var tindex = -1;
    var c;
    for (var i = this.index, str = this.string; i < this.length; i++) {
        if ((tindex = token.indexOf(c = str.charAt(i))) != -1) {
            this.token = i;
            return c;
        }
    }
    return -1;
}
Content.prototype.nextToken = function (offset) {
    this.token = this.index = this.token + (offset || 0);
}
Content.prototype.hasChar = function () {
    return this.index < this.length;
}
Content.prototype.charLeft = function () {
    return this.string.slice(this.index);
}
Content.prototype.charBeforeToken = function () {
    return this.string.slice(this.index, this.token);
}
Content.prototype.getIndex = function () {
    return this.index;
}
Content.prototype.skip = function (count) {
    this.index += (count || 1);
}
Content.prototype.currentChar = function () {
    return this.string.charAt(this.index);
}
Content.prototype.close = function () {
    this.index = this.length;
}

// ==========================
// TypeParser
// ==========================
function TypeParser(parser) {
    this.parser = parser;
}
TypeParser.prototype.parse = function (content) {
    // skip leading empth chars
    while (content.currentChar() == ' ') {
        content.skip();
    }

    var token = content.findToken(':[.#>+ ');
    if (token == -1) {
        var type = content.charLeft();
        this.parser.addSelector(new _TypeSelector(type));
        content.close();
    } else {
        var typesel;
        var type = content.charBeforeToken();
        if (type.length == 0) {
            type = '*';
        }

        // ==================
        // wrap of previous
        // ==================
        if ('>+'.indexOf(token) != -1) {
            // if not started with >+
            if (token != content.currentChar()) {
                typesel = new _TypeSelector(type);
                content.nextToken();
                this.parser.addSelector(typesel);
                this.parser.nextTypeParser();
                return;
            }

            typesel = this.parser.lastSelector();
            if (!typesel) {
                throw 'previous of token:' + token + ' not found.'
            }
            if (token == '>') {
                typesel = new _ChildSelector(typesel);
            } else {
                typesel = new _Adjacentselectors(typesel);
            }
            content.skip(1);
            this.parser.addSelector(typesel);
            this.parser.nextTypeParser();
            return;
        }

        // ==================
        // Decorate current
        // ==================
        if (type.length == 0) {
            typesel = new _TypeSelector('*');
        } else {
            typesel = new _TypeSelector(type);
        }

        var sel = typesel;
        content.nextToken();

        switch (token) {
            case '[':
                sel = new _AttributeSelector(typesel);
                this.parser.setParser(new AttributeParser(this.parser, sel))
                break;
            case ':':// pseudo
                sel = new _PseudoSelector(typesel);
                this.parser.setParser(new TokenParser(this.parser, sel, 'setPseudo'));
                break;
            case '.':// class
                sel = new _ClassSelector(typesel);
                this.parser.setParser(new TokenParser(this.parser, sel, 'setClass'));
                break;
            case '#':// id
                sel = new _IdSelector(typesel);
                this.parser.setParser(new TokenParser(this.parser, sel, 'setId'));
                break;

            case ' ':
                this.parser.nextTypeParser();
                break;
        }

        this.parser.addSelector(sel);
    }
}

// ==========================
// TokenParser .*#
// ==========================
function TokenParser(parser, selector, fn) {
    this.parser = parser;
    this.selector = selector;
    this.fn = fn;
}
TokenParser.prototype.parse = function (content) {
    content.skip(1);// skip

    var token = content.findToken(' >+');
    if (token == -1) {
        this.selector[this.fn](content.charLeft());
        content.close();
        return;
    }

    this.selector[this.fn](content.charBeforeToken());
    content.nextToken();
    this.parser.nextTypeParser();
}

// ==========================
// AttributeSelector []
// ==========================
function AttributeParser(parser, selector) {
    this.parser = parser;
    this.selector = selector;
}
AttributeParser.prototype.parse = function (content) {
    var checkstart = true;
    while (true) {
        if (checkstart) {
            var token = content.findToken(' >+[');
            if (token == '[') {
                content.nextToken(1);
                checkstart = false;
            } else {
                content.nextToken();
                this.parser.nextTypeParser();
                break;
            }
        } else {
            var token = content.findToken(']');
            if (token == -1) {
                throw 'attribute end not found:' + content.getIndex();
            }
            this.selector.addAttribute(content.charBeforeToken());
            content.nextToken(1);
            checkstart = true;
        }
    }
}
// ==========================
function Parser() {
    this.selectors = [];
}
Parser.prototype.nextTypeParser = function () {
    this.parser = new TypeParser(this);
}
Parser.prototype.setParser = function (parser) {
    this.parser = parser;
}
Parser.prototype.addSelector = function (m) {
    this.selectors.push(m);
}
Parser.prototype.lastSelector = function () {
    if (this.selectors.length > 0) {
        return this.selectors.pop();
    }
    return null;
}
Parser.prototype.parse = function (str) {
    if (!str || str.length == 0) {
        return;
    }

    var index = -1;
    var cycle = 0;
    var content = new Content(str);
    this.nextTypeParser();

    while (content.hasChar()) {
        this.parser.parse(content);
        if (content.getIndex() == index) {
            cycle++;
        } else {
            cycle = 0;
            index = content.getIndex();
        }
        if (cycle == 2) {
            throw 'can not recoginze selectors form string:' + str;
        }
    }

    return this.selectors;
}

module.exports = Parser;
