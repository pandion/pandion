/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
client.namespace("client.os");

client.os.registry = function () {};

client.os.registry.prototype = {
	HKCR: "HKEY_CLASSES_ROOT",
	HKCU: "HKEY_CURRENT_USER",
	HKLM: "HKEY_LOCAL_MACHINE",

	read: function (hive, key, value) {
		try {
			return external.RegRead(hive, key, value);
		} catch (error) {
			return null || error;
		}
	},
	readCR: function (key, value) {
		return this.read(this.HKCR, key, value);
	},
	readCU: function (key, value) {
		return this.read(this.HKCU, key, value);
	},
	readLM: function (key, value) {
		return this.read(this.HKLM, key, value);
	},
	readCULM: function (key, value) {
		return this.read(this.HKCU, key, value) || this.read(this.HKLM, key, value);
	},
	readLMCU: function (key, value) {
		return this.read(this.HKLM, key, value) || this.read(this.HKCU, key, value);
	},

	remove: function (hive, key, value) {
		try {
			if (value === undefined)
				external.RegDeleteKey(hive, key);
			else
				external.RegDeleteValue(hive, key, value);
			return true;
		} catch (error) {
			return false;
		}
	},

	write: function (hive, key, value, data) {
		try {
			if ((data instanceof String) || (typeof data == 'string'))
				external.RegWriteString(hive, key, value, data);
			else if ((data instanceof Number) || (typeof data == 'number'))
				external.RegWriteDWORD(hive, key, value, data);
			else
				return false;
			return true;
		} catch (error) {
			return false;
		}
	},
	writeCR: function (key, value, data) {
		return this.write(this.HKCR, key, value, data);
	},
	writeCU: function (key, value, data) {
		return this.write(this.HKCU, key, value, data);
	},
	writeLM: function (key, value, data) {
		return this.write(this.HKLM, key, value, data);
	},
	writeCULM: function (key, value, data) {
		return this.write(this.HKCU, key, value, data) || this.write(this.HKLM, key, value, data);
	},
	writeLMCU: function (key, value, data) {
		return this.write(this.HKLM, key, value, data) || this.write(this.HKCU, key, value, data);
	}
};

client.os.registry = new client.os.registry();
