/** @file init.c
 * @brief File for initialization code
 *
 * This file should contain the user initialize() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"
#include "liftControl.h"
#include "lcd.h"
#include "autoPrograms.h"
#include "drivePID.h"

const char* typeTitles[] = {"Skills", "Match"};
const char* matchTitles[] = {"22Left", "12Left", "7Left", "22Right", "12Right", "7Right", "Pylon Safe", "Pylon Left", "Pylon Right", "Defense", "24Left"};
const char* skillsTitles[] = {"Red Skills (L)"};
const char* sensorTypes[] = {"Encoders", "Gyroscope", "Potentiometer", "Joystick", "Motors"};
void (*matchScripts[])() = {Left22, Left12, Left7, Right22, Right12, Right7, pylon, pylonL, pylonR, defense, test};
void (*skillsScripts[])() = {redSkills};

/*
 * Runs pre-initialization code. This function will be started in kernel mode one time while the
 * VEX Cortex is starting up. As the scheduler is still paused, most API functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes (pinMode()) and port
 * states (digitalWrite()) of limit switches, push buttons, and solenoids. It can also safely
 * configure a UART port (usartOpen()) but cannot set up an LCD (lcdInit()).
 */
void initializeIO() {
}

/*
 * Runs user initialization code. This function will be started in its own task with the default
 * priority and stack size once when the robot is starting up. It is possible that the VEXnet
 * communication link may not be fully established at this time, so reading from the VEX
 * Joystick may fail.
 *
 * This function should initialize most sensors (gyro, encoders, ultrasonics), LCDs, global
 * variables, and IMEs.
 *
 * This function must exit relatively promptly, or the operatorControl() and autonomous() tasks
 * will not start. An autonomous mode selection menu like the pre_auton() in other environments
 * can be implemented in this task if desired.
 */


void initialize() {

  lcdIN = 0;
  setTeamName("1115B");

  //Sensor Initialization

  enLeftDrive = encoderInit(1, 2, 1);
  enRightDrive = encoderInit(3, 4, 0);
  enRightLift = encoderInit(7, 8, 0);
  enLeftLift = encoderInit(5, 6, 1);
  gyro = gyroInit(2, 0);

  //LCD Initialization

  lcdScriptInit(uart1);
  lcdScriptSelect();

  lMgFbHandler = taskCreate(lMgFb, TASK_DEFAULT_STACK_SIZE, NULL, 3);
  taskSuspend(lMgFbHandler);

}
