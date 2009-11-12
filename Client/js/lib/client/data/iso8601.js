client.namespace("client.data");

client.data.iso8601 = function (timestamp) {
	                           // 1 BCAD 2 Y          3 M         4 D            5 h         6 m         7 s         8 ms                              9 o   10 oh       11 om
	var match = timestamp.match(/^([-+]?)(\d{4,})(?:-?(\d{2})(?:-?(\d{2})(?:[Tt ](\d{2})(?::?(\d{2})(?::?(\d{2})(?:\.(\d{1,3})(?:\d+)?)?)?)?(?:[Zz]|(?:([-+])(\d{2})(?::?(\d{2}))?)?)?)?)?)?$/);
	if (match) {
		for (var ints = [2, 3, 4, 5, 6, 7, 8, 10, 11], i = ints.length - 1; i >= 0; --i)
			match[ints[i]] = (typeof match[ints[i]] != "undefined" &&
				match[ints[i]].length > 0) ? parseInt(match[ints[i]], 10) : 0;
		if (match[1] == '-') // BC/AD
			match[2] *= -1;
		var ms = Date.UTC(
			match[2], // Y
			match[3] - 1, // M
			match[4], // D
			match[5], // h
			match[6], // m
			match[7], // s
			match[8] // ms
		);
		if (typeof match[9] != "undefined" && match[9].length > 0) // offset
			ms += (match[9] == '+' ? -1 : 1) * (match[10]*3600*1000 + match[11]*60*1000); // oh om
		if (match[2] >= 0 && match[2] <= 99) // 1-99 AD
			ms -= 59958144000000;
		return ms;
	}
	else
		return Number.NEGATIVE_INFINITY;
};
