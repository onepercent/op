(function () {
    print(11);
    this.gc1 = new gcobserver('gc1');
    this.gc2 = new gcobserver('gc2');
    this.testFn = function () {
        print('in testFn');
        print(this.gc1);
        print(this.gc2);
    }
    this.removeRef = function () {
        delete this.gc1;
        delete this.gc2;
    }
    this.forceGc = function () {
        var count = 0;
        while (count++ < 100000) {
            new gcobserver();
        }
    }
});

var replact = function (str, param) {
    function findValue(param, propname) {
        var v = param[propname];
        if (v) {
            return v;
        }
        var names = propname.split('.');
        if (names.length == 0) {
            return null;
        }
        var cursor = param;
        for (var i = -1, l = names.length; ++i < l;) {
            cursor = cursor[names[i]];
            if (!cursor) {
                break;
            }
        }
        return cursor;
    }

    var start = 0,
        length = str.length;
    var result = '';
    while (start < length) {
        var p1 = str.indexOf('${', start);
        if (p1 == -1) {
            result += str.slice(start);
            start = str.length;
        } else {
            var p2 = str.indexOf('}', p1);
            if (p2 != -1) {
                //console.log("p1:",p1,"p2:",p2)
                var name = str.slice(p1, p2).slice(2);
                result += str.slice(start, p1);
                result += findValue(param, name) || '';
                start = p2 + 1;
            } else {
                result += str.slice(start);
                start = length;
            }
        }
    }
    return result;
}

console.log(replact('nihao ${name}, good morning', {name: 'jack'}));
console.log(replact('nihao ${name} are you at ${city.name} ${city.road}, good morning. ${what.do.you.think}', {name: 'jack', city: {name: 'HangZhou', road: 'WenEr.Rd'}, 'what.do.you.think': 'think nothing :)'}));
