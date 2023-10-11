#include "JSONParser.h"

int main(){
    SD.begin(BUILTIN_SDCARD);
    Serial.begin(9600);
    json_parser::arduino_parser parser = json_parser::arduino_parser("device/config/sensors.config");
    Serial.println(parser.get("BMP388_SENSOR_FILE_PATH"));
    while(true){}
}