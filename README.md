# Contact-Dummy
Firmware for contact dummy used in senior design

## Why Upload This
I recently found a copy of this code saved to one of my devices and I was pretty proud of this work when I wrote it in college. I wanted to upload it to show it off to anyone who might be interested as an example of the work I was able to do in 2023.

## A tour of the code
The goal of the smart tackling dummy was to allow hockey coaches to review checking plays and see how the checked dummy reacted to different types of hits. The goal was to minimize concussions and other injuries that could occur from a player being hit in the head or neck area.

### Capabilities
- 1 accelerometer in the head and one in the chest
- 1 gyroscope in the head and one in the chest
- The device would constantly be saving a small buffer of data in memory so that when it detected a hit, it could save sensor data leading up to the hit to the SD card. This allowed for analysis later on by coaches or researchers.
- An IR remote to control some basic functions on the dummy including turning recording on/off
- An SD card where the device would ave data to a CSV file
- Load cells in the shoulders that could measure the force of a hit
- Temperature sensors to keep the load cell calibration accurate for wide temperature ranges. This was particularly important because the dummy is going to be used in a hockey rink.
- The ability to calculate the probability of a concussion based on sensor readings
- Control of individually addressable LEDs which could indicate concussion severeity and other important data.
- Serial communication over simple protocol
- Bluetooth serial communication for remote control. (This was never tested because there wasn't time during development.)
- A control panel interface with a few buttons to be able to control the dummy physically.