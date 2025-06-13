/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "I2C_IMU.h"
#include "I2C_Accel.h"
#include "I2C_Temp.h"
#include "LoadCell.h"
#include "PINOUT.h"
#include "LEDStrip.h"
#include "ErrorLight.h"
#include "IndicatorLight.h"
#include "BluetoothSerialMessage.h"
#include "SDCard.h"
#include "ControlPanel.h"

// set up sensor headers
char head[] = "HEAD";
char body[] = "BODY";
char leftShoulder[] = "LftShldr";
char rightShoulder[] = "RgtShldr";
char concussionLabel[] = "Concussion";

/**
 * This section defines all of the sensors
*/
// create the I2C devices with placeholder values
TwoWire wire0(0);
TwoWire wire1(1);

I2C_Temp temp(&wire0, 0x38);
I2C_IMU bodyIMU(&wire0, 0x6A, body);
I2C_Accel bodyAccel(&wire0, 0x18, body);
I2C_IMU headIMU(&wire1, 0x6A, head);
I2C_Accel headAccel(&wire1, 0x18, head);

LoadCell leftLoadCell(LOAD_CELL1_DAT_PIN, LOAD_CELL1_CLK_PIN);
LoadCell rightLoadCell(LOAD_CELL2_DAT_PIN, LOAD_CELL2_CLK_PIN);

// Create a SerialMessage object
SerialMessage serialMessage;
BluetoothSerial bleSerial;
BluetoothSerialMessage bleSerialRead(&bleSerial);

bool bootup_errors_shown = false;
// each respective index is tru if the given thing is not initialized:
// 0: body IMU, 1: body accel, 2: temperature, 3: sd card
byte startup_errors = 0b00000000;


/**
 * This section define all needed functions and variables for the status lights
*/
// define functions
double headAccelMag(){
  double *IMUData = headIMU.getData();
  if(!headIMU.isInitialized() || !headAccel.isInitialized()){
    return 100;
  }
  if(IMUData[0] > 15 || IMUData[1] > 15 || IMUData[2] > 15){
    double *accelData = headAccel.getData();
    return sqrt(accelData[0]*accelData[0] + accelData[1]*accelData[1] + accelData[2]*accelData[2]);
  }
  return sqrt(IMUData[0]*IMUData[0] + IMUData[1]*IMUData[1] + IMUData[2]*IMUData[2]);
};

double headGyroMag(){
  if(!headIMU.isInitialized()){
    return 8000;
  }
  double *data = headIMU.getData();
  return sqrt(data[3]*data[3] + data[4]*data[4] + data[5]*data[5]);
};


double leftLoadCellMag(){
  if(!leftLoadCell.isInitialized()){
    return 500;
  }
  return leftLoadCell.getData();
};

double rightLoadCellMag(){
  if(!rightLoadCell.isInitialized()){
    return 500;
  }
  return rightLoadCell.getData();
};

double getBatteryVoltage(){
  return double(analogRead(BATTERY_PIN)) * 100 / 65535;
};

double concussionProbability(){
  double *data1 = bodyIMU.getPeaks();
  if(!headIMU.isInitialized() || !headAccel.isInitialized()){
    return 1;
  }

  double gyroMag = sqrt(data1[3]*data1[3] + data1[4]*data1[4] + data1[5]*data1[5]);
  double accelMag;
  // if the low g accelerometer is saturated, use the high g accelerometer
  if(data1[0] > 15 || data1[1] > 15 || data1[2] > 15){
    double *data2 = bodyAccel.getPeaks();
    accelMag = sqrt(data2[0]*data2[0] + data2[1]*data2[1] + data2[2]*data2[2]);
  }
  else{
    accelMag = sqrt(data1[0]*data1[0] + data1[1]*data1[1] + data1[2]*data1[2]);
  }

  // return the probability of concussion. This was given by the paper:
  // Brain Injury Prediction: Assessing the Combined Probability of Concussion Using Linear and Rotational Head Acceleration
  return 1/(1 + exp(-(-10.2 + 0.0433*accelMag + 0.000873*gyroMag - 0.00000092*accelMag*gyroMag)));
};

DataStream<double> concussionStream = DataStream<double>();


bool impactDetected = false;
bool concussionDetected = false;

// define all of the status lights
IndicatorLight indic1(0, concussionProbability, 0, 1, true, false);
IndicatorLight indic2(1, leftLoadCellMag, 0, 500, true, false);
IndicatorLight indic3(2, rightLoadCellMag, 0, 500, true, false);
IndicatorLight indic4(3, headGyroMag, 0, 8000, true, false);
IndicatorLight indic5(4, headAccelMag, 0, 100, true, false);
IndicatorLight indic6(5, getBatteryVoltage, 0, 100, true, true);
ErrorLight error1(6);
ErrorLight error2(7);

// set up the led strip
StatusLight * statusLights[] = {
  &indic1,
  &indic2,
  &indic3,
  &indic4,
  &indic5,
  &indic6,
  &error1,
  &error2
};

LEDStrip leds(LED_STRIP_PIN, statusLights, 8);

// create the SD card object
SDCard sdCard(SPI_CS_PIN);
char extension[] = ".csv";
char dynamicFilename[] = "/impact";

/**
 * This section defines all of the tasks
 * and other variables needed for task management
*/

// Control Panel
IRCom ir(IR_PIN);
ControlPanel controlPanel(LEFT_BUTTON_PIN, RIGHT_BUTTON_PIN, TOGGLE_BUTTON_PIN, RESET_BUTTON_PIN, &leds, &ir);

// create a task handle for parralel processing
TaskHandle_t updateIMUTask;
TaskHandle_t updateControlPanelTask;
TaskHandle_t updateTempTask;
TaskHandle_t printDataTask;
TaskHandle_t updateLoadCellTask;
TaskHandle_t readSerialTask;
TaskHandle_t updateSDCardTask;
TaskHandle_t showStartupErrorsTask;

// create a mutex to prevent multiple tasks from accessing the same data at the same time
SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

// update the sd card data ONCE
void updateSDCard(void * parameter){
  unsigned long time = 0;
  for(;;){
    xSemaphoreTake(mutex, portMAX_DELAY);
    if(impactDetected || concussionDetected){
      if(sdCard.isFileOpen() && (millis() - time > 3000)){
        sdCard.setFileNumber(sdCard.getFileNumber() + 1);
        Serial.print("New recording started #: ");
        Serial.println(sdCard.getFileNumber());
      }
      
      time = millis();
    }
    if(millis() - time < 2000){
      sdCard.update(true);
    }
    if(millis() - time >= 2000){
      impactDetected = false;
      concussionDetected = false;
      sdCard.closeFile();
      bodyIMU.resetPeaks();
      bodyAccel.resetPeaks();
      headIMU.resetPeaks();
      headAccel.resetPeaks();
      leftLoadCell.resetPeaks();
      rightLoadCell.resetPeaks();
    }
    xSemaphoreGive(mutex);

    // speed up this task while recording
    if(millis() - time < 2000){
      delay(2);
    }
    else{
      delay(100);
    }
  }
  vTaskDelete(NULL);
}

void updateIMU(void * parameter){
  for(;;){
    xSemaphoreTake(mutex, portMAX_DELAY);
    if(bodyIMU.isInitialized()){
      bodyIMU.update();
      if(bodyIMU.getAccelPeak() > 5){
        Serial.println("Impact Detected by Body IMU");
        impactDetected = true;
      }
    }
    if(bodyAccel.isInitialized()){
      bodyAccel.update();
    }
    if(headIMU.isInitialized()){
      headIMU.update();
      if(headIMU.getAccelPeak() > 5){
        Serial.println("Impact Detected by Head IMU");
        impactDetected = true;
      }
    }
    if(headAccel.isInitialized()){
      headAccel.update();
    }
    
    // only calculate concussion probability if an impact has not yet been detected.
    // once it has been detected, calcualting that probability is someone else's job
    if(headIMU.isInitialized() && headAccel.isInitialized()){
      double prob = concussionProbability();
      concussionStream.prepend(prob);
      // Serial.print("Probability: ");
      // Serial.println(prob,8);
      if(prob > 0.25){
        Serial.println("Concussion Detected");
        concussionDetected = true;
      }
    }
    xSemaphoreGive(mutex);
    
    // gives the ability for other tasks to run like bluetooth
    delay(2);
  }

  // in case the loop ever needs to exit, delete the task
  vTaskDelete(updateIMUTask);
}

// update the load cell data
void updateLoadCells(void * parameter){
  while(true){
    xSemaphoreTake(mutex, portMAX_DELAY);
    if(leftLoadCell.isInitialized()){
    leftLoadCell.setCurrentTemp(temp.getData()[0]);
      leftLoadCell.update();
      // TODO: Change this inequality when the load cell is calibrated
      if(leftLoadCell.getPeaks() > 100000000){
        Serial.println("Impact Detected by left load cell");
        impactDetected = true;
      }
    }
    if(rightLoadCell.isInitialized()){
      rightLoadCell.setCurrentTemp(temp.getData()[0]);
      rightLoadCell.update();
      if(rightLoadCell.getPeaks() > 100000000){
        Serial.println("Impact Detected by right load cell");
        impactDetected = true;
      }
    }

    xSemaphoreGive(mutex);
    // gives the ability for other tasks to run like bluetooth
    delay(2);
  }
  // in case the loop ever needs to exit, delete the task
  vTaskDelete(updateLoadCellTask);
}

void updateTemp(void * parameter){
  for(;;){
    xSemaphoreTake(mutex, portMAX_DELAY);
    temp.update();
    xSemaphoreGive(mutex);
    // We only need to get the temperature occasionally, so we can wait longer
    delay(10000);
  }
  vTaskDelete(updateTempTask);
}

void readSerial(void * parameter){
  uint8_t argLength = 0;
  int * args;
  for(;;){
    while(impactDetected || concussionDetected){
      delay(2500);
    }
    xSemaphoreTake(mutex, portMAX_DELAY);
    argLength = 0;
    serialMessage.update();
    bleSerialRead.update();
    if(serialMessage.isNewData()){
      argLength = serialMessage.getArgsLength();
      args = serialMessage.getArgs();
      serialMessage.clearNewData();
    }

    if(bleSerialRead.isNewData()){
      argLength = bleSerialRead.getArgsLength();
      args = bleSerialRead.getArgs();
      bleSerialRead.clearNewData();
    }

    // TODO: Fill out commands here
    if(argLength > 0){
      switch(args[0]){
        case 0:
          Serial.println("Do something");
          bleSerial.println("Do something");
          break;
        case 1:
          Serial.println("Do something else");
          bleSerial.println("Do something else");
          break;
        default:
          Serial.println("Invalid command");
          bleSerial.println("Invalid command");
          break;
      }
    }
    
    xSemaphoreGive(mutex);
  }
  vTaskDelete(readSerialTask);
}

void printXYZDataStream(DataStream<xyzData>* stream, String header){
  for(int i = 0; i < stream->size(); i++){
    Serial.print(header);
    Serial.print(",");
    Serial.print(stream->peek(i).x, 3);
    Serial.print(",");
    Serial.print(stream->peek(i).y, 3);
    Serial.print(",");
    Serial.print(stream->peek(i).z, 3);
    Serial.println(";");
    while(impactDetected || concussionDetected){
        xSemaphoreGive(mutex);
        delay(2000);
        xSemaphoreTake(mutex, portMAX_DELAY);
    }
  }

}

void printDoubleDataStream(DataStream<double>* stream, String header){
  for(int i = 0; i < stream->size(); i++){
    Serial.print(header);
    Serial.print(",");
    Serial.print(stream->peek(i), 3);
    Serial.println(";");
    while(impactDetected || concussionDetected){
        xSemaphoreGive(mutex);
        delay(2000);
        xSemaphoreTake(mutex, portMAX_DELAY);
    }
  }
}

void printData(void * parameter){
  xSemaphoreTake(mutex, portMAX_DELAY);
  DataStream<xyzData>* bodyIMUAccelStream = bodyIMU.getAccelStream();
  DataStream<xyzData>* bodyIMUGyroStream = bodyIMU.getGyroStream();
  DataStream<xyzData>* headIMUAccelStream = headIMU.getAccelStream();
  DataStream<xyzData>* headIMUGyroStream = headIMU.getGyroStream();
  DataStream<xyzData>* bodyAccelStream = bodyAccel.getDataStream();
  DataStream<xyzData>* headAccelStream = headAccel.getDataStream();
  DataStream<double>* leftLoadCellStream = leftLoadCell.getDataStream();
  DataStream<double>* rightLoadCellStream = rightLoadCell.getDataStream();
  xSemaphoreGive(mutex);

  for(;;){
    // pause this task while an impact is detected
    while(impactDetected || concussionDetected){
        xSemaphoreGive(mutex);
        delay(2000);
        xSemaphoreTake(mutex, portMAX_DELAY);
    }
    xSemaphoreTake(mutex, portMAX_DELAY);
    printDoubleDataStream(leftLoadCellStream, "!LeftCell");
    xSemaphoreGive(mutex);
    delay(100);
    xSemaphoreTake(mutex, portMAX_DELAY);
    printDoubleDataStream(rightLoadCellStream, "!RightCell");
    xSemaphoreGive(mutex);
    delay(100);
    xSemaphoreTake(mutex, portMAX_DELAY);
    printXYZDataStream(headIMUGyroStream, "!HeadIMUGyro");
    xSemaphoreGive(mutex);
    delay(100);
    xSemaphoreTake(mutex, portMAX_DELAY);
    printXYZDataStream(bodyAccelStream, "!BodyAccel");
    xSemaphoreGive(mutex);
    delay(100);
    xSemaphoreTake(mutex, portMAX_DELAY);
    printXYZDataStream(headAccelStream, "!HeadAccel");
    xSemaphoreGive(mutex);
    delay(100);
    xSemaphoreTake(mutex, portMAX_DELAY);
    printXYZDataStream(bodyIMUGyroStream, "!BodyIMUGyro");
    xSemaphoreGive(mutex);
    delay(100);
    xSemaphoreTake(mutex, portMAX_DELAY);
    printXYZDataStream(headIMUAccelStream, "!HeadIMUAccel");
    xSemaphoreGive(mutex);
    delay(100);
    xSemaphoreTake(mutex, portMAX_DELAY);
    printXYZDataStream(bodyIMUAccelStream, "!BodyIMUAccel");
    xSemaphoreGive(mutex);
    delay(100);
    xSemaphoreTake(mutex, portMAX_DELAY);
    Serial.print("!Temp,");
    Serial.print(temp.getData()[0], 3);
    Serial.println(";");
    xSemaphoreGive(mutex);
    // We only need to get the temperature occasionally, so we can wait longer
    delay(2300);
  }

  vTaskDelete(printDataTask);
}

void updateControlPanel(void * parameter){
  while(true){
    xSemaphoreTake(mutex, portMAX_DELAY);
    controlPanel.update();
    bool * buttonStates = controlPanel.getButtonStates();
    if(buttonStates[0]){
      Serial.println("Left button pressed");
      controlPanel.getButtonStates()[0] = false;
      digitalWrite(LEFT_LED_PIN, !digitalRead(LEFT_LED_PIN));
    }
    if(buttonStates[1]){
      Serial.println("Right button pressed");
      controlPanel.getButtonStates()[1] = false;
      digitalWrite(RIGHT_LED_PIN, !digitalRead(RIGHT_LED_PIN));
    }
    if(buttonStates[2]){
      Serial.println("Record button pressed");
      controlPanel.getButtonStates()[2] = false;
      impactDetected = true;
    }
    if(buttonStates[3]){
      Serial.println("Reset button pressed");
      controlPanel.getButtonStates()[3] = false;
      headAccel.resetPeaks();
      bodyAccel.resetPeaks();
      leftLoadCell.resetPeaks();
      rightLoadCell.resetPeaks();
      headIMU.resetPeaks();
      bodyIMU.resetPeaks();
    }

    xSemaphoreGive(mutex);
    delay(30);
  }
  vTaskDelete(NULL);
}

void showStartupErrors(void * parameter){
  uint8_t off[] = {0, 0, 0};
  uint8_t red[] = {255, 0, 0};
  uint8_t green[] = {0, 255, 0};
  uint8_t blue[] = {0, 0, 255};
  uint8_t yellow[] = {255, 255, 0};
  unsigned long timer = millis();

  while(!bootup_errors_shown){
      xSemaphoreTake(mutex, portMAX_DELAY);
      // if all flags are false, stop trying to show bootup errors
      if(startup_errors & 255 == 0) bootup_errors_shown = true;
      //if the body IMU is not initialized, flash red once on error1 led
      //if the body accel is not initialized, flash red once on error2 led
      //if the temperture sensor is not initialized, flash yellow on both error leds once
      // if the sd card is not found, flash blue on both error leds twice
      if((startup_errors & 0b00000001) == 0b00000001){
        if(millis() - timer < 2000){
          Serial.println("IMU Error");
          error1.set_animation(BLINK_ONCE, false);
          error2.set_animation(SOLID, false);
          error1.set_low_color(red);
          error2.set_low_color(off);
        }
        else{
          error1.set_animation(SOLID, false);
          error2.set_animation(SOLID, false);
          error1.set_low_color(green);
          error2.set_low_color(green);
          startup_errors = (startup_errors & 0b11111110);
          timer = millis();
        }
      }

      if((startup_errors & 0b00000011) == 0b00000010){
        if(millis() - timer < 2000){
          Serial.println("Accel Error");
          error1.set_animation(SOLID, false);
          error2.set_animation(BLINK_ONCE, false);
          error1.set_low_color(off);
          error2.set_low_color(red);
        }
        else{
          error1.set_animation(SOLID, false);
          error2.set_animation(SOLID, false);
          error1.set_low_color(green);
          error2.set_low_color(green);
          startup_errors = (startup_errors & 0b11111101);
          timer = millis();
        }
      }

      if((startup_errors & 0b00000111) == 0b00000100){
        if(millis() - timer < 2000){
          Serial.println("Temp Error");
          error1.set_animation(BLINK_ONCE, false);
          error2.set_animation(BLINK_ONCE, false);
          error1.set_low_color(yellow);
          error2.set_low_color(yellow);
        }
        else{
          error1.set_animation(SOLID, false);
          error2.set_animation(SOLID, false);
          error1.set_low_color(green);
          error2.set_low_color(green);
          startup_errors = (startup_errors & 0b11111011);
          timer = millis();
        }
      }

      if((startup_errors & 0b00001111) == 0b00001000){
        if(millis() - timer < 3000){
          error1.set_animation(BLINK_ONCE, false);
          error2.set_animation(BLINK_ONCE, false);
          error1.set_low_color(blue);
          error2.set_low_color(blue);
        }
        else{
          error1.set_animation(SOLID, false);
          error2.set_animation(SOLID, false);
          error1.set_low_color(green);
          error2.set_low_color(green);
          startup_errors = (startup_errors & 0b11110111);
          timer = millis();
        }
      }
      xSemaphoreGive(mutex);
      delay(25);
    }
    vTaskDelete(NULL);
}

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  bleSerialRead.init(115200);

  // initialize everything for the control panel
  uint8_t green[] = {0, 255, 0};
  uint8_t yellow[] = {255, 255, 0};
  uint8_t off[] = {0, 0, 0};
  error1.set_animation(SOLID, false);
  error2.set_animation(SOLID, false);
  error1.set_low_color(yellow);
  error2.set_low_color(yellow);
  controlPanel.init();
  pinMode(LEFT_LED_PIN, OUTPUT);
  pinMode(RIGHT_LED_PIN, OUTPUT);
  leds.update();
  

  Serial.println("Starting up...");
  // IMPORTANT: initialize the I2C bus before the devices or things will fail
  wire0.begin(SDA1_PIN, SCL1_PIN);
  wire1.begin(SDA2_PIN, SCL2_PIN);
  
  Serial.println("Initializing Temperature Sensor");
  startup_errors |= (!temp.init()) << 2;
  temp.update();
  
  Serial.println("Initializing Body IMU");
  startup_errors |= !bodyIMU.init();
  Serial.println("Initializing Head IMU");
  headIMU.init();
  
  Serial.println("Initializing Body Accelerometer");
  startup_errors = (!bodyAccel.init()) << 1;
  Serial.println("Initializing Head Accelerometer");
  headAccel.init();
  
  Serial.println("Initializing Load Cell");
  leftLoadCell.init();
  rightLoadCell.init();
  
  double leftCalibrationTemp[2] = {22, 22}; // TODO: get real calibration values
  double leftOutputValues[6] = {15300, 41100, 55200, 15300, 41100, 55200}; 
  double leftWeightValues[6] = {0, 22.7, 45.4, 0, 22.7, 45.4}; 
  leftLoadCell.calibrate(leftCalibrationTemp, leftOutputValues, leftWeightValues); 
  leftLoadCell.setCurrentTemp(temp.getData()[0]);
  leftLoadCell.setLocation(leftShoulder, 9);
  double rightCalibrationTemp[2] = {22, 22}; // TODO: get real calibration values
  double rightOutputValues[6] = {16350, 48800, 62850, 16350, 48800, 62850}; 
  double rightWeightValues[6] = {0, 22.7, 45.4, 0, 22.7, 45.4}; 
  rightLoadCell.calibrate(rightCalibrationTemp, rightOutputValues, rightWeightValues); 
  rightLoadCell.setCurrentTemp(temp.getData()[0]);
  rightLoadCell.setLocation(rightShoulder, 9);
  

  // initialize SD card
  Serial.println("Initializing SD Card");
  startup_errors |= (!sdCard.init()) << 3;
  
  // register all sensor data streams
  concussionStream.setHeader(concussionLabel, strlen(concussionLabel));
  sdCard.registerDoubleDatastream(leftLoadCell.getDataStream());
  sdCard.registerXYZDatastream(bodyIMU.getAccelStream());
  sdCard.registerXYZDatastream(bodyIMU.getGyroStream());
  sdCard.registerXYZDatastream(bodyAccel.getDataStream());
  sdCard.registerDoubleDatastream(&concussionStream);
  
  leftLoadCell.resetPeaks();
  rightLoadCell.resetPeaks();
  headAccel.resetPeaks();
  bodyAccel.resetPeaks();
  bodyIMU.resetPeaks();
  headIMU.resetPeaks();

  sdCard.setDynamicFilename(dynamicFilename, extension);
  Serial.println("Creating IMU task");
  // Create an IMU task to run on core 0
  // WARNING!! WiFi runs on core 0 and can crash the program if it doesn't get enough runtime. You need to use some yield() function to give it time to run
  xTaskCreatePinnedToCore(
    updateIMU, // task function
    "Get IMU Data", // task name
    10000, // stack size in words or bytes, i've seen conflicting info
    NULL, // parameters to give to the task function
    1, // priority. Keep all task priorities at 1
    &updateIMUTask, // task handle
    0 // the core it should run on
  );
  
  xTaskCreatePinnedToCore(
    updateLoadCells, // task function
    "Get IMU Data", // task name
    10000, // stack size in words or bytes, i've seen conflicting info
    NULL, // parameters to give to the task function
    1, // priority. Keep all task priorities at 1
    &updateLoadCellTask, // task handle
    0 // the core it should run on
  );

  Serial.println("Creating Temperature task");
  // create a task to get the temperature
  xTaskCreatePinnedToCore(
    updateTemp, // task function
    "Get the current temperature", // task name
    10000, // stack size in words or bytes, i've seen conflicting info
    NULL, // parameters to give to the task function
    1, // priority. Keep all task priorities at 1
    &updateTempTask, // task handle
    1 // the core it should run on
  );

  Serial.println("Creating Print task");
  // create a task to print the data
  xTaskCreatePinnedToCore(
    printData, // task function
    "Print the data", // task name
    10000, // stack size in words or bytes, i've seen conflicting info
    NULL, // parameters to give to the task function
    1, // priority. Keep all task priorities at 1
    &printDataTask, // task handle
    1 // the core it should run on
  );

  Serial.println("Creating Serial task");
  // create a task to read the serial port
  xTaskCreatePinnedToCore(
    readSerial, // task function
    "Read the serial port", // task name
    10000, // stack size in words or bytes, i've seen conflicting info
    NULL, // parameters to give to the task function
    1, // priority. Keep all task priorities at 1
    &readSerialTask, // task handle
    1 // the core it should run on
  );

  Serial.println("Creating SD Card Task");
  xTaskCreatePinnedToCore(
    updateSDCard, // task function
    "Update SD Card", // task name
    10000, // stack size in words or bytes, i've seen conflicting info
    NULL, // parameters to give to the task function
    1, // priority. Keep all task priorities at 1
    &updateSDCardTask, // task handle
    1 // the core it should run on
  );

  xTaskCreatePinnedToCore(
    updateControlPanel, // task function
    "Update the control panel", // task name
    10000, // stack size in words or bytes, i've seen conflicting info
    NULL, // parameters to give to the task function
    2, // priority. Keep all task priorities at 1
    &updateControlPanelTask, // task handle
    1 // the core it should run on
  );

  // show any error codes.
  xTaskCreatePinnedToCore(showStartupErrors, "", 10000, NULL, 1, &showStartupErrorsTask, 0); // start task to show startup errors
  error1.set_low_color(green); // set error 1 back to green
  error2.set_low_color(green); // set error 2 back to green
  leds.update();
  Serial.println("Finished setup");
  
}

// task to run on core 1 to handle communication
void loop() {
  // put your main code here, to run repeatedly:
  vTaskDelete(NULL);

}