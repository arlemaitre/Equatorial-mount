document.addEventListener("deviceready", onDeviceReady, false);


function onDeviceReady() {
    console.log("Running cordova-" + cordova.platformId + "@" + cordova.version);
    initialize();
}

let macAddress = "";
let chars = "";
let speed = 0;
let pole = "";
let speedButtonTouch = false;
let poleButtonTouch = false;
let boolEnabledConfirm = false;

document.addEventListener('click', function(e) {
    if (hasClass(e.target, 'deviceButton')) {
        macAddress = e.target.id;
        document.getElementById('error').innerHTML = " Try to connect to the device...";
        connectDevice();
    }
    if (hasClass(e.target, 'speedButton')) {
        speedButtonTouch = true;
        if (!boolEnabledConfirm) {
            enabledConfirm();
        }
        speed = e.target.id;
        onlyOne(e.target, "speedButton");
    }
    if (hasClass(e.target, 'poleButton')) {
        poleButtonTouch = true;
        if (!boolEnabledConfirm) {
            enabledConfirm();
        }
        pole = e.target.id;
        onlyOne(e.target, "poleButton");
    }
});
document.getElementById('disconnectButton').addEventListener('click', () => {
    disconnectDevice();
})

document.getElementById('send').addEventListener('click', () => {
    console.log("submit: " + speed + ":" + pole);
    writeSomethings(speed + ":" + pole);
})

function enabledConfirm() {
    if (speedButtonTouch && poleButtonTouch) {
        console.log("enable");
        document.getElementById('send').disabled = false;
        boolEnabledConfirm = true;
    }
}

function hasClass(elem, className) {
    return elem.classList.contains(className);
}

function onlyOne(checkbox, className) {
    let checkboxes = document.getElementsByClassName(className);
    for (let i = 0; i < checkboxes.length; i++) {
        if (checkboxes[i] !== checkbox) checkboxes[i].checked = false
    }
}

function initialize() {
    bluetoothSerial.isEnabled(() => { listDevice() }, () => { notEnabled() });
};

function notEnabled() {
    display("Bluetooth is not enabled.");
}

function listDevice() {
    bluetoothSerial.list(
        function(results) {
            for (let i = 0; i < results.length; i++) {
                let obj = results[i];
                console.log(results[i]);
                display('<button class="deviceButton" id="' + obj.address + '">' + obj.name + '</button>');
            }

        },
        function(e) {
            display(JSON.stringify(e));
        }
    );
}

function writeSomethings(message) {
    bluetoothSerial.write(message);
};

function connectDevice() {
    bluetoothSerial.connect(
        macAddress, // device to connect to
        () => { showUi() }, // start listening if you succeed
        (e) => {
            console.log("connect")
            showError(e)
            if (e == "Unable to connect to device") {
                document.getElementById('error').innerHTML = e;
            } else if (e == "Device connection was lost") {
                disconnectDevice();
            }
        }
    );
}

function showUi() {
    openPort();
    clear();
    document.getElementById('disconnectButton').style.display = "block";
    document.getElementById('ui').style.display = "flex";
    document.getElementById('message').style.display = "none";
    document.getElementById('title').innerHTML = "Select options: <span id='data'>waiting</span>";
}

function showDevice() {
    closePort();
    document.getElementById('ui').style.display = "none";
    document.getElementById('message').style.display = "flex";
    document.getElementById('title').innerHTML = "Select device: <span id='error'></span>";
    listDevice();
}

function disconnectDevice() {
    document.getElementById('disconnectButton').style.display = "none";
    bluetoothSerial.disconnect(
        () => { showDevice() }, // start listening if you succeed
        (e) => { showError(e) } // show the error if you fail
    );
}

function openPort() {
    console.log("suscribe");
    bluetoothSerial.subscribe('\n', function(data) {
        console.log("reçu: " + data);
        document.getElementById('data').innerText = data;
    });
}

function closePort() {
    bluetoothSerial.unsubscribe(
        function() {
            clear();
        },
        (e) => { showError(e) }
    );
}

function showError(error) {
    console.log("error: " + error);
};

function display(message) {
    let display = document.getElementById("message");
    display.insertAdjacentHTML("beforeend", message);
};

function clear() {
    let display = document.getElementById("message");
    display.innerHTML = "";
};