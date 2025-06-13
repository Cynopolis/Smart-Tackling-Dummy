/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "SDCard.h"
#include <SPI.h>
#include <SD.h>

SDCard::SDCard(uint8_t CS_PIN) :
CS_PIN(CS_PIN){}

bool SDCard::initReader(){
    if(!SD.begin(this->CS_PIN)){
        Serial.println("SD Card failed to initialize.");
        this->readerInitialized = false;
        return false;
    }
    Serial.println("SD Card succesfully initialized.");
    this->readerInitialized = true;
    return true;
}

bool SDCard::init(uint8_t CS_PIN){
    this->CS_PIN = CS_PIN;

    for(int i = 0; i < 10; i++){
        if(initReader()){
            return true;
        }
        else{
            Serial.print("Attempt (");
            Serial.print(i+1);
            Serial.println("/10)\nTrying again...");
            delay(10);
        }
    }
   return false;
}

bool SDCard::init(){
    return init(CS_PIN);
}

void SDCard::openFile(bool newFile, const char * mode){
    // check to see if the dynamic file name is set
    if(this->dynamicFilename == nullptr){
        Serial.println("Error opening file: Dynamic file name is not set.");
        this->fileOpen = false;
        return;
    }

    // check to see if the file is already open
    if(this->fileOpen){
        Serial.println("Error opening file: A file is already open.");
        return;
    }

    // create a temporary file name
    char tempFilename[50];
    // store the current file number in a temporary variable
    uint32_t tempFileNumber = this->fileNumber;
    strcpy(tempFilename, this->dynamicFilename);
    strcat(tempFilename, "_");
    char tempFileNumberStr[10];
    snprintf(tempFileNumberStr, 10, "%d", this->fileNumber);
    strcat(tempFilename, tempFileNumberStr);
    strcat(tempFilename, extension);

    // check to see if the file already exists
    while(SD.exists(tempFilename) && *mode == *FILE_WRITE){
        // if the file already exists, increment the file number
        tempFileNumber++;
        // create a new temporary file name
        strcpy(tempFilename, this->dynamicFilename);
        strcat(tempFilename, "_");
        char tempFileNumberStr[10];
        snprintf(tempFileNumberStr, 10, "%d", tempFileNumber);
        strcat(tempFilename, tempFileNumberStr);
        strcat(tempFilename, extension);
    }

    strcpy(this->filename, tempFilename);

    // open the file
    this->file = SD.open(this->filename, FILE_APPEND);
    // check to see if the file was opened successfully
    if(!this->file){
        Serial.print("Error opening file: ");
        Serial.println(this->filename);
        this->fileOpen = false;
        return;
    }
    // set the file number to the temporary file number
    this->fileNumber = tempFileNumber;
    // set the file open flag to true
    this->fileOpen = true;
}

void SDCard::closeFile(){
    this->file.close();
    this->file = File();
    fileOpen = false;
}

bool SDCard::write(const char* line){
    if(!this->readerInitialized){
        Serial.println("Error writing to file: SD Card reader is not initialized.");
        this->fileInitialized = false;
        return false;
    }
    if(!this->fileOpen){
        Serial.println("Error writing to file: No file is open.");
        this->fileInitialized = false;
        return false;
    }
    if(this->fileOpen && !this->file){
        Serial.println("Error writing to file: File is not available for write.");
        return false;
    }
    
    file.print(line);
    return true;
}

bool SDCard::write(String line){
    // convert the string to a char array
    char lineArray[line.length() + 1];
    line.toCharArray(lineArray, line.length() + 1);
    // write the line
    return write(lineArray);
}

bool SDCard::write(int line){
    // convert the int to a char array
    char lineArray[10];
    snprintf(lineArray, 10, "%d", line);
    // write the line
    return write(lineArray);
}

bool SDCard::write(float line){
    // convert the float to a char array
    char lineArray[10];
    snprintf(lineArray, 10, "%f", line);
    // write the line
    return write(lineArray);
}

bool SDCard::write(double line){
    // convert the float to a char array
    char lineArray[10];
    snprintf(lineArray, 10, "%f", line);
    // write the line
    return write(lineArray);
}

bool SDCard::writeln(const char* line){
    // tack on \r\n to the end of the line
    char lineArray[strlen(line) + 3];
    strcpy(lineArray, line);
    strcat(lineArray, "\r\n");
    // write the line
    return write(lineArray);    
}

bool SDCard::writeln(String line){
    // convert the string to a char array
    char lineArray[line.length() + 1];
    line.toCharArray(lineArray, line.length() + 1);
    // write the line
    return writeln(lineArray);
}

bool SDCard::writeln(int line){
    // convert the int to a char array
    char lineArray[10];
    snprintf(lineArray, 10, "%d", line);
    // write the line
    return writeln(lineArray);
}

bool SDCard::writeln(float line){
    // convert the float to a char array
    char lineArray[10];
    snprintf(lineArray, 10, "%f", line);
    // write the line
    return writeln(lineArray);
}

bool SDCard::writeln(double line){
    // convert the float to a char array
    char lineArray[10];
    snprintf(lineArray, 10, "%f", line);
    // write the line
    return writeln(lineArray);
}

void SDCard::registerDoubleDatastream(DataStream<double>* stream){
    doubleStreams[registeredDoubleStreams] = stream;
    registeredDoubleStreams++;
}

void SDCard::registerXYZDatastream(DataStream<xyzData>* stream){
    XYZStreams[registeredXYZStreams] = stream;
    registeredXYZStreams++;
}

void SDCard::update(bool writeDestructive){
    /** Write Data in this format:
     * Header_1, Header_2, Header_3, ..., Header_n
     * Data_1, Data_2, Data_3, ..., Data_n
     * Data_1, Data_2, Data_3, ..., Data_n
     */    

    // if the file is not open, print an error message and try to open it.
    if(!fileOpen){
        Serial.println("Error updating file: File is not open.");
        Serial.println("Attempting to reopen SD card for next write...");
        this->fileInitialized = false;
        closeFile();
        this->file = File();
        openFile(true, FILE_WRITE);
        return;
    }
    // if the file has been initialized, close it and reopen it for appending
    if(fileInitialized){
        closeFile();
        openFile(false, FILE_APPEND);
    }
    // otherwise, write the header
    else{
        closeFile();
        openFile(true, FILE_WRITE);
        writeHeader();
        fileInitialized = true;
    }

    // write the data
    // figure out which data stream has the least of ammount of data
    uint16_t minSize = 1001;
    for(auto i = 0; i < registeredDoubleStreams; i++){
        if(doubleStreams[i]->size() < minSize){
            minSize = doubleStreams[i]->size();
        }
    }
    for(auto i = 0; i < registeredXYZStreams; i++){
        if(XYZStreams[i]->size() < minSize){
            minSize = XYZStreams[i]->size();
        }
    }

    // write the data
    if(writeDestructive){
        writeDataDestructive(minSize);
    }
    else{
        writeData(minSize);
    }
    file.close();
}

void SDCard::writeHeader(){
    // write the double headers
    
    for(int i = 0; i < registeredDoubleStreams; i++){
        this->write(doubleStreams[i]->getHeader());
        if(i < registeredDoubleStreams - 1){
            this->write(",");
        }
    }

    if(registeredDoubleStreams > 0 && registeredXYZStreams > 0){
        this->write(",");
    }

    //write the XYZ headers
    for(int i = 0; i < registeredXYZStreams; i++){
        char * header = XYZStreams[i]->getHeader();
        this->write(header);
        this->write(":X,");
        this->write(header);
        this->write(":Y,");
        this->write(header);
        this->write(":Z,");
        this->write(header);
        this->write(":Magnitude");
        if(i < registeredXYZStreams - 1){
            this->write(",");
        }
    }
    this->writeln("");
}

void SDCard::writeData(uint16_t numLines){
    // for each row in numLines
    for(uint16_t i = 0; i < numLines; i++){
        // write that row of data
        // write the double data
        for(auto j = 0; j < registeredDoubleStreams; j++){
            this->write(doubleStreams[j]->peek(i));
            if(j < registeredDoubleStreams - 1){
                this->write(",");
            }
        }

        // if there is both double and XYZ data, write a comma
        if(registeredDoubleStreams > 0 && registeredXYZStreams > 0){
            this->write(",");
        }
        // write the XYZ data
        for(auto j = 0; j < registeredXYZStreams; j++){
            xyzData data = XYZStreams[j]->peek(i);
            this->write(data.x);
            this->write(",");
            this->write(data.y);
            this->write(",");
            this->write(data.z);
            this->write(",");
            this->write(data.magnitude());
            if(j < registeredXYZStreams - 1){
                this->write(",");
            }
            
        }
        this->writeln("");
    }
}

void SDCard::writeDataDestructive(uint16_t numLines){
    
    // for each row in numLines
    for(auto i = 0; i < numLines; i++){
        // write that row of data
        // write the double data
        for(auto j = 0; j < registeredDoubleStreams; j++){
            this->write(doubleStreams[j]->pop());
            if(j < registeredDoubleStreams - 1){
            this->write(",");
            }
            
        }

        if(registeredDoubleStreams > 0 && registeredXYZStreams > 0){
            this->write(",");
        }

        // write the XYZ data
        for(auto j = 0; j < registeredXYZStreams; j++){
            xyzData data = XYZStreams[j]->pop();
            this->write(data.x);
            this->write(",");
            this->write(data.y);
            this->write(",");
            this->write(data.z);
            this->write(",");
            this->write(data.magnitude());
            if(j < registeredXYZStreams - 1){
                this->write(",");
            }
            
        }
        this->writeln("");
    }
}

void SDCard::setDynamicFilename(char * dynamicFileName, char * extension){
    this->dynamicFilename = dynamicFileName;
    this->extension = extension;
}

uint32_t SDCard::getFileNumber(){
    return fileNumber;
}

void SDCard::setFileNumber(uint32_t fileNumber){
    if(fileNumber != this->fileNumber) this->fileInitialized = false;
    this->fileNumber = fileNumber;
}