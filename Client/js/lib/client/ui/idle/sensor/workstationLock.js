/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
client.namespace("client.ui.idle.sensor");

client.ui.idle.sensor.workstationLock = function (arg) {
	this.events = new client.events.manager(arg.events || {});
};

client.ui.idle.sensor.workstationLock.prototype.start = function () {
	window["handlerWorkstationLock"] = function (that) {return function () {
		that.handlerWorkstationLock();
	}}(this);
	external.wnd.setWorkstationLockHandler("handlerWorkstationLock");
};

client.ui.idle.sensor.workstationLock.prototype.cancel = function () {
	external.wnd.setWorkstationLockHandler("");
	external.wnd.setWorkstationUnlockHandler("");
};

client.ui.idle.sensor.workstationLock.prototype.keepalive = function () {
};

client.ui.idle.sensor.workstationLock.prototype.timePassed = function () {
	return 0;
};

client.ui.idle.sensor.workstationLock.prototype.handlerWorkstationLock = function () {
	external.wnd.setWorkstationLockHandler("");
	external.wnd.setWorkstationUnlockHandler("handlerWorkstationUnlock");
	window["handlerWorkstationUnlock"] = function (that) {return function () {
		that.handlerWorkstationUnlock();
	}}(this);
	this.events.publish("enteringIdle");
};

client.ui.idle.sensor.workstationLock.prototype.handlerWorkstationUnlock = function () {
	external.wnd.setWorkstationUnlockHandler("");
	external.wnd.setWorkstationLockHandler("handlerWorkstationLock");
	window["handlerWorkstationLock"] = function (that) {return function () {
		that.handlerWorkstationLock();
	}}(this);
	this.events.publish("leavingIdle");
};
