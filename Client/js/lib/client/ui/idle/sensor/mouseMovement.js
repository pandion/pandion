client.namespace("client.ui.idle.sensor");

client.ui.idle.sensor.mouseMovement = function (arg) {
	this.events = new client.events.manager(arg.events || {});

	var that = this, idleMouseX, idleMouseY;
	this.mouseTracker = new client.utils.countdown({
		duration: 5*60*1000,
		period: 10000,
		events: {
			onStep: function () {
				if (idleMouseX != external.cursorX || idleMouseY != external.cursorY) {
					idleMouseX = external.cursorX;
					idleMouseY = external.cursorY;
					that.mouseTracker.keepalive();
				}
			},
			onTrigger: function () {
				that.events.publish("enteringIdle");
				idleMouseX = external.cursorX;
				idleMouseY = external.cursorY;
				var idleInterval = setInterval(function () {
					if (idleMouseX != external.cursorX || idleMouseY != external.cursorY) {
						idleMouseX = external.cursorX;
						idleMouseY = external.cursorY;
						clearInterval(idleInterval);
						that.events.publish("leavingIdle");
						that.mouseTracker.start();
					}
				}, 2000);
			}
		}
	});
};

client.ui.idle.sensor.mouseMovement.prototype.start = function () {
	this.mouseTracker.start();
};

client.ui.idle.sensor.mouseMovement.prototype.cancel = function () {
	this.mouseTracker.cancel();
};

client.ui.idle.sensor.mouseMovement.prototype.keepalive = function () {
	this.mouseTracker.keepalive();
};

client.ui.idle.sensor.mouseMovement.prototype.timePassed = function () {
	return this.mouseTracker.timePassed();
};
