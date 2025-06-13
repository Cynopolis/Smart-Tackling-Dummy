/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once
#include "DataStream.h"
#include <HX711.h>

class LoadCell{
    public:
        
        /**
         * @brief Construct a new Load Cell object
         * @param loadCellPin The pin number that the load cell is connected to
         */
        LoadCell(uint8_t dataPin, uint8_t clockPin);

        /**
         * @brief Initialize the load cell
         */
        bool init();

        /**
         * @brief Get the load cell value
         * @param calirationTemps An array of 2 temperature values in celcius that the two sets of calibration values were taken at. (First value goes with the first set and second value goes with the second set)
         * @param outputValues 2 sets of 3 Load cell readings. Each set of three readings should be taken at different temperatures
         * @param weightValues The weight applied to the load cell corresponding to each of the calibration values
         * @return float The load cell value
         */
        void calibrate(double* calibration_temp, double *outputValues, double* weightValues);

        /**
         * @brief Read new load cell data
         */
        void update();

        /**
         * @brief Get the load cell value
         * @return float The load cell value
         */
        double getData();

        /**
         * @brief Set the current temperature
         * @param temp The current temperature
         */
        void setCurrentTemp(double temp);

        /**
         * @brief Get the load cell data stream
         * @return * DataStream<double> A pointer to the load cell data stream
         */
        DataStream<double> *getDataStream();

        /**
         * @brief set the load cell location
         * @param location the location of the load cell
         * @param length the length of the location  char array
         */
        void setLocation(char* location, unsigned int length);

        /**
         * @brief Get the load cell location
         * @return char* The location of the load cell
         */
        char* getLocation();

        /**
         * @brief Get the load cell location length
         * @return unsigned int The length of the location char array
         */
        unsigned int getLocationLength();

        /**
         * @brief Get whether the load cell is initialized
         * @return true The load cell is initialized
        */
        bool isInitialized(){return this->initialized;};

        /**
         * @brief Get the load cell peaks
         * @return double The load cell peaks
        */
        double getPeaks();

        /**
         * @brief Reset the load cell peaks
         * @return None
        */
        void resetPeaks();
        
    
    private:
        uint8_t clockPin;
        uint8_t dataPin;
        HX711 loadCell;

        // calibration curve coefficients for a quadratic fit
        double a, b, c;

        /**
         * @brief Given a load cell reading, return the actual weight
         * @return double The actual weight
         */
        double getWeight();

        double lastReading = 0;
        double tempFactor = 0;
        double currentTemp = 0;
        double calibrationTemp = 0;
        double peakImpact = 0;

        bool initialized = false; // Whether or not the load cell has been initialized

        // Holds the last 100 readings
        DataStream<double> dataStream = DataStream<double>();
};