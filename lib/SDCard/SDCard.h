/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/


#pragma once
#include "DataStream.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "sensorTemplate.h"

#define SPI_SPEED SD_SCK_MHZ(4)

class SDCard{
    public:
        /**
         * @brief Construct a new SDCard object
         * @param CS_PIN the pin to use for the SPI CS pin
        */
        SDCard(uint8_t CS_PIN);
        ~SDCard() = default;

        /**
         * @brief Initialize the SD Card
         * @param SPI_CS_PIN the pin to use for the SPI CS pin
         */
        bool init(uint8_t CS_PIN);

        /**
         * @brief Initialize the SD Card
        */
        bool init();

        /**
         * @brief Open a file on the SD Card
         * @param filename the name of the file to open
         */
        void openFile(bool newFile, const char* mode);

        /**
         * @brief Close the currently open file
         */
        void closeFile();
        
        // Add the ability to give a data stream pointer to the SDCard class and have it automatically write its contents to the open file
        //void registerDataStream(DataStream stream);

        bool writeln(const char* line);
        bool writeln(String line);
        bool writeln(int line);
        bool writeln(float line);
        bool writeln(double line);

        bool write(const char* line);
        bool write(String line);
        bool write(int line);
        bool write(float line);
        bool write(double line);

        /**
         * @brief Add a data stream to the SDCard
         * @param stream a pointer to a datastream which stores type double.
         */
        void registerDoubleDatastream(DataStream<double>* stream);

        /**
         * @brief Add a data stream to the SDCard
         * @param stream a pointer to ta datastream which stores type xyzData.
        */
       void registerXYZDatastream(DataStream<xyzData> * stream);

        /**
         * @brief Write any new data from the data streams to the file
         * @param writeDestructive If true, when getting data from the data streams, the data will be cleared from the data stream
         * @post If writeDestructive is true, the data streams will be cleared of any data that was written to the file
         */
        void update(bool writeDestructive = false);

        /**
         * @brief set the dynamic file name and extension
         * @param dynamicFilename the dynamic file name
         * @param extension the file extension
         */
        void setDynamicFilename(char * dynamicFilename, char * extension);

        /**
         * @brief get the file number
         * @return uint32_t the file number
         */
        uint32_t getFileNumber();

        /**
         * @brief set the file number
         * @param fileNumber the file number
         */
        void setFileNumber(uint32_t fileNumber);

        /**
         * @brief return true if an SD Card is connected
         * @return true if an SD Card is connected
        */
        bool isSDCardConnected(){return readerInitialized;}

        /**
         * @brief return true if a file is open
         * @return true if a file is open
        */
        bool isFileOpen(){return fileOpen;};

        /**
         * @brief return true if the file has been initialized
         * @return true if the file has been initialized
        */
        bool isFileInitialized(){return fileInitialized;};


    private:
        // keep track of if the file is open or not
        bool readerInitialized = false; // true when controller cna successfully connect to the SD Card reader
        bool fileOpen = false; // true when a file is open
        bool fileInitialized = false; // true when a file has been opened the header has been written to it
        File file;// = File();
        uint8_t CS_PIN;

        // linked list of pointers to data streams
        DataStream<double>* doubleStreams[10] = {nullptr};
        DataStream<xyzData>* XYZStreams[10] = {nullptr};
        uint8_t registeredDoubleStreams = 0; // keep track of how many data streams have been registered in the array
        uint8_t registeredXYZStreams = 0;
        // configure these for dynamic filename generation
        char * dynamicFilename = nullptr;
        char * extension = nullptr;
        char filename[50];
        uint32_t fileNumber = 0;


        /**
         * @brief Write the header to the file
        */
        void writeHeader();

        /**
         * @brief Write the data from the data streams to the file
         * @param numLines the number of lines to write to the file
         */
        void writeData(uint16_t numLines);

        /**
         * @brief Write the data from the data streams to the file and clear the data from the data streams
         * @param numLines the number of lines to write to the file
         */
        void writeDataDestructive(uint16_t numLines);

        /**
         * @brief attempt to initialize the SD Card reader
        */
        bool initReader();
};