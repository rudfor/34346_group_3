# Package Tracking Monitor
## 34346 Networking technologies and application development for Internet of Things

Project for DTU by: 
- s221652 - Erik Gottschalk
- s221856 - Juan Marin Collado
- s220019 - Sindhuja Ramamoorthy
- s183934 - Gustav Juul Jørgensen
- s137353 - Rudolf Anton Fortes

Simple IoT package tracker and monitor
Consists of Client with sensors relaying warnings to a hub 

## Package tracker P2P client

- Compile the sensors_lora
- Setup the wiring similar to this
  - breadboard example:
![Alt text](images/BreadBoard.png?raw=true "BreadBoard")
  - Schematic example:
![Alt text](images/SchematicAltium.png?raw=true "Schematic")
  - Schematic example2:
![Alt text](images/SchematicFritzing.png?raw=true "Schematic")
- Ensure you have the followin libraries insttalled
```
Name                      Installed                                
Adafruit_BME680_Library   2.0 2                                              
Adafruit_BusIO            1.14.1                                             
Adafruit_GFX_Library      1.11.5                                             
Adafruit_MPU6050          2.2.3                                             
Adafruit_SSD1306          2.5.7                                             
Adafruit_Unified_Sensor   1.1.9                                             
AllThingsTalk_LoRaWAN_SDK 3.1.6                                             
AnalogRTCLib              1.0.0                                              
DHT_sensor_library        1.4.4                                              
DS1307RTC                 1.4        
DS3231                    1.1.2                                              
RTClib                    2.1.1                                              
SD                        1.2.3       
TheThingsNetwork          2.7.2           
ThingsIoT                 1.2.0     
```

## IoT Hub LoRa P2P server

- Visit [AzureIoTCentral](https://apps.azureiotcentral.com) and create a `new application`.
- Select `Sample Devkits`
- Add a new `mxchip` device. (a real device) (under `Device Explorer`)
- Browse into device UI (by clicking to name of the device under `Device explorer`)
- Click/open `Connect` at top-right of the device UI
- Grab `scopeId`, `device Id` and `primary key` and fill the necessary parts under `gate_lora/lora_gate.ino`

```
#define WIFI_SSID "<ENTER WIFI SSID HERE>"
#define WIFI_PASSWORD "<ENTER WIFI PASSWORD HERE>"

const char* SCOPE_ID = "<ENTER SCOPE ID HERE>";
const char* DEVICE_ID = "<ENTER DEVICE ID HERE>";
const char* DEVICE_KEY = "<ENTER DEVICE primary/secondary KEY HERE>";
```

## Tips and Tools

Compile it! and deploy to your device. (see below)

- Download Arduino-CLI from [this link](https://github.com/arduino/arduino-cli#download-the-latest-stable-release)
- FOR WINDOWS, use version 0.3.3 => [download](https://github.com/arduino/arduino-cli/releases/download/0.3.3-alpha.preview/arduino-cli-0.3.3-alpha.preview-windows.zip)
- FOR WINDOWS, you will need a [driver for USB to UART](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
- Add additional url to board manager in arduino-cli.yml (this is usually located in the same folder of CLI executable)

```
board_manager:
  additional_urls:
  - http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
Setup the environment; (under the project folder)
```
arduino-cli-0.3.... core update-index
arduino-cli-0.3.... core install esp8266:esp8266
arduino-cli-0.3.... board attach esp8266:esp8266:nodemcu
```

Compile!
```
arduino-cli-0.3.... compile
```

Upload
```
arduino-cli-0.3.... upload -p <PORT / DEV?? i.e. => /dev/cu.SLAB_USBtoUART >
```

Monitoring?

```
npm install -g nodemcu-tool
```

Assuming the port/dev for the board is `/dev/cu.SLAB_USBtoUART`
```
nodemcu-tool -p /dev/cu.SLAB_USBtoUART -b 9600 terminal
```
