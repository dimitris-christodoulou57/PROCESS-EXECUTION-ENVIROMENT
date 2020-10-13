/*DIMITRIS CHRISTODOULOU 2113
 *EIRINI SRYRIDONI 2183
 * PROGRAMMA GIA ELEGXO HW3 ME XEIRISMO SIMATON*/
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

// DIMIOURGEIA KA8OLIKIS METEBLITIS GIA TON ELEGXO TON SIMATON
volatile sig_atomic_t gotsig=-1;

/* TO HANDLER ANA8ETI STIN KA8OLIKI METABLITI 
 * TIN AKERAIA TIMI TOU SIGUSR1 GIA NA 3EROUME 
 * OTI TO PROGRAMMA PARELABE TO SIMA*/
void handler(int sig) {
	
	gotsig=sig;
	
	
}

int main(int argc, char *argv[]){
	
	int counter, check;
	struct sigaction act= { { 0 } };
	sigset_t signal;
	
	if(argc!=5){
		printf("invalid data\n");
		return 0;
	}
	
	/*ELEGXOS TOU argv[4] GIA NA 3EROUME AN PREPEI NA 
	 * MPLOKAROUME TO SIMA I OXI ANALOGA ME TIN EPILOGI*/
	if(atoi(argv[4])==1){
		
// 		DIMIOURGEIA XIRISTI SIMATWN ME HANDLER
		act.sa_handler=handler;
		check=sigaction(SIGUSR1, &act, NULL);
		if(check==-1){
			fprintf(stderr, "error in sigaction %d\n", __LINE__);
		}
		
		
		check=sigemptyset(&signal);
		if(check==-1){
			fprintf(stderr, "error in sigemptyset %d\n", __LINE__);
		}
		
		check=sigaddset(&signal, SIGUSR1);
		if(check==-1){
			fprintf(stderr, "error in sigaddset %d\n", __LINE__);
		}
		
// 		MLPOKAROUME TO SIMA POU STAL8IKE
		check=sigprocmask(SIG_BLOCK, &signal, NULL);
		if(check==-1){
			fprintf(stderr, "error in sigpromask %d\n", __LINE__);
		}
		
		for (counter=0; counter<atoi(argv[2]); counter++){
// 			STA MISA TWN EPANALIPSEWN 3EMPLOKAROUME TO SIMA
			if(counter==atoi(argv[2])/2) {
				check=sigprocmask(SIG_UNBLOCK, &signal, NULL);
			}
			/*AN PRAGMATOPOII8I TO HANDLER MPAINEI STO IF KAI KANEI TO 
			 *TON METRITI O KAI TON XEIRISTI TOU SIMATOS -1*/ 
			if(gotsig!=-1){
				counter=0;
				gotsig=-1;
			}
			printf("pid: %d ", getpid());
			printf("%d/%d\n", counter, atoi(argv[2]));
			sleep(5);
		}
	}
	else if(atoi(argv[4])==0){
		
// 		DIMIOURGEIA XIRISTI SIMATWN ME HANDLER
		act.sa_handler=handler;
		check=sigaction(SIGUSR1, &act, NULL);
		if(check==-1){
			fprintf(stderr, "error in sigaction %d\n", __LINE__);
		}
		
		for (counter=0; counter<atoi(argv[2]); counter++){
			/*AN PRAGMATOPOII8I TO HANDLER MPAINEI STO IF KAI KANEI TO 
			 *TON METRITI O KAI TON XEIRISTI TOU SIMATOS -1*/ 
			if(gotsig!=-1){
				counter=0;
				gotsig=-1;
			}
			printf("pid: %d ", getpid());
			printf("%d/%d\n", counter, atoi(argv[2]));
			sleep(5);
		}
	}
	
	return 0;
	
}