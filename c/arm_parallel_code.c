/*----------------------------------------------------------------------------
 * Model: FRDM KL46Z
 *----------------------------------------------------------------------------*/
 
 #include "helper.h"
 #include "slcd.h"	
 #include "RTL.h"
 
// -----------------------------------------------------------------------------
// Programa principal del TT2, Alejandro Iregui Valcarcel
// -----------------------------------------------------------------------------

// Funciones y datos auxiliares 
OS_TID taskCreatorTID, tasks[5];
OS_MUT lcdMut;

int angs [10][2];
void fill_angs (){
	int i = 0;
	
	for (i = 0; i < 10; i++){
		angs [i][0] = 50;
	}
	
	for (i = 0; i< 10; i++){
		if (i%3 == 0)
			angs[i][1] = 40;
		
		else if (i%3 == 1)
			angs[i][1] = 130;
		
		else if (i%3 == 2)
			angs[i][1] = 50;
	}
}

int is_comp(int a, int b){
	if (a+b == 90)
		return 1; // devuelve task[1] !
			
	return 0; // si no es complementario comprobamos si es suplementario
}

int is_sup(int a, int b){
	if (a+b == 180)
		return 2; // devuelve task[2] !
			
	return 3;  // si no es complementario ni suplementario -> task[3] !
}

// Interrupciones de los SW1 y SW3
short int in = 0;
void sw1_handler(void){ in = 1; }
void sw3_handler(void){ in = 3; }

// Flags de eventos
#define ini        0x0001
#define calculated 0x0002
#define fin_ev     0x0003

// Tarea que irá leyendo el array de angulos y comprobandolo 
__task void task0 (void) {
	short int i, res;
	
	os_evt_wait_or (ini, 0xFFFF);
	
	for (i = 0; i < 10; i++){
		res  = is_comp(angs[i][0], angs[i][1]);
		
		if (res == 0) 
			res = is_sup(angs[i][0], angs[i][1]);		
		
		os_evt_set (calculated, tasks[res]);
		//os_dly_wait(200);
	}
	
	os_evt_set (fin_ev, tasks[1]);
}

// Tarea que cuenta los angulos complementarios
__task void task1 (void) {
	short int contComp = 0, ret_flags;
	
	while (1){
		os_evt_wait_or(calculated | fin_ev, 0xFFFF);
		ret_flags = os_evt_get();
		
		if (ret_flags == calculated){
			contComp ++;
			
			os_mut_wait (&lcdMut, 0xffff);
			slcdSet(contComp, 1);
			os_mut_release (&lcdMut);
		
		}else if (ret_flags == fin_ev) {
			in = 0;
			setLedG(1);
			
			while (in != 1){os_dly_wait(100);} 
			in = 0;
			
			slcdSet(0, 1);
			setLedG(0);
			os_evt_set(fin_ev, tasks[2]);
			break;
		}
	}
}

// Tarea que cuenta los angulos suplementarios
__task void task2 (void) {
	short int contSup = 0, ret_flags;
	
	while (1){
		os_evt_wait_or(calculated | fin_ev, 0xFFFF);
		ret_flags = os_evt_get();
		
		if (ret_flags == calculated){
			contSup ++;
			
			os_mut_wait (&lcdMut, 0xffff);
			slcdSet(contSup, 2);
			os_mut_release (&lcdMut);
		
		}else if (ret_flags == fin_ev) {
			in = 0;
			setLedR(1);
			
			while (in != 1){os_dly_wait(100);} 
			in = 0;
			
			slcdSet(0, 2);	
			setLedR(0);
			os_evt_set(fin_ev, tasks[3]);
			break;
		}
	}
}

// Tarea que cuenta los angulos ni suplementarios ni complementarios
__task void task3 (void) {
	short int contNoSupNoComp = 0, ret_flags;
	
	while (1){
		os_evt_wait_or(calculated | fin_ev, 0xFFFF);
		ret_flags = os_evt_get();
		
		if (ret_flags == calculated){
			contNoSupNoComp ++;
			
			os_mut_wait (&lcdMut, 0xffff);
			slcdSet(contNoSupNoComp, 4);
			os_mut_release (&lcdMut);
		
		}else if (ret_flags == fin_ev) {
			in = 0;
			while (in != 1){
				setLedR(1);
				setLedG(1);
				os_dly_wait(100);
				setLedR(0);
				setLedG(0);
				os_dly_wait(100);
			} 
			in = 0;
			
			slcdSet(0, 4);
			setLedR(0);
			setLedG(0);
			os_evt_set(fin_ev, tasks[4]);
			break;
		}
	}
}

// Tarea para finalizar la ejecución del programa
__task void task4 (void){
	os_evt_wait_or(fin_ev, 0xFFFF);
		
	setLedR(1);
	setLedG(1);
	
	in = 0;
	while (in == 0){os_dly_wait(100);}
	in = 0;
	
	setLedR(0);
	setLedG(0);
	slcdClear();	
	os_evt_set(fin_ev, taskCreatorTID);
}

// Tarea de inicialización
__task void taskCreator (void) {
	taskCreatorTID = os_tsk_self();
	
	tasks[0] = os_tsk_create (task0, 1);	      
	tasks[1] = os_tsk_create (task1, 2);	      
	tasks[2] = os_tsk_create (task2, 2);	      
	tasks[3] = os_tsk_create (task3, 2);
	tasks[4] = os_tsk_create (task4, 2);
	
	// Inicializamos el mutex y comenzamos la ejecucion hasta fin
	os_mut_init (&lcdMut);
	os_evt_set (ini, tasks[0]);
	
	os_evt_wait_or(fin_ev, 0xFFFF);
}
 
int main (void) {
	// LEDs Inicialmente apagados
	setLedG(0); setLedR(0);
	
	// Inicializamos los switchs y lcd
	initSwitch3(sw3_handler);
	initSwitch1(sw1_handler);
	slcdInitialize();
	
	// Inicializamos el array de angulos y llamamos al OS
	fill_angs();
	os_sys_init(taskCreator);
}
