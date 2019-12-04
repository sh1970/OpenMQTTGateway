# Devices
**A list of verified compatible devices is maintained [here](https://docs.google.com/spreadsheets/d/1_5fQjAixzRtepkykmL-3uN3G5bLfQ0zMajM9OBZ1bx0/edit?usp=sharing).**

Added to that is an overview of devices supported by OpenMQTTGateway:
## For radio frequency devices 
OpenMQTTGateway can support a wide range of 433mhz/315mhz devices, all the ones with SC5262 / SC5272, HX2262 / HX2272, PT2262 / PT2272, EV1527, RT1527, FP1527, HS1527 chipsets are supported by the RF gateway. Added to that RF2 support Kaku and Pilight an [huge list](https://wiki.pilight.org/devices). 
Note that for the moment RF, RF2 and Pilight can not be activated on the same boards together.

![boards](../img/OpenMQTTGateway_devices_rf1.png ':size=250%')
![boards](../img/OpenMQTTGateway_devices_rf2.png ':size=250%')
![boards](../img/OpenMQTTGateway_devices_rf3.png ':size=250%')

## For BLE devices 
OpenMQTTGateway is able to scan all the BLE devices that advertise their data so as to do presence detection. Added to that it can retrieve the measures from the devices below.

|Devices|Measurements|
|-|:-:|
| BLE watches with fixed mac|rssi for presence detection|
| BLE beacons keychains|rssi for presence detection|
| XIAOMI Mi Flora |temperature/moisture/luminance/fertility|
| XIAOMI Mi Jia |temperature/humidity/battery|
| ClearGrass |temperature/humidity/battery|
| ClearGrass with atmospheric pressure |temperature/humidity/air pressure|
| LYWDS02 Clock |temperature/humidity|
| XIAOMI Mi Scale |weight|
| XIAOMI Mi band |steps|


![boards](../img/OpenMQTTGateway_devices_ble.png ':size=250%')

## For infrared IR devices 
the list of supported devices for ESP is [here](https://github.com/crankyoldgit/IRremoteESP8266/blob/master/SupportedProtocols.md), and [here](https://github.com/1technophile/OpenMQTTGateway/blob/6f73160d1421bebf2c1bbc9b8017978ff5b16520/main/config_IR.h#L123) for Arduino boards, as there is also the possibility of using raw and global caché (ESP)  sending possibilities of this gateway is huge!

## LORA
 is more dedicated at this moment for tinkering and DIY and there is no Off the shelves devices compatible to my knowledge with this gateway.
