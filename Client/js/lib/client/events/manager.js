/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
client.namespace("client.events");

client.events.manager = function (callbacks) {
	this.callbacks = {};
	for (var name in callbacks)
		this.subscribe(name, callbacks[name]);
};

client.events.manager.prototype.publish = function (name, data) {
	if (name in this.callbacks)
		for (var i = 0; i < this.callbacks[name].length; i++)
			this.callbacks[name][i](data);
};

client.events.manager.prototype.subscribe = function (name, callback) {
	if (!(name in this.callbacks))
		this.callbacks[name] = [];
	this.callbacks[name].push(callback);
};

client.events.manager.prototype.unsubscribe = function (name, callback) {
	if (name in this.callbacks)
		for (var i = 0; i < callbacks[name].length; i++)
			if (callbacks[name][i] === callback)
				callbacks[name].splice(i--, 1);
};
