<template>
  <div id="app">
    <section class="start" @click="connect()" v-if="!gatt">
      <img height="40" src="@/assets/images/Bluetooth.svg" class="bt-logo"/>
      Kliknij aby rozpocząć
    </section>
    <main>
      <section class="content">
        <div class="color-picker" ref="colorPicker"></div>
      </section>
      <nav>
        <ul class="menu">
          <li>
            <select id="section" v-model="section">
              <optgroup label="Podstawowe">
                <option value="0">Wszystko</option>
                <option value="1">1</option>
                <option value="2">2</option>
                <option value="3">3</option>
                <option value="4">4</option>
                <option value="5">5</option>
                <option value="6">6</option>
                <option value="7">7</option>
                <option value="8">8</option>
                <option value="9">9</option>
              </optgroup>
              <optgroup label="Dodatkowe">
                <option value="10">Przód</option>
                <option value="11">Tył</option>
                <option value="12">Biurko</option>
                <option value="13">Łóżko</option>
                <option value="14">Przód + drzwi</option>
                <option value="15">Tył + środek</option>
              </optgroup>
            </select>
          </li>
          <li @click="halloweenMode()"><img src="@/assets/images/pumpkin.svg" alt="H" height="24"></li>
          <li @click="thanosSnap()"><img src="@/assets/images/infinity-guantlet.svg" alt="I" height="24"></li>
          <li id="hotword-recognition"><img src="@/assets/images/microphone.svg" alt="I" height="24"></li>
          <li @click="powerBtn()"><i class="fa fa-power-off"></i></li>
          <li @click="disconnect()"><i class="fa fa-sign-out"></i></li>
        </ul>
      </nav>
    </main>
    <div :class="['snackbar', {'snackbar-show': snackbar.show}]">{{ snackbar.message }}</div>
  </div>
</template>

<script>
import "reinvented-color-wheel/css/reinvented-color-wheel.min.css";
import ReinventedColorWheel from "reinvented-color-wheel";

export default {
  name: 'App',
  data() {
    return {
      config: {
        device: {
          name: 'BT05M',
          service: '0000ffe0-0000-1000-8000-00805f9b34fb',
          characteristic: '0000ffe1-0000-1000-8000-00805f9b34fb'
        }
      },
      colorWheel: null,
      gatt: null,
      charstc: null,
      snackbar: {
        timeoutID: null,
        show: false,
        message: ''
      },
      sendStatus: false,
      powerStatus: false,
      //socket: io.connect(window.config.hotwordDetectionServer),
      recordAudio: null,
      recording: false,
      section: null,
    }
  },
  mounted() {
    this.colorWheel = new ReinventedColorWheel({
      appendTo: this.$refs.colorPicker,
      rgb: [255, 78, 0],
      wheelThickness: 30,
      onChange: color => {
        if(this.charstc && !this.sendStatus) {
          this.sendStatus = true;
          this.charstc.writeValue(new Uint8Array([parseInt(this.section) | 0b00100000, ...color.rgb])).finally(() => {
            this.sendStatus = false;
          });
        }
      }
    });
    this.colorWheel.wheelDiameter = window.innerWidth < window.innerHeight ? window.innerWidth*0.9 : window.innerHeight*0.7;
    this.colorWheel.redraw();
  },
  methods: {
    connect() {
      if(!navigator.bluetooth) {
          window.location.replace(`https:${location.href.substring(location.protocol.length)}`);
      }
      navigator.bluetooth.requestDevice({
          filters: [{
              services: [this.config.device.service]
          }]
      }).then(device => {
          this.gatt = device.gatt;
          return device.gatt.connect()
      })
      .then(server => server.getPrimaryService(this.config.device.service))
      .then(service => service.getCharacteristic(this.config.device.characteristic))
      .then(characteristic => {
          this.charstc = characteristic;
      })
      .catch(err => {
          console.log(err);
      });
    },
    disconnect() {
      if(this.gatt) {
        this.gatt.disconnect();
        this.gatt = null;
      }
    },
    showSnackbar(msg) {
      if(this.snackbar.timeoutID) {
        clearTimeout(this.snackbar.timeoutID);
      }
      this.snackbar.message = msg;
      this.snackbar.show = true;
      this.snackbar.timeoutID = setTimeout(() => {
        this.snackbar.show = false;
      }, 3000);
    },
    powerBtn() {
      this.send(parseInt(this.section), 1);
    },
    send(section, command) {
      if(!this.gatt || !this.charstc) {
        return this.showSnackbar('Nie połączono z urządzeniem');
      }
      this.charstc.writeValue(Uint8Array.of(section | (command << 4)));
    },
    halloweenMode() {
      this.send(parseInt(this.section), 3);
    },
    thanosSnap() {
      console.log('I am inevitable');
      this.send(parseInt(this.section), 4);
    }
  }
}
</script>

<style lang="scss">
#app {
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
}
body, html, #app {
  height: 100%;
}
body {
  margin: 0;
  text-align: center;
  color: #fff;
  font-family: 'Baloo Tamma 2', cursive;
  font-size: 20px;
  background-color: #555;
}
.btn {
  background-color: darksalmon;
  border-radius: 3px;
  border: 0;
  padding: 20px 25px;
  color: white;
  margin: 10px 0;
  font-size: 30px;
}
.btn:hover:not([disabled]) {
  cursor: pointer;
}
.btn[disabled] {
  filter: grayscale(0.7) brightness(1.5);
}
.btn-on {
  background-color: firebrick;
}
.btn-off {
  background-color: forestgreen;
}
.start {
  display: flex;
  width: 100%;
  height: 100%;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  position: absolute;
  top: 0;
  left: 0;
  transition: opacity 1s, visibility 1s linear;
  opacity: 1;
  visibility: visible;
  z-index: 2;
  background: linear-gradient(354deg, #4568DC 0%, #b06ab3 100%);
  user-select: none;
}
.bt-logo {
  margin-bottom: 15px;
}
.section-hidden {
  opacity: 0;
  visibility: hidden;
}
main {
  display: flex;
  flex-direction: column;
  height: 100%;
}
.content {
  flex-grow: 1;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
}
nav {
  background-color: #444;
}
.menu {
  padding: 0;
  margin: 0;
  height: 100%;
  display: flex;
  justify-content: center;
  list-style-type: none;
}
.menu li {
  padding: 15px;
  font-size: 1.2em;
  display: flex;
  cursor: pointer;
}
.menu li:hover {
  background-color: #4c4c4c;
}
.snackbar {
  position: fixed;
  bottom: 15px;
  left: 50%;
  transform: translateX(-50%);
  min-width: 250px;
  background-color: #333;
  border-radius: 10px;
  color: #fff;
  z-index: 3;
  font-size: 0.8em;
  opacity: 0;
  visibility: hidden;
  transition: opacity 1s, visibility 1s;
  padding: 2px 5px;
}
.snackbar-show {
  opacity: 1;
  visibility: visible;
}
</style>
