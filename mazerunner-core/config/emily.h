#ifndef EMILY_H
#define EMILY_H

const int SENSOR_COUNT = 4;
const float MAX_MOTOR_VOLTS = 6.0;

//***************************************************************************//
// set this to zero to disable profile data logging over serial
#define DEBUG_LOGGING 1
// time between logged lined when reporting is enabled (milliseconds)
const int REPORTING_INTERVAL = 10;


const int EEPROM_ADDR_SETTINGS = 0x0000;
#define SETTINGS_SIGNATURE 0xF1F0C00F

#define WHEEL_DIAMETER 32.0
#define ENCODER_PULSES 12.0
#define GEAR_RATIO 20.0
#define MOUSE_RADIUS 37.0

// forward motion uses the sum of the two encoders for odometry hence the 2.0 constant
#define DEFAULTS_MM_PER_COUNT (PI * WHEEL_DIAMETER / (2.0 * ENCODER_PULSES * GEAR_RATIO))

// rotation uses the difference between the encoders
#define DEFAULTS_DEG_PER_COUNT ((360.0 * DEFAULTS_MM_PER_COUNT) / (2.0 * PI * MOUSE_RADIUS))

#define DEFAULTS_FWD_KP 1.0
#define DEFAULTS_FWD_KD 5.0

#define DEFAULTS_ROT_KP 0.35
#define DEFAULTS_ROT_KD 4.0

// controller constants for the line follower configuration
#define DEFAULTS_WALL_KP 1.5
#define DEFAULTS_WALL_KD 6.0

// time delay for sensors to respond to emitters
#define DEFAULTS_EMITTER_ON_TIME 50


//*** MOTION CONTROL CONSTANTS **********************************************//

// forward motion controller constants
const float FWD_KP = 2.0;
const float FWD_KD = 1.1;

// rotation motion controller constants
const float ROT_KP = 2.1;
const float ROT_KD = 1.2;

// controller constants for the steering controller
const float STEERING_KP = 0.25;
const float STEERING_KD = 0.00;
const float STEERING_ADJUST_LIMIT = 10.0; // deg/s




// encoder polarity is either 1 or -1 and is used to account for reversal of the encoder phases
#define ENCODER_LEFT_POLARITY (-1)
#define ENCODER_RIGHT_POLARITY (1)

// similarly, the motors may be wired with different polarity and that is defined here so that
// setting a positive voltage always moves the robot forwards
#define MOTOR_LEFT_POLARITY (-1)
#define MOTOR_RIGHT_POLARITY (1)


//***************************************************************************//

//***** PERFORMANCE CONSTANTS************************************************//
// search and run speeds in mm/s and mm
const int DEFAULT_TURN_SPEED = 300;
const int DEFAULT_SEARCH_SPEED = 400;
const int DEFAULT_MAX_SPEED = 800;
const int DEFAULT_SEARCH_ACCEL = 2000;
//***************************************************************************//



//***** SENSOR CALIBRATION **************************************************//
// wall sensor thresholds and constants
// if you have the basic sensor board enter the same value for both front constants
#if EVENT == HOME
    // RAW values for the front sensor when the robot is backed up to a wall
    const int FRONT_LEFT_CALIBRATION = 97;
    const int FRONT_RIGHT_CALIBRATION = 48;
    // RAW values for the side sensors when the robot is centred in a cell
    // and there is no wall ahead
    const int LEFT_CALIBRATION = 87;
    const int RIGHT_CALIBRATION = 80;
    // SS90E turn thresholds. This is the front sum reading to trigger a turn
    // it changes a bit if there is an adjacent wall. The threshold is set for
    // when the robot is 20mm past the threshold.
    const int TURN_THRESHOLD_SS90E = 115;
    const int EXTRA_WALL_ADJUST = 6;

#elif EVENT == UK

    // RAW values for the front sensor when the robot is backed up to a wall
    const int FRONT_LEFT_CALIBRATION = 83;
    const int FRONT_RIGHT_CALIBRATION = 39;
    // RAW side sensor values when robot is centred in a cell and wall ahead
    const int LEFT_CALIBRATION = 80;
    const int RIGHT_CALIBRATION = 72;

    // SS90E turn thresholds. This is the front sum reading to trigger a turn
    // it changes a bit if there is an adjacent wall. The threshold is set for
    // when the robot is 20mm past the threshold.
    const int TURN_THRESHOLD_SS90E = 100;
    const int EXTRA_WALL_ADJUST = 6;

#elif EVENT == PORTUGAL
    // wall sensor thresholds and constants
    // RAW values for the front sensor when the robot is backed up to a wall
    const int FRONT_LEFT_CALIBRATION = 97;
    const int FRONT_RIGHT_CALIBRATION = 48;
    // RAW values for the side sensors when the robot is centred in a cell
    // and there is no wall ahead
    const int LEFT_CALIBRATION = 87;
    const int RIGHT_CALIBRATION = 80;
    // SS90E turn thresholds. This is the front sum reading to trigger a turn
    // it changes a bit if there is an adjacent wall. The threshold is set for
    // when the robot is 20mm past the threshold.
    const int TURN_THRESHOLD_SS90E = 115;
    const int EXTRA_WALL_ADJUST = 6;

#endif


//***** SENSOR SCALING ******************************************************//
// This is the normalised value seen by the front sensor when the mouse is
// in its calibration position
const int SIDE_NOMINAL = 100;
const int FRONT_NOMINAL = 100;

// Sensor brightness adjustment factor. The compiler calculates these so it saves processor time
const float FRONT_LEFT_SCALE = (float)FRONT_NOMINAL / FRONT_LEFT_CALIBRATION;
const float FRONT_RIGHT_SCALE = (float)FRONT_NOMINAL / FRONT_RIGHT_CALIBRATION;
const float LEFT_SCALE = (float)SIDE_NOMINAL / LEFT_CALIBRATION;
const float RIGHT_SCALE = (float)SIDE_NOMINAL / RIGHT_CALIBRATION;

// the values above which, a wall is seen
const int LEFT_THRESHOLD = 40;   // minimum value to register a wall
const int FRONT_THRESHOLD = 20;  // minimum value to register a wall
const int RIGHT_THRESHOLD = 40;  // minimum value to register a wall
const int FRONT_REFERENCE = 850; // reading when mouse centered with wall ahead

const float left_edge_pos = 90.0f;
const float right_edge_pos = 93.0f;

//***************************************************************************//
//***************************************************************************//
// Some physical constants that are likely to be board -specific

// with robot against back wall, how much travel is there to the cell center?
const int BACK_WALL_TO_CENTER = 48;



// The robot is likely to have wheels of different diameters and that must be
// compensated for if the robot is to reliably drive in a straight line
const float ROTATION_BIAS = 0.0025; // Negative makes robot curve to left


//***************************************************************************//
// Battery resistor bridge //Derek Hall//
// The battery measurement is performed by first reducing the battery voltage
// with a potential divider formed by two resistors. Here they are named R1 and R2
// though that may not be their designation on the schematics.
//
// Resistor R1 is the high-side resistor and connects to the battery supply
// Resistor R2 is the low-side resistor and connects to ground
// Battery voltage is measured at the junction of these resistors
// The ADC port used for the conversion will have a full scale reading (FSR) that
// depends on the device being used. Typically that will be 1023 for a 10-bit ADC as
// found on an Arduino but it may be 4095 if you have a 12-bit ADC.
// Finally, the ADC converter on your processor will have a reference voltage. On
// the Arduinos for example, this is 5 Volts. Thus, a full scale reading of
// 1023 would represent 5 Volts, 511 would be 2.5Volts and so on.
//
// in this section you can enter the appropriate values for your ADC and potential
// divider setup to ensure that the battery voltage reading performed by the sensors
// is as accurate as possible.
//
// By calculating the battery multiplier here, you can be sure that the actual
// battery voltage calulation is done as efficiently as possible.
// The compiler will do all these calculations so your program does not have to.

const float BATTERY_R1 = 10000.0; // resistor to battery +
const float BATTERY_R2 = 10000.0; // resistor to Gnd
const float BATTERY_DIVIDER_RATIO = BATTERY_R2 / (BATTERY_R1 + BATTERY_R2);
const float ADC_FSR = 1023.0;    // The maximum reading for the ADC
const float ADC_REF_VOLTS = 5.0; // Reference voltage of ADC

const float BATTERY_MULTIPLIER = (ADC_REF_VOLTS / ADC_FSR / BATTERY_DIVIDER_RATIO);


// these are aliases of convenience
// the BASIC sensor board has two LEDs
// const int LED_LEFT = USER_IO_6;
// const int LED_RIGHT = USER_IO_11;
// the ADVANCED sensor board has only one LED so use the value twice
const int LED_LEFT = USER_IO_6;
const int LED_RIGHT = USER_IO_6;


//***** SENSOR HARDWARE *****************************************************//
// the ADC channels corresponding to the sensor inputs. There are 8 available
// Channels 0..3 are normally used for sensors. 
// Channels 4 and 5 are available if you do not want to add an I2C device
// Channel 6 is pre-allocated to the Battery monitor
// Channel 7 is re-allocated to the function switch and button
// ADVANCED SENSOR
#define RFS_CHANNEL 0
#define RSS_CHANNEL 1
#define LSS_CHANNEL 2
#define LFS_CHANNEL 3
// BASIC SENSOR - just repeat the front sensor to make the code cleaner
// #define RFS_CHANNEL 1
// #define RSS_CHANNEL 0
// #define LSS_CHANNEL 2
// #define LFS_CHANNEL 1

// if you have  basic sensor board with a single emitter pin,
// put the same pin number for both entries
// BASIC
// const int EMITTER_A = USER_IO_12;
// const int EMITTER_B = USER_IO_12;
// ADVANCED
const int EMITTER_A = USER_IO_11;
const int EMITTER_B = USER_IO_12;



#endif