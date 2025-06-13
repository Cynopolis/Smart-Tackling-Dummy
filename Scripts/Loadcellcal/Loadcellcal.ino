#include <HX711.h>
HX711 loadCell;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  loadCell.begin(13, 12);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(loadCell.read());
}
