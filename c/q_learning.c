#pragma config(Sensor, S1,     leftBumper,     sensorEV3_Touch, modeEV3Touch)
#pragma config(Sensor, S2, 		 backBumper,     sensorEV3_Touch, modeEV3Touch)
#pragma config(Sensor, S3,     gyro,           sensorEV3_Gyro)
#pragma config(Sensor, S4,     rightBumper,    sensorEV3_Touch, modeEV3Touch)
#pragma config(MotorPidSetting,  motorA,  255, 255, 65535, 255, 255,   65535, 65535, 65535)
#pragma config(MotorPidSetting,  motorB,  255, 255, 65535, 255, 255,   65535, 65535, 65535)
#pragma config(MotorPidSetting,  motorC,  255, 255, 65535, 255, 255,   65535, 65535, 65535)
#pragma config(MotorPidSetting,  motorD,  255, 255, 65535, 255, 255,   65535, 65535, 65535)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "movement.h"

/*
PSEUDOCODE
Initialize an empty matrix with rows for states and columns for actions.
Initialize variables for the rate of adventure and learning (alpha).
Repeat forever:
	{
		Get the current robot state based on sensors.
		Default next action to an ambiguous case.
		Cycle through the array values for the current state to find the action which will give the highest utility.
		Generate a random number between 1 and 10 to see whether the robot should randomly explore.
		If no action has been found to be better than the others, or the random number is below the exploration threshold,
		generate a random number between 0 and 3 inclusive to represent the next action.
		Execute the action decided upon.
		Find the robot state after the action.
		Calculate the reward for being in the current state.
		Update the matrix value for the initial state and action pair based on the reward, the previous value, and potential
		utility values for the current state using the Q algorithm.
	}
*/


int getState()
{
	if(SensorValue[leftBumper] && SensorValue[rightBumper] && SensorValue[backBumper])
	{
		return 5;
	}
	else if(SensorValue[leftBumper] && SensorValue[rightBumper])
	{
		return 4;
	}
	else if(SensorValue[backBumper])
	{
		return 3;
	}
	else if(SensorValue[rightBumper])
	{
		return 2;
	}
	else if(SensorValue[leftBumper])
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

void execute(int action, int state)
{
	//All of the functions used in this function are defined in the "movement.h" file.
	int sign = 1;

	if(state == 3)
	{
		sign = -1;
	}

	switch(action)
	{
		case 0:
			moveWithEncoders(100, 50, motorB, motorD);
			break;
		case 1:
			moveWithEncoders(100, -50, motorB, motorD);
			break;
		case 2:
			moveWithEncoders(75, -50 * sign, motorB, motorD);
			turnDegrees(1, 60, 50, -50, motorB, motorD, gyro);
			moveWithEncoders(175, 50 * sign, motorB, motorD);
			break;
		case 3:
			moveWithEncoders(75, -50 * sign, motorB, motorD);
			turnDegrees(0, 60, 50, -50, motorB, motorD, gyro);
			moveWithEncoders(175, 50 * sign, motorB, motorD);
			break;
		default:
			break;
	}
}

int getReward(int state)
{
	int reward;

	switch(state)
	{
		case 0:
			reward = 50;
			break;
		case 1:
			reward = -20;
			break;
		case 2:
			reward =  -20;
			break;
		case 3:
			reward = -20;
			break;
		case 4:
			reward = -40;
			break;
		case 5:
			reward = -50;
			break;
		default:
			return 0;
			break;
	}

	return reward;

}

int state;
int action;
int nextState;
int r;
int randNum;
int max;
int c;

task main()
{
	int QVALUES[6][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

	// 0,0       0, 1      0, 2      0,3     //FIRST VALUE INDICES - STATES							//SECOND VALUE INDICES - ACTIONS
	// 1,0       1, 1      1, 2      1,3     //0: No bumpers pressed (desired)					        //0: Move forwards
	// 2,0       2, 1      2, 2      2,3     //1: Left bumper pressed							//1: Move backwards
	// 3,0       3, 1      3, 2      3,3     //2: Right bumper pressed							//2: Turn right
    	// 4,0       4, 1      4, 2      4,3     //3: Back bumper pressed 							//3: Turn left
	// 5,0       5, 1      5, 2      5,3     //4: L+R bumper pressed
                                                 //5: All bumpers pressed

	float alpha = 0.1;      //Alpha is the rate of learning, the rate at which new data replaces previous knowledge.
	float discount = 0.5;		//Discount is the relative value of short term success versus long term success.
	int exploration = 2;    //Exploration is the rate at which the robot will move randomly despite there being a potentially better option.

	while(true)
	{
		state = getState();
    max = -32000;
		action = 4;

		for(c = 0; c < 4; c++)
		{
			if(QVALUES[state][c] > max)
			{
				max = QVALUES[state][c];
				action = c;
			}
		}

		writeDebugStreamLine("Best possible action found: %d", action);

		randNum = random(9);

		if(action == 4 || randNum + 1 <= exploration)
		{
			action = random(3);
		}

		writeDebugStreamLine("Actual action to execute: %d", action);
		execute(action, state);
		wait1Msec(450);
		nextState = getState();
		r = getReward(nextState);
		wait1Msec(50);

		max = -32000;

		for(c = 0; c < 4; c++)
		{
			if(QVALUES[nextState][c] > max)
			{
				max = QVALUES[nextState][c];
			}
		}

		//Implemented Q learning algorithm, should cause higher values to plateau instead of becoming infinitely higher.
		//Curr peak = ~80
		QVALUES[state][action] = (int) (QVALUES[state][action]) + alpha * (r + (discount * max) - QVALUES[state][action]);

		//Explanation: alpha is the rate of learning, the rate at which newly learned information replaces old information.
		//Discount is the comparative worth of short term rewards versus longer term rewards.

		writeDebugStreamLine("Initial state %d, Prev Action %d, Reward %d, Set Value to %d", state, action, r, QVALUES[state][action]);
		wait1Msec(500);

	} //Infinite loop ends here. Anything after this will not run.

}
