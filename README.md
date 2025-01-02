# Light-Saver - Energy saving automatic street lamp hardware/software
**Note:** Note: This repository is a clone of the original repository, with all hardware-related files (e.g., Makefiles, MPLab project files) removed along with the commit history.

## Contents
1. Learning Outcomes
2. Brief
3. Specification
4. Hardware Setup
5. Programme Explanation   
6. Programme Framework  
6.1 main.c  
6.2 interrupts.c  
6.3 ADC.c  
6.4 timers.c  
6.5 LEDarray.c  
6.6 clock.c  
6.7 LCD.c  
6.8 calibrate.c  
7. Demonstation
8. Supplementary information and help
   
## 1. Objectives

The principal objectives for this project are:

- Implement a working microprocessor based system to achieve a more complex real world task
- Develop my ability to independently plan, organise and structure my code 
- Improve my grasp of the C language and writing my own functions
- Continue to improve my collaborative coding skills with Git


## 2. Brief

My task is to design a microcontroller-based system that automates the switching of outdoor street lamps, turning them off during low-traffic hours (e.g., 1am-5am) to save energy and costs, based on ambient light levels. The system must also stay in time continously without human interaction/calibration apart from initial setup.

## 3. Specification

1. Monitors light level with the LDR and turns on an LED in low light conditions (i.e. night-time) and off in bright conditions (i.e. daytime)
2. Displays the current hour of day on the LED array in binary
3. Turns the light off between approx. 1am and 5am
4. Adjusts for daylight savings time
5. Maintain synchronicity with the sun indefinitely
6. Be fully automatic (requires zero maintenance after installation)

## 4. Program Explanation

### 4.1 Monitors light level with the LDR and turns on an LED in low light conditions (i.e. night-time) and off in bright conditions (i.e. daytime)
This objective is achieved by:
- Reading the LDR value using the **ADC.c** script and turning the LED array (rather than an individual LED to allow for the LEDs to show additional information) on/off according to a set of rules (**LED_rules**)
- To indicate that the light level is above the threshold (indicating day), LED RH3 turns on, and vice versa
- To indicate a sunrise or sunset event (represented by the first time light crosses the threshold along with some other rules), LED RD7 is flashed

### 4.2 Displays the current hour of day on the LED array in binary
This objective was changed to display the time as hrs : mins : secs and the date as day / month / year on the LCD screen by:
- Sending the time and date as a string to the LCD using the **LCD_sendstring()** function to display the time on the top line and the date on the bottom line
- Using a char array to convert between the day of the week in numbers and in text for better legibility

### 4.3 Turns the light off between approx. 1am and 5am
This objective is achieved through:
- A top level if statement which read the time from the clock structure and if the hours were between 1am and 5am (and 10am and 3pm) all the LED array lights were turned off

### 4.4 Adjusts for daylight savings time
This objective, and other time-keeping rules, are achieved by:
- Defining a clock struct in clock.h with fields including hrs, mins, secs, days, monds, yrs, dotw (day of the week)
- Creating an instance of the struct (datetime) in main representing the initialision time of the system
- Directly modifying datetime by using pointers to pass it to **clock_update** which carries out all time keeping as explained later
- ![Day length](gifs/day-length-london.jpg)Daylight savings graph

### 4.5 Maintain synchronicity with the sun indefinitely
This objective is achieved by:
- Using pointers to pass the datetime struct to **calibrate_clock**, which handles keeping the time in sync with the sun using the solar midday 

### 4.6 Be fully automatic (requires zero maintenance after installation)
This objective is achieved by:
- Automating the entire process, managing tasks like light monitoring, LED control, daylight-saving adjustments, and sun synchronisation internally
- Only requires initial inputs of the setup datetime, arrays of average dawn/dusk times monthly (separated into hrs and mins), and empty structs of parameters passed to functions for better memory efficiency compared to static/global variables
- Handles edge cases including short busts of bright light at night/darkness in the day to ensure sunrise/sunset events aren't incorrectly recorded that would cause errors in calibration

## 5. Program Framework

### 5.1 main.c
This script calls and initialises all the functions from the different modules and exectutes the main code in a continuous loop to constantly update/calibrate the time, turn on/off the LED array and individual LEDs accordingly, and send current datetime information to be displayed on the LCD 

### 5.2 interrupts.c  
This script initialises the interrupts used in the clock update function by incrementing the count volatile variable every second (calculated with the timer prescaler and initial values from Lab 3 where each timer overflow is very close to a second) 

### 5.3 ADC.c  
This script initialises the ADC module and sets it up to receive analog data from a specific pin i.e. pin RA3, allowing for LDR values to be recorded. This script also contains **LED_rules**, which define the rules that control the lighting of the LED array, which contains parameters passed through a pointer to a struct from main, removing the need for static/global variables. The main rules are as follows:
- Only enter the logic if the LDR value is changing gradually, accounting for edge cases including unexpected dark/light
- Always keep LED array off between 1am-5am and 10am-3pm for energy saving and as no sunrise/sunsets occur in these time periods
- Turn the LED array on if the light is below the threshold (indicating night) and the current time is either less than the monthly average sunrise time (if before midday) or greater than the monthly average sunset time (if after midday) with time buffers, which also indicates night
- Return 0 normally
- Return 1 for the first instance of the light going from below the threshold to above when the time is near the estimated sunrise, indicating a sunrise event (sunrise_check is updated to ensure only the first instance of light crossing the threshold is recorded as sunrise)
- Return 2 for the first instance of the light going from above the threshold to below when the time is near the estimated sunset, indicating a sunset event (sunset_check is updated to ensure only the first instance of light crossing the threshold is recorded as sunset)

### 5.4 timers.c  
This script initialises Timer0 with a pre and post scaler to overflow every second and has a function to read the 16 bit timer value from Timer0

### 5.5 LEDarray.c  
This script initialises the pins to drive the LED array and contains **LEDarray_disp_bin** which is used to turn the LED array on/off

### 5.6 clock.c  
This script contains **clock_update** which handles all timekeeping rules for the Clock struct (datetime in main) as follows:
- Updates the secs field according to the count variable from the interrupt (can also update the hrs field instead for testing
- Handles overflows of all timing fields (e.g. secs going above 59 resets to 0 and adds 1 to mins)
- Handles month length logic by checking if a month is either in an array of 31 day months, 30 day months, or neither (indicating February)
- Handles the day of the week by wrapping round the intial day of the week once it reaches 6 (Sunday) using the remainder operator
- Handles leap year logic by checking the leap year rule (if the year is either divisible by 400, or divisible by 4 and not divisible by 100) and adding a day to February if passed
- Handles daylight savings time by checking if it is Sunday (day 6) in Mar/Oct, and if adding 7 days to the date would not take it to the next month (indicating last Sunday of the month), and either adding or subtracting an hour if Mar/Oct respectively, ensuring a flag is set to not infinitely loop at the Oct daylight savings

### 5.7 LCD.c  
This script initialises the LCD screen to display the date and time 

### 5.8 calibrate.c
This script enables completion of task 5 by recalibrating the clock using an input of average sunrise and sunset times for each month. The **calibrate_clock** function also receives parameters by pointers to a struct, and performs calibration as follows:
- Checks if a sunrise/sunset event has occured, and appends the current time in mins to an array, incrementing an index to ensure the next time will be added to the next element in the array
- Every 5th sunrise, find the average sunrise/sunset time, add them and divide by 2 to find the solar midday in minutes from midnight
- Calculate the ideal solar midday from the monthly average sunrise/sunset time and find the difference between the 2 middays
- Use this difference to update the Clock struct using pointers, accounting for possible negative differences

## 6. Demonstration

### Sunrise/Sunset Events : [Video](https://youtube.com/shorts/L6aLjjsyCKE?feature=share)

### Auto Calibration: [Video](https://youtube.com/shorts/iT7LTpw1pjk?feature=share)

### Daylight Savings Time: [Video](https://youtube.com/shorts/25odGjUFU5s?feature=share)

### DOTW/Secs Overflow: [Video](https://youtube.com/shorts/q37tM_xhz-4?feature=share)

### Leap Year: [Video](https://youtube.com/shorts/EbAhDTGBesc?feature=share)


