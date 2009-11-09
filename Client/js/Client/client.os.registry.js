client.namespace("client.os.registry");

client.os.registry.read = function (hive, key, value) {
	try {
		return external.RegRead(hive, key, value);
	} catch (error) {
		return null;
	}
};

client.os.registry.write = function (hive, key, value, data) {
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
};

client.os.registry.remove = function (hive, key, value) {
	try {
		if (value == undefined)
			external.RegDeleteKey(hive, key);
		else
			external.RegDeleteValue(hive, key, value);
		return true;
	} catch (error) {
		return false;
	}
};
