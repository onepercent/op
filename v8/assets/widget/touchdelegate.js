function TouchDelegate(button) {
    this.button = button;
    this.active = false;
}
TouchDelegate.prototype.isInArea = function (vector) {
    vector.add(this.button.getCenter());
    if (vector[0] < 0 || vector[1] < 0) {
        return false;
    }
    vector.sub(this.button.getSize());
    if (vector[0] > 0 || vector[1] > 0) {
        return false;
    }
    return true;
}
TouchDelegate.prototype.onTouch = function (event, stack) {
    if (!this.isInArea(event.vector)) {
        this.active = false;
        this.button.setActive(this.active);
        return false;
    }
    switch (event.state) {
        case 0:// down
            this.active = true;
            break;
        case 1:// up
            if (this.active) {
                this.button.onclick(this);
            }
            this.active = false;
            break;
    }
    this.button.setActive(this.active);
    return true;
}

module.exports = TouchDelegate;
