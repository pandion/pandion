client.namespace("client.ui.idle.sensor");

client.ui.idle.sensor.screenSaver = function (arg) {
	this.events = new client.events.manager(arg.events || {});
};

client.ui.idle.sensor.screenSaver.prototype.start = function () {
	window["handlerScreenSaverActivate"] = function (that) {return function () {
		that.handlerScreenSaverActivate();
	}}(this);
	external.wnd.setScreenSaverActivateHandler("handlerScreenSaverActivate");
};

client.ui.idle.sensor.screenSaver.prototype.cancel = function () {
	external.wnd.setScreenSaverActivateHandler("");
	external.wnd.setScreenSaverDeactivateHandler("");
};

client.ui.idle.sensor.screenSaver.prototype.keepalive = function () {
};

client.ui.idle.sensor.screenSaver.prototype.timePassed = function () {
	return 0;
};

client.ui.idle.sensor.screenSaver.prototype.handlerScreenSaverActivate = function () {
	external.wnd.setScreenSaverActivateHandler("");
	external.wnd.setScreenSaverDeactivateHandler("handlerScreenSaverDeactivate");
	window["handlerScreenSaverActivate"] = function (that) {return function () {
		that.handlerScreenSaverDeactivate();
	}}(this);
	this.events.publish("enteringIdle");
};

client.ui.idle.sensor.screenSaver.prototype.handlerScreenSaverDeactivate = function () {
	external.wnd.setScreenSaverDeactivateHandler("");
	external.wnd.setScreenSaveHandler("handlerScreenSaverActivate");
	window["handlerScreenSaverActivate"] = function (that) {return function () {
		that.handlerScreenSaverActivate();
	}}(this);
	this.events.publish("leavingIdle");
};
