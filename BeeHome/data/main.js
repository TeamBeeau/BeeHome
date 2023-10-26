
var ws;
var s = "";
window.onload = function () {

	var wage = document.getElementById("p");
	wage.addEventListener("keydown", function (e) {
		if (e.code === "Enter") {  //checks whether the pressed key is "Enter"
			Submit();
		}
	});

	var wage1 = document.getElementById("s");
	wage1.addEventListener("keydown", function (e) {
		if (e.code === "Enter") {  //checks whether the pressed key is "Enter"
			document.getElementById("p").focus();
		}
	});


    var _url = location.hostname;
    _url = "ws://" + _url + ":81";
    ws = new WebSocket(_url);
    ws.onopen = function(evt) { onOpen(evt) };
    ws.onclose = function(evt) { onClose(evt) };
    ws.onmessage = function(evt) { onMessage(evt) };
    ws.onerror = function(evt) { onError(evt) };
}; 
		
function onOpen(evt) {
    scan();
};

var _timeout = false;
function time_out() {
	_timeout = false;
}
function clear_div() {
	const element = document.getElementById("myDIV");
	while (element.getElementsByTagName("div").length > 0) {
		element.getElementsByTagName("div")[0].remove();
	}
}
function scan() {
	if (_timeout) return;
	clear_div();
	ws.send("Scan");
	_timeout = true;
	setTimeout(time_out, 5000);
};

function change(){
    x = document.getElementById("suggest_select").value;
    document.getElementById("SSID").value = x;
};

function c(l) {
	document.getElementById('s').value = l.innerText || l.textContent;
	p = l.nextElementSibling.classList.contains('l');
	document.getElementById('p').disabled = !p;
	if (p) document.getElementById('p').focus()
};

function f() { var x = document.getElementById('p'); x.type === 'password' ? x.type = 'text' : x.type = 'password'; }


function Submit(){
    var c1 = document.getElementById("s").value;
    var c2 = document.getElementById("p").value;
		  
    if (c1.value == ""){
    alert("Import SSID!");
    }
	else {
	var sen = "SSID:" + c1 + "," + c2;
	ws.send(sen);
	window.location.replace("finish.html");
	}
		  
		  
};

function onError(evt){
    alert("Connect error!");
    location.reload();
};

function onMessage(evt){
	var cmt = evt.data.slice(0, 3);
	if (cmt == "ADD") {
		clear_div();
		var data;
		data = evt.data.slice(4);
		var selec = document.getElementById("myDIV");
		var _index = data.indexOf(';');
		while (_index > 0) {
			var _index2 = data.indexOf(',');
			if (_index2 > 0) {
				let ss = data.slice(0, _index2);
				var data2 = data.slice(_index2 + 1);
				var _index3 = data2.indexOf(',');
				let qs = data2.slice(0, _index3);
				let qsc = data2.slice(_index3 + 1, _index3 + 2);
				var qqs = parseInt(qs);
				var clasname = 'q q-';
				if (qqs > 80) clasname += '4';
				else if (qqs > 60) clasname += '3';
				else if (qqs > 40) clasname += '2';
				else if (qqs > 20) clasname += '1';
				else clasname += '0';
				if (qsc == '1') clasname += ' l';

				const t1 = document.getElementById("temp").cloneNode(true);
				const c = t1.getElementsByTagName('div');
				t1.style.display = "block";
				t1.getElementsByTagName('a')[0].innerHTML = ss;
				t1.getElementsByTagName('div')[0].className = clasname;
				const element = document.getElementById("myDIV");
				element.appendChild(t1);

			}
			//
			data = data.slice(_index + 1);
			_index = data.indexOf(';');
		}
		_timeout = false;
	}
};

function onClose(evt){

};

        