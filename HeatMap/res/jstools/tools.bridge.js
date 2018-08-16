// pipe errors to log
window.onerror = function (msg, url, num) {
	log("Error: " + msg + "\nURL: " + url + "\nLine: " + num);
};

// auto log for Qt and console
function log(logtext) {

	if (isQtAvailable) {
		QtBridge.js_debug(logtext.toString());
	}
	else {
		console.log(logtext);
	}
}

function notifyBridgeAvailable() {

    if (isQtAvailable) {
        QtBridge.js_available();
    }
}

function setViewBusy() {

    if (isQtAvailable) {
        QtBridge.js_viewIsBusy(true);
    }
}

function setViewAvailable() {

    if (isQtAvailable) {
        QtBridge.js_viewIsBusy(false);
    }
}

function clamp(min, val, max) {
	return Math.max(min, Math.min(val, max));
}

function cssSanitize(str) {

	//return str.replace(/(\:|\.|\[|\]|,)/g, "\\$1");
	return str.replace(/(!|#|\$|%|&|'|\(|\)|\*|\+|,|-|\.|\/|:|;|<|=|>|\?|@|\[|\]|\^|`|\{|\}|\||~)/g, "\\$1");
}

String.prototype.width = function (font) {
	var f = font || '12px arial';
	var o = document.createElement('div');
	o.innerHTML = this;
	o.style.visibility = "hidden";
	o.style.position = "absolute";
	o.style.float = "left";
	o.style.whiteSpace = "nowrap";
	o.style.font = f;

	document.getElementsByTagName('body')[0].appendChild(o);

	w = o.clientWidth;

	o.remove();

	return w;
}