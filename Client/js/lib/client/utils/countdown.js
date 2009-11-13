client.namespace("client.utils");

client.utils.countdown = function (arg) {
	arg = arg || {};
	this.duration = arg.duration || 0;
	this.period = arg.period || Math.round(this.timeout / 10);
	this._timeout = null;
	this._interval = null;
	this._timestamp = null;
	this.events = new client.events.manager(arg.events || {});
};

client.utils.countdown.prototype._start = function () {
	var that = this;
	this._timestamp = (new Date()).getTime();
	this._timeout = setTimeout(function () {
		that.trigger();
	}, this.duration);
	this._interval = setInterval(function () {
		that.events.publish("onStep");
	}, this.period);
};

client.utils.countdown.prototype._cancel = function () {
	clearTimeout(this._timeout);
	clearInterval(this._interval);
	this._timeout = null;
	this._interval = null;
	this._timestamp = null;
};

client.utils.countdown.prototype._isRunning = function () {
	return this._timeout != null;
};

client.utils.countdown.prototype.start = function () {
	if (!this._isRunning()) {
		this._start();
		this.events.publish("onStart");
	}
};

client.utils.countdown.prototype.cancel = function () {
	if (this._isRunning()) {
		this._cancel();
		this.events.publish("onCancel");
	}
};

client.utils.countdown.prototype.keepalive = function () {
	if (this._isRunning()) {
		this._cancel();
		this._start();
	}
};

client.utils.countdown.prototype.trigger = function (data) {
	if (this._isRunning()) {
		this._cancel();
		this.events.publish("onTrigger", data);
	}
};

client.utils.countdown.prototype.timePassed = function () {
	return this._isRunning() ? ((new Date()).getTime() - this._timestamp) : 0;
};
