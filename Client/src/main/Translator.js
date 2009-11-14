function Translator ()
{
	this.CodeCache = {};
	this.Direction = false; // false: LTR, true: RTL
	this.HTMLCache = {};

	this.GetSoftwareNameT10n = GetSoftwareNameT10n;
	this.LoadFile = LoadFile;
	this.Reload = Reload;
	this.Translate = Translate;
	this.TranslateWindow = TranslateWindow;

	function Reload () {
		this.Direction = false;
		this.CodeCache = {};
		this.HTMLCache = {};

		var path = external.globals("cwd") + "..\\languages\\";
		var language = "en";
		var userLanguage = navigator.userLanguage.toLowerCase();
		if (userLanguage == "zh-hk"|| userLanguage == "zh-sg")
			userLanguage = "zh-tw";

		this.LoadFile(path + language + ".xml");

		if (external.FileExists(path + external.globals("language") + ".xml"))
			language = external.globals("language");
		else if (external.FileExists(path + userLanguage + ".xml"))
			language = userLanguage;
		else if (userLanguage.indexOf("-") > 0 && external.FileExists(path + userLanguage.substr(0, userLanguage.indexOf("-")) + ".xml"))
			language = userLanguage.substr(0, userLanguage.indexOf("-"));

		if (language != "en")
			this.LoadFile(path + language + ".xml");

		external.globals("language") = language;

		external.globals("softwarename") = this.GetSoftwareNameT10n();
	}

	function GetSoftwareNameT10n () {
		var dom = new ActiveXObject("MSXML2.DOMDocument");
		dom.async = false;
		dom.resolveExternals = false;
		dom.load(external.globals("cwd") + "..\\settings\\brand.xml");
		var appLanguageName, appLanguagePrefixName, userLanguageName, userLanguagePrefixName, defaultName = external.globals("softwarename");
		if (dom.documentElement) {
			var appLanguage = external.globals("language").toLowerCase();
			var appLanguagePrefix = appLanguage.split("-")[0];
			var userLanguage = navigator.userLanguage.toLowerCase();
			if (userLanguage == "zh-hk"|| userLanguage == "zh-sg")
				userLanguage = "zh-tw";
			var userLanguagePrefix = userLanguage.split("-")[0];
			var names = dom.documentElement.selectNodes("/brand/softwarename");
			for (var i = 0; i < names.length; i++) {
				var xmlLang = names[i].getAttribute("xml:lang") !== null ? names[i].getAttribute("xml:lang").toLowerCase() : "";
				switch (xmlLang) {
					case appLanguage: appLanguageName = names[i].text; break;
					case appLanguagePrefix: appLanguagePrefixName = names[i].text; break;
					case userLanguage: userLanguageName = names[i].text; break;
					case userLanguagePrefix: userLanguagePrefixName = names[i].text; break;
					case "en":
					case "":
						defaultName = names[i].text; break;
				}
			}
		}
		return appLanguageName || appLanguagePrefixName || userLanguageName || userLanguagePrefixName || defaultName;
	}

	function LoadFile (path) {
		var dom = new ActiveXObject("MSXML2.DOMDocument");
		dom.async = false;
		dom.resolveExternals = false;
		dom.load(path);
		if (dom.documentElement == null)
			return;

		if (dom.documentElement.getAttribute("dir") !== null)
			this.Direction = dom.documentElement.getAttribute("dir").toString().toLowerCase() == "rtl";

		var stringFilter = function ($0, $1) {return external.globals.Exists($1) ? external.globals($1) : $0};

		var windowNodes = dom.documentElement.selectNodes("/translation/window[@name]");
		for (var i = windowNodes.length - 1; i >= 0; --i) {
			var windowName = windowNodes.item(i).getAttribute("name");
			if (!(windowName in this.CodeCache))
				this.CodeCache[windowName] = {};
			if (!(windowName in this.HTMLCache))
				this.HTMLCache[windowName] = {};

			var iterator = windowNodes.item(i).selectNodes("./item[@id]");
			for (var j = iterator.length - 1; j >= 0; --j) {
				var itemTag = iterator.item(j);
				var data = {};
				var childTags = itemTag.childNodes;
				for (var k = childTags.length - 1; k >= 0; --k)
					data[childTags.item(k).tagName] = childTags.item(k).text.replace(/\$\{(\w+)\}/mg, stringFilter);
				this.HTMLCache[windowName][itemTag.getAttribute("id")] = data;
			}

			iterator = windowNodes.item(i).selectNodes("./code[@id]");
			for (var j = iterator.length - 1; j >= 0; --j)
				this.CodeCache[windowName][iterator.item(j).getAttribute("id")] = iterator.item(j).text.replace(/\$\{(\w+)\}/mg, stringFilter);
		}
	}

	function Translate (windowName, stringName, variables) {
		if (!(windowName in this.CodeCache))
			return "";
		if (!(stringName in this.CodeCache[windowName]))
			return "";
		return this.CodeCache[windowName][stringName].replace(
			/\${(\d+)}/mg,
			function ($0, $1) {
				if (variables && $1 < variables.length)
					return variables[$1];
				return $0;
			}
		);
	}

	function TranslateWindow (translationWindowName, DOMDocument, externalWindow) {
		if (!externalWindow)
			externalWindow = DOMDocument.parentWindow.external.wnd;

		externalWindow.rightToLeft(this.Direction);
		if (DOMDocument.documentElement.dir == "rtl" && !this.Direction)
			DOMDocument.dir = DOMDocument.documentElement.dir = DOMDocument.documentElement.className = "ltr";
		else if (DOMDocument.documentElement.dir != "rtl" && this.Direction)
			DOMDocument.dir = DOMDocument.documentElement.dir = DOMDocument.documentElement.className = "rtl";

		if (translationWindowName in this.HTMLCache)
			for (var name in this.HTMLCache[translationWindowName]) {
				var htmlElem = DOMDocument.getElementById(name);
				if (htmlElem)
					for (var textItem in this.HTMLCache[translationWindowName][name])
						htmlElem[textItem] = this.HTMLCache[translationWindowName][name][textItem];
			}
	}
}
