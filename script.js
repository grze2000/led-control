const config = {
    device: {
        name: 'BT05M',
        service: '0000ffe0-0000-1000-8000-00805f9b34fb',
        characteristic: '0000ffe1-0000-1000-8000-00805f9b34fb'
    }
}

let colorWheel = null;
let gatt = null;
let charstc = null;
let snackbarTimeout = null;
let sendStatus = false;
let powerStatus = false;
var socket = io.connect(window.config.hotwordDetectionServer);
let recordAudio;
let recording = false;

function startRecording() {
    navigator.getUserMedia({ audio: true, video: false}, mediaStream => {
        recordAudio = RecordRTC(mediaStream, {
            type: 'audio',
            mimeType: 'audio/webm',
            sampleRate: 44100,
            desiredSampRate: 16000,
            recorderType: StereoAudioRecorder,
            numberOfAudioChannels: 1,
            timeSlice: 1000,
            ondataavailable: blob => {
                let stream = ss.createStream();
                ss(socket).emit('stream', stream, {size: blob.size});
                ss.createBlobReadStream(blob).pipe(stream);
            }
        });
        recordAudio.startRecording();
    }, err => {
        console.log(err);
    });
}

function stopRecording() {
    recordAudio.stopRecording(() => {
        recordAudio.getDataURL(audioDataURL => {
            socket.emit('stream', {
                type: recordAudio.getBlob().type || 'audio/wav',
                data: audioDataURL
            });
        });
    });
}

socket.on('hotword', hotword => {
    console.log('Hotword detected:' + hotword);
    if(hotword == 'snap') {
        send(parseInt($('#section').val()), 4);
    }
});

const send = (section, command) => {
    if(!gatt || !charstc) {
        return showSnackbar('Nie połączono z urządzeniem');
    }
    charstc.writeValue(Uint8Array.of(section | (command << 4)));
}

$(() => {
    if (location.protocol !== 'https:') {
        location.replace(`https:${location.href.substring(location.protocol.length)}`);
    }
    $('.actual-color').css('background-color', 'rgb(255, 78, 0)');
    colorWheel = new ReinventedColorWheel({
        appendTo: document.querySelector('.color-picker'),
        rgb: [255, 78, 0],
        wheelThickness: 30,
        onChange: color => {
            if(charstc && !sendStatus) {
                sendStatus = true;
                charstc.writeValue(new Uint8Array([parseInt($('#section').val()) | 0b00100000, ...color.rgb])).finally(() => {
                    sendStatus = false;
                    $('.actual-color').css('background-color', color.hex);
                });
            }
        }
    });
    colorWheel.wheelDiameter = window.innerWidth < window.innerHeight ? window.innerWidth*0.9 : window.innerHeight*0.7;
    colorWheel.redraw();

    $('.start').on('click touch', function() {
        if(!navigator.bluetooth) {
            window.location.replace(`https:${location.href.substring(location.protocol.length)}`);
        }
        navigator.bluetooth.requestDevice({
            filters: [{
                services: [config.device.service]
            }]
        }).then(device => {
            gatt = device.gatt;
            return device.gatt.connect()
        })
        .then(server => server.getPrimaryService(config.device.service))
        .then(service => service.getCharacteristic(config.device.characteristic))
        .then(characteristic => {
            $(this).addClass('section-hidden');
            charstc = characteristic;
        })
        .catch(err => {
            console.log(err);
        });
    });

    $('#disconnect').on('click touch', function() {
        if(gatt) {
            gatt.disconnect();
        }
        $('.start').removeClass('section-hidden');
    });

    $('#light-switch').on('click touch', function() {
        send(parseInt($('#section').val()), 1);
    });

    $('#halloween-mode').on('click touch', function() {
        send(parseInt($('#section').val()), 3);
    });

    $('#thanos-snap').on('click touch', function() {
        console.log('I am inevitable');
        send(parseInt($('#section').val()), 4);
    });

    $('#hotword-recognition').on('click touch', function() {
        if(recording) {
            stopRecording();
            $(this).find("img").attr("src", "img/microphone.svg");
        } else {
            startRecording();
            $(this).find("img").attr("src", "img/microphone-on.svg");
        }
        recording = !recording;
    });
});
function showSnackbar(msg) {
    $('.snackbar').text(msg).addClass('snackbar-show');
    if(snackbarTimeout) {
        clearTimeout(snackbarTimeout);
    }
    snackbarTimeout = setTimeout(() => {
        $('.snackbar').removeClass('snackbar-show');
    }, 3000);
}