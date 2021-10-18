#pragma config(StandardModel, "EV3_REMBOT")

TSemaphore sem0;
bool avoid_followRed = false;

TSemaphore sem1;
bool followRed_followWall = false;

void cruise(){
	//Comprobamos que podemos movernos
	while(!bDoesTaskOwnSemaphore(sem1))
		semaphoreLock(sem1);

	motor[armMotor] = 90;
	motor[leftMotor] = 50;
	motor[rightMotor] = 50;

	while (SensorValue(S4) > 15){
	//Nos movemos hacia delante hasta encontrar un objeto/pared a menos de 15cm
	}
	turnLeft(0.9,seconds,20);
}

void crossDoor(){//Accion de cruzar una puerta
	turnLeft(0.4,seconds, 20);
	motor[leftMotor] = 35;
	motor[rightMotor] = 35;
	wait(1.2);
	turnRight(1,seconds, 20);
	motor[leftMotor] = 35;
	motor[rightMotor] = 35;
	wait(2);
	turnRight(0.8,seconds,20);
}

task followWall(){
	wait(1);
	while (true){
		cruise();

		//Vamos a seguir la pared/objeto por siempre hasta que algo cambie
		while (true){
			if (followRed_followWall == true){
				semaphoreUnlock(sem1);
				wait(0.5);
				break;
			}
			motor[leftMotor] = 35;
			motor[rightMotor] = 35;
			wait(0.8);
			turnRight(0.48,seconds,20);

			if (SensorValue(S4) < 25)//NO PUERTA
				turnLeft(0.4,seconds, 20);
			else if (SensorValue(S4) >= 30)//PUERTA
				crossDoor();

		}//FIN SEGUNDO WHILE TRUE

	}//FIN PRIMER WHILE TRUE
}//FIN TASK

void controlSem_followRed(){
	if (avoid_followRed == true){//CHOCAMOS
		//Paramos a los de abajo si tienen control
		followRed_followWall = true;
		while (!bDoesTaskOwnSemaphore(sem1))
			semaphoreLock(sem1);
		//Si tenemos el control nosotros lo damos
		if (bDoesTaskOwnSemaphore(sem0))
			semaphoreUnlock(sem0);

	}else {//NO CHOCAMOS
	  //Pedimos el control arriba
		while (!bDoesTaskOwnSemaphore(sem0))
			semaphoreLock(sem0);
		//Le damos el control a los de abajo
		followRed_followWall = false;
		if (bDoesTaskOwnSemaphore(sem1))
			semaphoreUnlock(sem1);
	}//FIN IF-ELSE
}

task followRed(){
	bool start_cont = false;
	int cont = 0;

	while(true){
		if (getColorName(S3) == colorRed && start_cont == false)
			start_cont = true;
		else if (cont < -100){
			start_cont = false;
			cont = 0;
		}

		if (start_cont == true && getColorName(S3) == colorRed)
			cont ++;
		else if (start_cont == true)
			cont = cont - 100;

		if (cont >= 70000){
			followRed_followWall = true;
			while(!bDoesTaskOwnSemaphore(sem0))
				semaphoreLock(sem0);

			while (!bDoesTaskOwnSemaphore(sem1))
				semaphoreLock(sem1);

			motor[leftMotor] = 80;
			motor[rightMotor] = 80;
			displayText(0,":)!!");
			wait(3);
		}//FIN IF CONT >= 1O

		controlSem_followRed();

	}//FIN WHILE TRUE
}//FIN DE TASK

task avoid(){
	while (true){

		if (SensorValue(S1) == 1 || SensorValue(S4) <= 4){
			avoid_followRed = true;
			while (!bDoesTaskOwnSemaphore(sem0))
				semaphoreLock(sem0);

			stopAllMotors();
			motor[leftMotor] = -25;
			motor[rightMotor] = -25;
			wait(1);
			stopAllMotors();
			avoid_followRed = false;
			semaphoreUnlock(sem0);

		}//FIN IF
	}//FIN WHILE TRUE
}//FIN TASK

task main (){
	semaphoreInitialize(sem0);
	semaphoreInitialize(sem1);
	startTask(avoid);
	startTask(followRed);
	startTask(followWall);

	while (true){
	}
}
