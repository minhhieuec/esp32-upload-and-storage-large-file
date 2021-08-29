# esp32-upload-and-storage-large-file

This project allow user upload and storage large file in ESP32.
To save file to flash i using Little FS instead of SPIFFS because SPIFFS cannot use full memory in setting partition (https://github.com/lorol/LITTLEFS/issues/10).

## Installation

1. Install esp32littlefs plugin(https://github.com/lorol/arduino-esp32littlefs-plugin)
2. Add ESP32WebServer lib (https://github.com/Pedroalbuquerque/ESP32WebServer)
3. Add LittleFS lib (https://github.com/lorol/LITTLEFS)

## Configure

1. Select board: `Tools` >> `ESP32 Dev Module`
2. upload file to flash: `Tools` >> `ESP32 LittleFS Data Upload`
3. Select Flash size: `Tools` >> `4MB`
4. Select Partition Scheme: `No OTA (1MB APP/3MB SPIFFS)`

![select partition scheme](docs/img/select_partition_scheme.jpg)

5. Select COM Port: `Tools` >> `Port`
6. Compile and upload

## Usage
1. Connect to ESP32 Access Point: ssid `ESP32`, password `12345678`
2. Open Web Browser and type `192.168.4.1/upload`

![upload file](docs/img/upload.jpg)

3. Select text file wanna upload to ESP32

## Fix errors
1. mklittlefs not found!

Easy to fix this error is install esp8266 board on arduino (AppData\Local\Arduino15\packages\esp8266\tools\mkspiffs\3.0.4-gcc10.3-1757bed):
- `File` >> `Preferences` >> `Additional Board Manager URLs` >> `https://dl.espressif.com/dl/package_esp32_index.json, https://arduino.esp8266.com/stable/package_esp8266com_index.json`
- `Tools` >> `Board` >> `Boards Manager ...` >> `type and intall esp8266 by ESP8266 Community`


## References
- https://github.com/lorol/LITTLEFS
- https://github.com/G6EJD/ESP32-8266-File-Upload/blob/master/ESP_File_Download_Upload.ino
