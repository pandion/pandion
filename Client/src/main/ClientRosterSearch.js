function ClientRosterSearch ()
{
	var autoHide = false;

	this.Show = Show;
	this.Hide = Hide;
	this.Focus = Focus;
	this.Close = Close;
	this.Blur = Blur;
	this.Reset = Reset;
	this.Query = "";

	var ClientRosterSearch = this;
	var domSearchArea = document.getElementById("find-area");
	var domSearchField = document.getElementById("find-query");
	var domSearchClose = document.getElementById("find-close");

	if ( autoHide )
		ClientRosterSearch.Hide();
	else
		ClientRosterSearch.Show();

	function Show ()
	{
		if (domSearchArea.style.display != "block")
			domSearchArea.style.display = "block";
	}

	function Focus ()
	{
		if (document.getElementById("content-dialog").style.display == "block")
			domSearchField.focus();
	}

	function Hide ()
	{
		domSearchArea.style.display = "none";
	}

	function Close ()
	{
		this.Reset();
		if (autoHide)
			this.Hide();
		else
			this.Blur();
	}

	function Blur ()
	{
		domSearchField.blur();
		searchBlur();
	}

	function Reset ()
	{
		domSearchField.value = "";
	}

	document.attachEvent("onkeypress", searchFirstChar);
	function searchFirstChar ()
	{
		// Esc
		if (event.keyCode == 27)
			ClientRosterSearch.Close();
		else
		{
			ClientRosterSearch.Reset();
			ClientRosterSearch.Show();
			ClientRosterSearch.Focus();
		}
	}

	domSearchClose.attachEvent("onclick", closeClick);
	function closeClick ()
	{
		ClientRosterSearch.Close();
	}

	domSearchField.attachEvent("onselectstart", searchSelect);
	function searchSelect ()
	{
		event.cancelBubble = true;
		return true;
	}

	domSearchField.attachEvent("onkeypress", cancelBubble);
	function cancelBubble ()
	{
		event.cancelBubble = true;
		return true;
	}

	domSearchField.attachEvent("onkeydown", keyboardSearch);
	function keyboardSearch ()
	{
		event.cancelBubble = true;

		if (event.keyCode == 27) // Esc
		{
			ClientRosterSearch.Close();
			return false;
		}
		else if (event.keyCode == 13) // Enter / Return
		{
			activateFirstContact();
			return false;
		}
		else 
			return true;
	}

	document.attachEvent("onkeydown", keyboardGlobal);
	function keyboardGlobal ()
	{
		// F3 or Ctrl + F
		if ( event.keyCode == 114 || ( event.keyCode == 70 && event.ctrlKey ) )
		{
			ClientRosterSearch.Show();
			ClientRosterSearch.Focus();
			event.cancelBubble = true;
			return false;
		}
	}

	domSearchField.attachEvent("onfocus", searchFocus);
	function searchFocus ()
	{
		if (domSearchField.value == external.globals("Translator").Translate("main", "search_info"))
			domSearchField.value = "";
		domSearchField.className = "";
	}

	domSearchClose.attachEvent("onblur", searchBlur);
	domSearchField.attachEvent("onblur", searchBlur);
	function searchBlur ()
	{
		if (domSearchField.value == "")
		{
			domSearchField.className = "inactive";
			domSearchField.value = external.globals("Translator").Translate("main", "search_info");
		}
	}

	domSearchField.attachEvent("onpropertychange", displayClose);
	function displayClose ()
	{
		if (domSearchField.value == "" || domSearchField.value == external.globals("Translator").Translate("main", "search_info"))
			domSearchClose.className = "inactive";
		else
			domSearchClose.className = "";
	}

	domSearchField.attachEvent("onpropertychange", checkQuery);
	function checkQuery ()
	{
		var newQuery = domSearchField.value.toLowerCase();
		if (newQuery == external.globals("Translator").Translate("main", "search_info").toLowerCase())
			newQuery = "";
		if (ClientRosterSearch.Query != newQuery)
		{
			ClientRosterSearch.Query = newQuery;
			applyFilter();
		}
	}

	function applyFilter ()
	{
		if (ClientRosterSearch.Query.length == 0)
		{
			restoreGroups();
			restoreContacts();
		}
		else
		{
			hideGroups();
			scanContacts();
		}
	}

	function restoreGroups ()
	{
		var groups = (new VBArray(external.globals("ClientRoster").Groups.Keys())).toArray();
		for (var i in groups)
			displayGroup(
				external.globals("ClientRoster").Groups(groups[i]),
				true,
				external.globals("ClientRoster").Groups(groups[i]).ShowAll,
				external.globals("ClientRoster").Groups(groups[i]).ShowOffline,
				external.globals("ClientRoster").Groups(groups[i]).ShowAll
			);
	}

	function restoreContacts ()
	{
		var contacts = (new VBArray(external.globals("ClientRoster").Items.Keys())).toArray();
		for (var i in contacts)
			displayContact(external.globals("ClientRoster").Items(contacts[i]), true);
	}

	function displayGroup (Group, ShowHeader, ShowAll, ShowOffline, ShowToggle)
	{
		Group.HTMLHeader.style.display = ShowHeader ? "block" : "none";
		Group.HTMLOnline.style.display = ShowAll ? "block" : "none";
		Group.HTMLOffline.style.display = (ShowAll && ShowOffline) ? "block" : "none";
		Group.HTMLShowAll.style.display = ShowToggle ? "inline" : "none";
	}

	function displayContact (Contact, Visible)
	{
		if (!Contact.Resources.Count)
			for (var i in Contact.Groups)
				if (Visible)
				{
					if (!Contact.HTMLElements.Exists(Contact.Groups[i]))
						Contact.Draw(external.globals("ClientRoster").Groups(Contact.Groups[i]));
				}
				else
				{
					if (Contact.HTMLElements.Exists(Contact.Groups[i]))
						Contact.Hide(external.globals("ClientRoster").Groups(Contact.Groups[i]));
				}

		var resources = (new VBArray(Contact.Resources.Keys())).toArray();
		for (var i in resources)
		{
			var groups = (new VBArray(Contact.Resources(resources[i]).HTMLElements.Keys())).toArray()
			for (var j in groups)
				Contact.Resources(resources[i]).HTMLElements(groups[j]).style.display = Visible ? "block" : "none";
		}
	}

	function hideGroups ()
	{
		var groups = (new VBArray(external.globals("ClientRoster").Groups.Keys())).toArray();
		for (var i in groups)
			displayGroup(external.globals("ClientRoster").Groups(groups[i]), false, false, false, false);
	}

	function scanContacts ()
	{
		var contacts = (new VBArray(external.globals("ClientRoster").Items.Keys())).toArray();
		for (var i in contacts)
		{
			var contact = external.globals("ClientRoster").Items(contacts[i]);
			if (matchContact(contact))
			{
				displayContact(contact, true);
				for (var j in contact.Groups)
					displayGroup(external.globals("ClientRoster").Groups(contact.Groups[j]), true, true, true, false);
			}
			else
				displayContact(contact, false);
		}
	}

	function matchContact (Contact)
	{
		if (Contact.Address.ShortAddress().indexOf(ClientRosterSearch.Query) != -1)
			return true;
		if (Contact.Name.toLowerCase().indexOf(ClientRosterSearch.Query) != -1)
			return true;
		else
			return false;
	}

	function activateFirstContact ()
	{
		var list = external.globals("ClientRoster").HTMLElement.children;
		for (var i = 1; i < list.length; i += 3)
		{
			for (var j = 0; j < 2; ++j)
			{
				var contacts = list.item(i+j).children;
				for (var k = 0; k < contacts.length; ++k)
				{
					if (contacts.item(k).JID && (j == 1 || contacts.item(k).style.display == "block"))
					{
						ClientRosterSearch.Close();
						dial_chat(contacts.item(k).JID);
						return;
					}
				}
			}
		}
	}
}
