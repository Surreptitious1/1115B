#include "liftControl.h"
#include "main.h"
#include "repos.h"
#include "math.h"
#include "drivePID.h"

#define PI 3.14159

int wheeld = 11;
int robotd = 39;
//int speed = 2;
int gearRatio = 7;
int gyroOld = 0;

void lMgFb(void *parameter) {
  int calc = 0;
  forebarVar = analogRead(1);
  mobileVar = 0;
  liftVar = 0.5 * (encoderGet(enLeftLift) + encoderGet(enRightLift));
  while (0 == 0) {
    delay(20);

    //Lift
    motorSet(2, repos(encoderGet(enLeftLift), -liftVar, 30)); //Neg Up
    motorSet(3, repos(encoderGet(enRightLift), liftVar, 30)); //Pos Up

    //Forebar
    calc = repos2(analogRead(1), forebarVar, 30, .2);
    motorSet(8, calc);
    motorSet(9, calc);

    //Mobile Goal
    if (mobileVar != 0) {
      calc = 127 * mobileVar;
      motorSet(10,calc); //Pos Out
      if (mobileVar > 0) {
        mobileVar = mobileVar - 1;
      }
      if (mobileVar < 0) {
        mobileVar = mobileVar + 1;
      }
    } else {
      motorSet(10, 0);
    }
  }
}

void forebarSet(int pos, bool wait) {
  int calc = repos(analogRead(1), pos, 30);
  while (wait && abs(pos-analogRead(1)) > 30) {
    motorSet(8, calc);
    motorSet(9, calc);
    lcdPrint(uart1, 1, "Ptmr:%d", analogRead(1));
  }
  if (wait != 1) {
    forebarVar = pos;
  }
}

void extake(int del) {
  motorSet(1, 100);
  delay(del);
  motorStop(1);
}

void intake(int del) {
  motorSet(1, -100);
  delay(del);
  motorStop(1);
}

void lift(int targLift, bool wait) {
  int i = 10;
  while (wait) {
    delay(20);

    lcdPrint(uart1, 1, "enL: %d", encoderGet(enLeftLift));
    lcdPrint(uart1, 2, "enR: %d", encoderGet(enRightLift));

    motorSet(2, repos(encoderGet(enRightLift), -targLift, 15));
    motorSet(3, repos(encoderGet(enLeftLift), targLift, 15));

    if (inr(max(encoderGet(enLeftLift), encoderGet(enRightLift)), targLift, 20)) {
      i -= 1;
    } else {
      i = 10;
    }
    if (i <= 0) {
      motorStop(2);
      motorStop(3);
      break;
    }
  }
  if (wait != 1) {
    liftVar = targLift;
  }
}

void liftUp(int targLift) {
  while (encoderGet(enRightLift) < targLift || encoderGet(enLeftLift) < targLift) {
    delay(20);

    lcdPrint(uart1, 1, "enL: %d", encoderGet(enLeftLift));
    lcdPrint(uart1, 2, "enR: %d", encoderGet(enRightLift));

    if(encoderGet(enRightLift) < targLift) {
      motorSet(2, 127);
    } else {motorStop(2);}
    if(encoderGet(enLeftLift) < targLift) {
      motorSet(3, -127);
    } else {motorStop(3);}
  }
}

void liftDown (int targLift) {
  while (encoderGet(enRightLift) < targLift || encoderGet(enLeftLift) < targLift) {
    delay(20);

    lcdPrint(uart1, 1, "enL: %d", encoderGet(enLeftLift));
    lcdPrint(uart1, 2, "enR: %d", encoderGet(enRightLift));

    if(encoderGet(enLeftLift) > targLift) {
      motorSet(2, -127);
    } else {motorStop(2);}
    if(encoderGet(enRightLift) > targLift) {
      motorSet(3, 127);
    } else {motorStop(3);}
  }
}

void forward(float dist) {
  encoderReset(enLeftDrive);
  encoderReset(enRightDrive);
  float calc = ceil((dist / (wheeld*PI))*360);
  int tick = 5; // Amout of seconds*10 after the bot moves to correct any final
  while (true) {
    delay(20);
    motorSet(5, 0.8*repos(encoderGet(enLeftDrive), calc, 30));
    motorSet(7, 0.8*repos(encoderGet(enLeftDrive), calc, 30));
    motorSet(4, -0.8*repos(encoderGet(enRightDrive), calc, 30));
    motorSet(6, -0.8*repos(encoderGet(enRightDrive), calc, 30));

    lcdPrint(uart1, 1, "enR: %d", encoderGet(enRightDrive));
    lcdPrint(uart1, 2, "enL: %d", encoderGet(enLeftDrive));

    //motorSet(2, repos2(-encoderGet(leftBack), calc, getSpeed(leftBack), 1, 1));
    //motorSet(3, repos2(-encoderGet(leftBack), calc, getSpeed(leftBack), 1, 1));
    //motorSet(4, -repos2(encoderGet(rightBack), calc, getSpeed(rightBack), 1, 1));
    //motorSet(5, -repos2(encoderGet(rightBack), calc, getSpeed(rightBack), 1, 1));
    if (inr(max(encoderGet(enLeftDrive), encoderGet(enRightDrive)), calc, 35)) {
      tick -= 1;
    } else {
      tick = 5;
    }
      if (tick <= 0) {
        motorStop(5);
        motorStop(7);
        motorStop(4);
        motorStop(6);
        break;
      }
      delay(100);
    }
  }

  void rotSet() {
    gyroOld = gyroGet(gyro);
  }

  void rotGo(float speed) {
    int calc = gyroOld - gyroGet(gyro);
    rotate(calc, speed);
  }

  void rotate(int ang, float speed) {
    gyroOld = gyroOld - gyroGet(gyro);
    gyroReset(gyro);
    float calc = 12/speed;
    int t = 0;
    int tick = 5; // Amout of seconds*10 after the bot moves to correct any final
    while (0 == 0) {
      delay(20);
      t = t + 20;
      motorSet(5, speed*repos(gyroGet(gyro), ang, 30));
      motorSet(7, speed*repos(gyroGet(gyro), ang, 30));
      motorSet(4, speed*repos(gyroGet(gyro), ang, 30));
      motorSet(6, speed*repos(gyroGet(gyro), ang, 30));

      lcdPrint(uart1, 1, "Gyro: %d", gyroGet(gyro));
      lcdPrint(uart1, 2, "Targ: %d", ang);

      /*motorSet(2, repos2(-encoderGet(leftBack), calc, getSpeed(leftBack), 1, 1));
      motorSet(3, repos2(-encoderGet(leftBack), calc, getSpeed(leftBack), 1, 1));
      motorSet(4, -repos2(encoderGet(rightBack), calc, getSpeed(rightBack), 1, 1));
      motorSet(5, -repos2(encoderGet(rightBack), calc, getSpeed(rightBack), 1, 1));*/
      if (inr(gyroGet(gyro), ang, calc) || t > 250) {
        tick -= 1;
      } else {
        tick = 5;
      }
        if (tick <= 0) {
          motorStop(5);
          motorStop(7);
          motorStop(4);
          motorStop(6);
          break;
        }
        delay(100);
      }
    }

void forward2(float dist) {
  dist = ceil((dist / (wheeld*PI))*360);
  encoderReset(enLeftDrive);
  encoderReset(enRightDrive);
  int gyroInit = gyroGet(gyro);
  int i = 5;
  int diff = 0;
  int calcL = 0;
  int calcR = 0;

  while (0 == 0) {
    delay(20);
    diff = gyroGet(gyro) - gyroInit;
    calcL = 0.8*repos(encoderGet(enLeftDrive), dist, 30);
    calcR = -0.8*repos(encoderGet(enRightDrive), dist, 30);
    if (diff > 0) {
      calcL = calcL/abs(1 + (0.1 * diff));
    }
    if (diff < 0) {
      calcR = calcR/abs(1 + (0.1 * abs(diff)));
    }
    lcdPrint(uart1, 1, "CalcL: %d", calcL);
    lcdPrint(uart1, 2, "enL: %d", encoderGet(enLeftDrive));

    motorSet(5, calcL);
    motorSet(7, calcL);
    motorSet(4, calcR);
    motorSet(6, calcR);

    if (inr(max(encoderGet(enLeftDrive), encoderGet(enRightDrive)), dist, 35)) {
      i -= 1;
    } else {
      i = 5;
    }

    if (i <= 0) {
      motorStop(5);
      motorStop(7);
      motorStop(4);
      motorStop(6);
      break;
    }
  }
}

void forward3(float dist) {
  dist = ceil((dist / (wheeld*PI))*360);
  driveSetPos(dist);
  int i = 5;
  while (0 == 0) {
    delay(20);
    driveMove();

    if (inr(max(encoderGet(enLeftDrive), encoderGet(enRightDrive)), dist, 10)) {
      i -= 1;
    } else {
      i = 5;
    }

    if (i <= 0) {
      motorStop(5);
      motorStop(7);
      motorStop(4);
      motorStop(6);
      break;
    }
  }
}

void forward3t(float dist, int t) {
  dist = ceil((dist / (wheeld*PI))*360);
  driveSetPos(dist);
  int i = 5;
  while (0 == 0) {
    delay(20);
    driveMove();
    t = t - 20;
    if (inr(max(encoderGet(enLeftDrive), encoderGet(enRightDrive)), dist, 10)) {
      i -= 1;
    } else {
      i = 5;
    }

    if (i <= 0  || t < 0) {
      motorStop(5);
      motorStop(7);
      motorStop(4);
      motorStop(6);
      break;
    }
  }
}

void mobileMove(float time, bool wait) {
  if (wait) {
    if (time > 0) {
      motorSet(10, -127);
    }
    if (time < 0) {
      motorSet(10, 127);
    }

    time = abs(time);

    while(time >= 0) {
      delay(20);
      time = time - 20;
    }

    motorSet(10, 0);
  }
  if (wait != 1) {
    mobileVar = floor(time/20);
  }
}

void back(float dist) {
  encoderReset(enLeftDrive);
  encoderReset(enRightDrive);
  float calc = -ceil((dist / (wheeld*PI))*360);
  int tick = 10; // Amout of seconds*10 after the bot moves to correct any final
  while (true) {
    delay(20);
    motorSet(5, repos(encoderGet(enLeftDrive), calc, 30));
    motorSet(7, repos(encoderGet(enLeftDrive), calc, 30));
    motorSet(4, -repos(encoderGet(enRightDrive), calc, 30));
    motorSet(6, -repos(encoderGet(enRightDrive), calc, 30));

    lcdPrint(uart1, 1, "enR: %d", encoderGet(enRightDrive));
    lcdPrint(uart1, 2, "enL: %d", encoderGet(enLeftDrive));

    /*motorSet(2, repos2(-encoderGet(leftBack), calc, getSpeed(leftBack), 1, 1));
    motorSet(3, repos2(-encoderGet(leftBack), calc, getSpeed(leftBack), 1, 1));
    motorSet(4, -repos2(encoderGet(rightBack), calc, getSpeed(rightBack), 1, 1));
    motorSet(5, -repos2(encoderGet(rightBack), calc, getSpeed(rightBack), 1, 1));*/
    if (inr(min(encoderGet(enLeftDrive), encoderGet(enRightDrive)), calc, 20)) {
      tick -= 1;
      if (tick <= 0) {
        motorStop(5);
        motorStop(7);
        motorStop(4);
        motorStop(6);
        break;
      }
      delay(100);
    }
  }
}

void align(int speed) {
	motorSet(4, -speed);
	motorSet(5, speed);
  motorSet(6, -speed);
	motorSet(7, speed);
	delay(500);
	while (getSpeed(enLeftDrive) != 0 && getSpeed(enRightDrive) != 0) {
    motorSet(4, -speed);
  	motorSet(5, speed);
    motorSet(6, -speed);
  	motorSet(7, speed);
	}
	delay(500);
  motorStop(5);
  motorStop(7);
  motorStop(4);
  motorStop(6);
	encoderReset(enLeftDrive);
	encoderReset(enRightDrive);
}
