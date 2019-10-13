// --------------- WebSocket handling ------------
ws = new WebSocket("ws://192.168.178.6/ws");
var wsTimeout = setTimeout(function() {
	document.getElementById('inputlockicon').className.replace("fas fa-lock", "fas fa-sync-alt");
	document.getElementById('inputlocktext').style.display = "block";
	document.getElementById('inputlockreload').style.pointerEvents = "auto";		
}, 5000);

ws.onopen = function() {
	console.log("WS: Connected");
	clearTimeout(wsTimeout);
	document.getElementById('inputlock').style.display = "none";
	AJAX_JSON_Req();
};

ws.onclose = function() {
	console.log("WS: Connection closed");
	document.getElementById('inputlock').style.display = "block";
	document.getElementById('inputlockicon').className.replace("fas fa-lock", "fas fa-sync-alt");
	document.getElementById('inputlocktext').style.display = "block";
	document.getElementById('inputlockreload').style.pointerEvents = "auto";
};

ws.onmessage = function(evt) {
    //textbox.value = evt.data;
    //slider.value = evt.data;
}

function sendData(type, data) {
    var msg = "0x";
    switch (type) {
        case 01:
        case "effectchange":
            msg += "01" + data;
            break;
        case 02:
        case "effect1_data":
            msg += "02" + data; 
            break;

        default:
            break;
    }

    if (msg != "0x")
        ws.send(msg);
}

function closeWebsocket() {
    ws.close();
}

var lastcall = Date.now();
function msGoneBy(ms) {
    if (ms < 0 || ms === undefined)
    {
        ms = 50;
    }
    
    if (Date.now() - lastcall >= ms)
    {
        lastcall = Date.now();
        return true;
    }
    
    console.log("WS: limiter engaged!");
    return false;
}

// --------------- Defaults JSON -----------------

var settings;
var activeEffect;
function AJAX_JSON_Req()
{
    var AJAX_req = new XMLHttpRequest();
    AJAX_req.open("GET", "/settings.json", true);
    AJAX_req.setRequestHeader("Content-type", "application/json");
    AJAX_req.onreadystatechange = function() {
        if( AJAX_req.readyState == 4 && AJAX_req.status == 200 )
        {
            settings = JSON.parse( AJAX_req.responseText );
            writeDefaults();
        }
    }
    AJAX_req.send();
}

function writeDefaults() {
    textbox.value = settings.brightness;
    slider.value = settings.brightness;
    activeEffect = settings.activeEffect;
    changeEffect('input', "effect" + activeEffect, false);

    colorPicker.color.hexString = settings.effect1Data.color1;

    console.log("Settings loaded");
    console.log("Ready")
}

// --------------- Event handler -----------------

// textbox
var textbox = document.getElementById('textbox');
textbox.addEventListener('keyup', function(event) {
    event.preventDefault();
    if (event.keyCode === 13) {
        sendButton.click();
    }
});

// slider
var slider = document.getElementById('slider');
slider.addEventListener('input', function() {
    if (msGoneBy(25)) {
        //sendData(slider.value);
    }
});

// sendButton
var sendButton = document.getElementById('sendButton');
sendButton.addEventListener('click', function() {
    //sendData(textbox.value);
});

// sidebar
var sidebar = document.getElementById('sidebar');
var overlay = document.getElementById('overlay');
function openSidebar() {
    sidebar.style.display = "block";
    overlay.style.display = "block";
}

function closeSidebar() {
    sidebar.style.display = "none";
    overlay.style.display = "none";
}

function changeEffect(evt, newEffect, sendChange) {
    var i, x, effectlinks;
    x = document.getElementsByClassName("effect");
    for (i = 0; i < x.length; i++) {
        x[i].style.display = "none";
    }
    effectlinks = document.getElementsByClassName("effectlink");
    for (i = 0; i < effectlinks.length; i++) {
        effectlinks[i].className = effectlinks[i].className.replace(" w3-deep-orange", "");
    }
    document.getElementById(newEffect).style.display = "block";
    document.getElementById(newEffect.replace("effect", "effectlink")).className += " w3-deep-orange";

    if (sendChange) {
        activeEffect = Number(newEffect.replace("effect", ""));
        sendData("effectchange", activeEffect);    
    }
}

// ----------------- color picker ----------------
var colorPicker = new iro.ColorPicker('#effect1color', {
    color: "#ff0000",
    width: 200,
    layout: [{
        component: iro.ui.Wheel,
        options: {}
    }]
});

colorPicker.on('input:change', onColorChange);
colorPicker.on('mount', function(){
    assd = document.getElementsByClassName("iro__colorPicker");
    assd[0].style.margin = "auto";
})

function onColorChange(color, changes) {
    if (msGoneBy(25)) {
        sendData("effect1_data", color.hexString);
        console.log(color.hexString);
    }
}
    
// --------------- other functions ---------------

function disableElement(id) {
    var element = document.getElementById(id);
    if (!element.classList.contains('w3-disabled')) {
        element.classList.add('w3-disabled');
    }
}

function enableElement(id) {
    var element = document.getElementById(id);
    if (element.classList.contains('w3-disabled')) {
        element.classList.remove('w3-disabled');
        element.disabled = false;
    }
}