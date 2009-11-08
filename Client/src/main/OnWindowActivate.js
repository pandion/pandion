function OnWindowActivate () {
	if (document.getElementById("content-dialog").style.display == "block") {
		external.globals("ClientRoster").Search.Blur();
	}
}
