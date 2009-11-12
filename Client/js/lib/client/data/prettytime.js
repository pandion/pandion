client.namespace("client.data");

client.data.prettytime = function (date) {
	if (date instanceof Date)
		date = date.getTime();
	var timeAgo = (new Date()).getTime() - date;
	timeAgo /= 1000;
	var map = [
		[0, "now"],
		[1, "second"],
		[2, "seconds", 2],
		[60, "minute"],
		[2*60, "minutes", 2],
		[60*60, "hour"],
		[2*60*60, "hours", 2],
		[24*60*60, "day"],
		[2*24*60*60, "days", 2],
		[7*24*60*60, "week"],
		[2*7*24*60*60, "weeks", 2],
		[30*24*60*60, "month"],
		[2*30*24*60*60, "months", 2],
		[365*24*60*60, "year"],
		[2*365*24*60*60, "years", 2]
	];
	for (var i = 1; i < map.length && timeAgo >= map[i][0]; ++i);
	i--;
	if (map[i].length == 3)
		timeAgo *= map[i][2];
	return external.globals("Translator").Translate(
		"main",
		"time-" + map[i][1],
		[parseInt(timeAgo / map[i][0])]
	);
};
