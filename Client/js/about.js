﻿/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
window.attachEvent("onload", function () {
	external.globals("Translator").TranslateWindow("about", document);
	var selectionFilter = function () {return event.srcElement.tagName == "TEXTAREA" || event.srcElement.tagName == "INPUT"};
	document.attachEvent("onselectstart", selectionFilter);
	document.attachEvent("ondragstart", selectionFilter);
	document.attachEvent("oncontextmenu", selectionFilter);
	document.attachEvent("onkeydown", function () {if (event.keyCode == 27) external.wnd.close()});

	document.getElementById("software-name").innerText = external.globals("softwarename");
	document.getElementById("software-number").innerText = external.globals("softwareversion");
	document.getElementById("software-track").innerText = external.globals("Translator").Translate("main", "track-" + external.globals("softwaretrack"));
	client.css.addClass(document.getElementById("software-track"), external.globals("softwaretrack"));
	//document.getElementById("software-link").innerText = document.getElementById("software-link").href = external.globals("softwareurl");
	document.getElementById("contributor-name").innerText = external.globals("contributorname");
	document.getElementById("contributor-mail").innerText = external.globals("contributormail");
	document.getElementById("contributor-mail").href = 'mailto:' + external.globals("contributormail");

	document.getElementById("content").attachEvent("onsubmit", function () {
		event.returnValue = false;
		external.wnd.close();
	});

	var anchors = document.getElementsByTagName("a");
	for (var i = 0; i < anchors.length; i++)
		if (anchors[i].href.indexOf("http") != -1)
			client.html.anchorToBrowser(anchors[i]);

	document.getElementById("btn-ok").focus();
	external.wnd.hide(false);
});