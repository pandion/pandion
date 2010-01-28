/* Chops up and filters XMPP addresses
 */
function XMPPAddress (address) {
	this.Host = "";
	this.User = "";
	this.Resource = "";
	this.Querytype = "";
	this.Keys = {};
	this.Fragment = "";

	this.Parse = Parse;
	this.LongAddress = LongAddress;
	this.CleanAddress = CleanAddress;
	this.ShortAddress = ShortAddress;

	if (address)
		this.Parse(address);

	function Parse (address) {
		this.Host = "";
		this.User = "";
		this.Resource = "";
		this.Querytype = "";
		this.Keys = {};
		this.Fragment = "";

		if (address.indexOf("#") !== -1) {
			this.Fragment = decodeURIComponent(address.substr(address.indexOf("#") + 1));
			address = address.substr(0, address.indexOf("#"));
		}

		if (address.indexOf("?") !== -1) {
			var query = address.substr(address.indexOf("?") + 1).split(";");
			this.Querytype = decodeURIComponent(query[0]).toLowerCase();
			for (var i = 1; i < query.length; i++) {
				var pair = query[i].split("=");
				this.Keys[decodeURIComponent(pair[0])] = decodeURIComponent(pair[1]);
			}
			address = address.substr(0, address.indexOf("?"));
		}

		if (address.indexOf("/") !== -1) {
			this.Resource = decodeURIComponent(address.substr(address.indexOf("/") + 1));
			address = address.substr(0, address.indexOf("/"));
		}

		if (address.indexOf("@") !== -1) {
			this.User = decodeURIComponent(address.substr(0, address.indexOf("@"))).toLowerCase();
			this.Host = decodeURIComponent(address.substr(address.indexOf("@") + 1)).toLowerCase();
		} else
			this.Host = decodeURIComponent(address).toLowerCase();
	}

	/* Returns the address without the resource
	 */
	function ShortAddress () {
		if (this.User.length)
			return this.User + "@" + this.Host;
		else
			return this.Host;
	}

	/* Returns the address including the resource
	 */
	function LongAddress () {
		if (this.Resource.length)
			return this.ShortAddress() + "/" + this.Resource;
		else
			return this.ShortAddress();
	}

	/* Returns the short address without the transport
	 */
	function CleanAddress () {
		if (external.globals("ClientServices").Services.Exists(this.Host))
			return this.User.length ? this.User.replace(/%/, "@") : this.Host;
		else
			return this.ShortAddress();
	}
}
