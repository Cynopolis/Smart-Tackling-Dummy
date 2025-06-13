/**
 * @author: Quinn Henthorne henth013@d.umn.edu 7636568391
*/

#define SPI_MOSI_PIN 23 //Used for SD card communication
#define SCL1_PIN 22 //I2C Bus for main board I2c components
#define IR_PIN 32 //Connected to IR reciever to get data from IR remote
#define LEFT_BUTTON_PIN 35 //Toggles the left shoulder led on and off
#define SDA1_PIN 21 //I2C Bus for main board I2c components
#define SPI_MISO_PIN 19 //Used for SD card communication
#define SPI_CLK_PIN 18 //Used for SD card communication
#define SPI_CS_PIN 5 //Used for SD card communication
#define SDA2_PIN 17 //I2C Bus for external I2C Components
#define SCL2_PIN 16 //I2C Bus for external I2C Components
#define TOGGLE_BUTTON_PIN 4 //Toggles whether the data is being recorded to an SD card
#define RIGHT_LED_PIN 0 //Controls the right hsoulder LED
#define LEFT_LED_PIN 2 //Controls the left shoulder led
#define LED_STRIP_PIN 15 //Controls the individually addressable led strip

#define BATTERY_PIN 36 //Reads the voltage of the battery. This should be connected to a voltage divider and can only tolerate 3.3v
#define RESET_BUTTON_PIN 39 //Resets the current data recording
#define RIGHT_BUTTON_PIN 34 //Toggles the right hsoulder led



#define LOAD_CELL3_CLK_PIN 25 //Clock Pin for third load cell
#define LOAD_CELL3_DAT_PIN 26 //Data pin for third load cell
#define LOAD_CELL2_CLK_PIN 27 //Clock Pin for second load cell
#define LOAD_CELL2_DAT_PIN 14 //Data pin for second load cell
#define LOAD_CELL1_CLK_PIN 12 //Clock Pin for first load cell
#define LOAD_CELL1_DAT_PIN 13 //Data pin for first load cell
