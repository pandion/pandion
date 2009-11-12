client.namespace("client.utils");

client.utils.countdown = function (arg) {
	arg = arg || {};
	this.timeout = undefined;
	this.duration = arg.duration || 30000;
	this.events = new client.events.manager(arg.events || {});
};

client.utils.countdown.prototype.start = function () {
	if (this.timeout === undefined) {
		this.timeout = setTimeout(this.trigger, this.duration);
		this.events.publish("onStart");
	}
};

client.utils.countdown.prototype.stop = function () {
	if (this.timeout !== undefined) {
		clearTimeout(this.timeout);
		this.timeout = undefined;
		this.events.publish("onStop");
	}
};

client.utils.countdown.prototype.keepalive = function () {
	if (this.timeout !== undefined) {
		clearTimeout(this.timeout);
		this.timeout = setTimeout(this.trigger, this.duration);
	}
};

client.utils.countdown.prototype.trigger = function () {
	this.events.publish("onTrigger");
};
