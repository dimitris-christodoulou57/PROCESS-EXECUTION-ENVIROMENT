/*DIMITRIS CHRISTODOULOU 2113
 *EIRINI SRYRIDONI 2183
 *MESO TOU PROGRAMMATOS DIMIOURGOUNTAI DIADIKASIES
 * PAIDIA POU TREXOUN KIKLIKA GIA SIGKEKRIMENO XRONO*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#define PROSECC_RUN 1 //OTAN TREXEI I DIERGASIA 
#define PROSECC_STOP 0 //OTAN DEN TREXEI I DIERGASIA

struct list {
	int size_args, status_prosecc;
	//ME TO size_args METRAME TA ORISMATA POU 8A DWSEI O XRISTIS 
	//KAI ME TO status_prosecc BLEPOUME AN TO PAIDI TREXEI
	pid_t pid_prosecc;
	//APO8IKEUSI TOU pid  TIS DIERGASIAS
	char *args[20];
	//APO8IKEUSI TON ORISMATWN SE PINAKA
	struct list *next;
	struct list *prev;
};

struct list *head;

// ARXIKOPOIISI TIS LISTAS
void init_list() {
	head = (struct list *)malloc(sizeof(struct list));
	if (head == NULL) {
		fprintf(stderr, "error in malloc %d\n", __LINE__);
		return;
	}
	
	head->next = head;
	head->prev = head;
}

int add_node(pid_t program_pid, int number_args, char *args[]){
	
	int i, j, length;
	struct list *new_node;
	
// 	DIMIOURGEIA TOU KOMBOU
	new_node=(struct list*)malloc(sizeof(struct list));
	if(new_node==NULL){
		fprintf(stderr, "error in malloc %d\n", __LINE__);
		return -1;
	}
	
	new_node->size_args=number_args-1;
	new_node->pid_prosecc=program_pid;
	
/*AN I DIESRASIA EINAI I PROTI POU 8A MPEI STIN LISTA TOTE TO status_prosecc 8A PAREI TIN TIMI 1 DILADI OTI TREXEI
 * DIAFORETIKA TIN TIMI 0 POU SIMENEI OTI DEN TREXEI*/
	if(head->next==head){
		new_node->status_prosecc=PROSECC_RUN;
	}
	else {
		new_node->status_prosecc=PROSECC_STOP;
	}
	
	for(i=0, j=0; i<number_args-1; i++, j++){
		length=strlen(args[i]);
		new_node->args[j]=(char*)malloc((length+1)*sizeof(char));
		if(new_node==NULL){
			fprintf(stderr, "error in malloc %d\n", __LINE__);
			return -1;
		}
		strcpy(new_node->args[j],args[i]);
	}
	
	new_node->next=head->next;
	new_node->prev=head;
	new_node->next->prev=new_node;
	new_node->prev->next=new_node;
	
	return 0;
}

int find_node() {
	
	struct list *current;
	pid_t pid;
	
	head->status_prosecc=PROSECC_RUN;
	head->pid_prosecc=0;
	for(current=head->prev; current->status_prosecc!=PROSECC_RUN;  current=current->prev) {
	}
	if(current==head){
		return (current->pid_prosecc);
	}
	else {
		pid=current->pid_prosecc;
		current->status_prosecc=PROSECC_STOP;
		current->prev->status_prosecc=PROSECC_RUN;
		return (pid);
	}
}

int find_node_for_cont() {
	
	struct list *current;
	pid_t pid;
	
	head->status_prosecc=PROSECC_RUN;
	head->pid_prosecc=0;
	for(current=head->prev; current->status_prosecc!=PROSECC_RUN;  current=current->prev) {
	}
	if(current==head){
		pid=current->prev->pid_prosecc;
		current->prev->status_prosecc=PROSECC_RUN;
		return (pid);
	}
	else {
		pid=current->pid_prosecc;
		return (pid);
	}
}

void print_list(){
	
	int i,j;
	struct list *current;
	
	for(current=head->next, j=0; current!=head; current=current->next, j++){
		
		printf("pid: %d, name: (", current->pid_prosecc);
		for(i=0; i<current->size_args; i++){
			printf("%s", current->args[i]);
			if(i!=(current->size_args-1)) {
				printf(" ,");
			}
		}
		
		if(current->status_prosecc==PROSECC_RUN)
			printf(")(R)\n");
		else
			printf(")\n");
		
	}
	
	if(j==0){
		printf("list empty\n");
	}
}

// DIAGRAFEI KOMBOU 
void remove_node(pid_t program_pid){
	
	struct list *current;
	
	head->pid_prosecc=program_pid;
	for(current=head->next; current->pid_prosecc!=program_pid; current=current->next){
	}
	
	if(current==head){
		return;
	}
	
	current->next->prev=current->prev;
	current->prev->next=current->next;
	
	free(current);
	return;
}


void find_node_term(pid_t child_pid){
	
	struct list *current;
	
	head->pid_prosecc=child_pid;
	for(current=head->prev; current->pid_prosecc!=child_pid; current=current->prev){
	}
	
	
	if(current->prev==head){
		head->prev->status_prosecc=PROSECC_RUN;
		kill(head->prev->pid_prosecc, SIGCONT);
		remove_node(child_pid);
	}
	else {
		current->prev->status_prosecc=PROSECC_RUN;
		kill(current->prev->pid_prosecc, SIGCONT);
		remove_node(child_pid);
	}
	
}

// otan xtipaei to roloi i trexousa diergasia stamataei kai i epomeni sti lista 3ekinaei
void handler_alarm(int sig){
	
	int check;
	pid_t pid_stop,pid_cont;
	
	printf("BEEP\n");
	
// 	briskei poia diergasia trexei gia na kanei SIGSTOP
	pid_stop=find_node();
	if(pid_stop!=0){
		check=kill(pid_stop, SIGSTOP);
		if(check==-1){
			fprintf(stderr, "error in kill %d\n", __LINE__);
			return ;
		}
	}
	
// 	briskei poia diergasia prepei na kanei SIGCONT
	pid_cont=find_node_for_cont();
	if(pid_cont!=0){
		check=kill(pid_cont, SIGCONT);
		if(check==-1){
			fprintf(stderr, "error in kill %d\n", __LINE__);
			return ;
		}
	}
}

int main(int argc, char *argv[]){
	
	char string[255], *args[20], *s, *ptr;
	int i=0, number_args, length, pid1, pipe1[2], id_pid, check, found, status, flags;
	pid_t term_pid, pid_wait, quit_pid;
	struct sigaction act= { { 0 } };
	struct itimerval time = { { 0 } };
	
// 	arxikooisi listas
	init_list();
	
// 	 tou simatos SIGALRM me handler kai SA_RESTART gia na min xanoume dedomena 
// 	pou pliktrologise o xristis otan xtipaei to roloi
	act.sa_handler=handler_alarm;
	act.sa_flags=SA_RESTART;
	check=sigaction(SIGALRM,&act,NULL);
	if(check==-1){
		fprintf(stderr, "error in sigaction %d\n", __LINE__);
		return -1;
	}
	
	
// 	orizoume ka8e pote prepei na stelnete to sima me tin setitimer
	time.it_interval.tv_sec=20;
	time.it_interval.tv_usec=0;
	time.it_value.tv_sec=20;
	time.it_value.tv_usec=0;
	check=setitimer(ITIMER_REAL, &time, NULL);
	if(check==-1){
		fprintf(stderr, "error in sigaction %d\n", __LINE__);
		return -1;
	}
	
// 	3emplokaroume tin fgets etsi wste na min perimenei na diabasei kati apo to 
// 	pliktrologio gia na pragmatopoiisi tin while gia na doume an kapoio paidi teleiwsw
// 	i termatistike apo allo priktorologio prin xtipisi to poloi
	flags=fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO,F_SETFL, flags|O_NONBLOCK);
	
// 	kanoume mia epanalipsi i opoia stamataei an dwsoume "quit" i kati allo paei straba me ta system call
	while(1){
		
// 		diabazoume apo to pliktrologio
		s=fgets(string, 254 , stdin);
		
		
		if(s!=NULL){
			
// 			kanoume to i=0 gia na apo8ikeuontai ta orismata apo tin arxi tou pinaka
			i=0;
			
// 			xrisimopoioume tin strtok gia na xwrizei se le3eis to string otan uparxei keno mexri na brei \n
			ptr=strtok(string, " \n");
			while(ptr!=NULL){
// 				briskoume to mikos tis le3eis gia na desmeusoume me malloc mnimi
				length=strlen(ptr);
				args[i]=(char*)malloc((length+1)*sizeof(char));
				if(args[i]==NULL){
					printf("no memory\n");
					return -1;
				}
				strcpy(args[i], ptr);
				ptr=strtok(NULL, " \n");
				i++;
			}
			
// 			apo8ikeuoume posa orismata dw8ikan kai stin epomeni 8esi tou pinaka 
// 			bazoume NULL gia na mporesoume na tis perasoume stin execv
			number_args=i;
			args[number_args]=NULL;
			
			if (args[0] == NULL) continue;
			
			if(strcmp(args[0], "exec")==0) {
				
// 				dimiourgeia pipe gia na perasoume too id tou paidiou ston patera
				check=pipe(pipe1);
				if(check==-1){
					fprintf(stderr, "error in pipe %d\n", __LINE__);
					return -1;
				}
				
// 				dimiourgeia paidiou
				pid1=fork();
				if(pid1==-1){
					fprintf(stderr, "error in fork %d\n", __LINE__);
					return -1;
				}
				
// 				briskoume to id kai to stelnoume ston patera kai trexoume to 
// 				to programma mesa apo to paidi me execv
				if(pid1==0) {
					id_pid=getpid();
					check=write(pipe1[1], &id_pid, sizeof(int));
					if(check==-1){
						fprintf(stderr, "error in write %d\n", __LINE__);
						return -1;
					}
					
// 					klinoume ta pipe gia na min mplokari to programma 
					close(pipe1[0]);
					close(pipe1[1]);
					
					execv(args[1], &args[1]);
					perror("execlp");
					return -1;
					
				}
				
// 				apo8ikeuoume to id tou paidiou ston patera
				close(pipe1[1]);
				check=read(pipe1[0], &id_pid, sizeof(int));
				if(check==-1){
					fprintf(stderr, "error in read %d\n", __LINE__);
					return -1;
				}
				close(pipe1[0]);
				
// 				an i lista den einai adeia i find_node_for_cont 8a epistrepsi 
// 				kapoio id opote 8a prepei na to kanoume na stamatisei me SIGSTOP
				found=find_node_for_cont();
				if(found!=0){
					kill(id_pid, SIGSTOP);
					if(check==-1) {
						fprintf(stderr, "error in kill %d\n", __LINE__);
						return -1;
					}
				}
				
// 				pros8etoume ta orismata tou paidiou stin lista
				add_node(id_pid,number_args, &args[1]);
			}
			
// 			an dw8ei term 8a prepei na skotosoume to programma me to katalilo id
			else if(strcmp(args[0], "term")==0){
// 				ELEXOUME AN DWTHIKE DEYTERI PARAMETROS DHLADH TO ID THS DIERGASIAS POU THELOYME GIA NA APOFYGOYME SEGMENTATION FAULT
				if(args[1]!=NULL){
					term_pid=atoi(args[1]);
					check=kill(term_pid, SIGTERM);
					if(check==-1) {
						fprintf(stderr, "error in kill %d\n", __LINE__);
						return -1;
					}
				}
			}
			
// 			an dw8ei sig 8a prepei na stalei sima SIGUSR1
			else if(strcmp(args[0], "sig")==0){
				//ELEXOUME AN DWTHIKE DEYTERI PARAMETROS DHLADH TO ID THS DIERGASIAS POU THELOYME GIA NA APOFYGOYME SEGMENTATION FAULT
				if(args[1]!=NULL){   
					term_pid=atoi(args[1]);
					check=kill(term_pid, SIGUSR1);
					if(check==-1) {
						fprintf(stderr, "error in kill %d\n", __LINE__);
						return -1;
					}
				}
			}
			
// 			me tin epilogi info prepei na emfanizete to periexomeno tis lista
			else if(strcmp(args[0], "info")==0){
				print_list();
			}
			
// 			an dw8ei quit 8a prepei na termatizei to programma
			else if(strcmp(args[0], "quit")==0){
				break;
			}
			
			else{
				printf("invalid command\n");
			}
			
// 			kanoume free ta args[i] pou desmesa gia ta orismata
			for(i=0; i<number_args; i++){
				free(args[i]);
			}
		}
		
		else {
// 			an i fgets epistrepsi NULL kai to sfalma einai kapoio allo apo to EAGAIN 
// 			to programma prepei na termatizei 
			if (errno != EAGAIN) {
				perror("fgets");
				break;
			}
		}
		
// 		elegxoume an allakse i katastasi kapoiou paidiou dld an skoto8ike i termatise
		while(1){
			
			pid_wait=waitpid(-1, &status, WNOHANG);
// 			an den allakse i katastasi kapoiou paidiou tote 8a epistrafi 0 h' -1
			if(pid_wait<=0){
				break;
			}
			
// 			an skoto8ike i termatise tote to afairoume apo tin lista kai kanoume SIGCONT sto epomeno
			if ((WIFEXITED(status)) || (WIFSIGNALED(status))) {
				find_node_term(pid_wait);
			}
			
		}
		
		
	}
	//ELEXOYME AN YPARXOYN DIERGASIES POY TREXOUN AKOMA GIA NA TIS SKOTWSOYME ETSI WSTE NA MHN YPARXOYN PAIDIA ZOMBIE
	while(1){
		quit_pid=find_node();
		if(quit_pid!=0){
			kill(quit_pid, SIGKILL);
			waitpid(-1, &status, 0);
		}
		else{
			break;
		}
	}
	
	free(head);
	for(i=0; i<number_args; i++){
		free(args[i]);
	}
	return 0;
}

