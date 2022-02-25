#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#define MAX 80
#define MAX_SIZE 30
#define PORT 8080
#define SA struct sockaddr

typedef struct Patient{
    char names[MAX_SIZE];
    int years;
    char egn[10];
    char diagnose[MAX_SIZE];
}sPatient_t;

struct List{
    sPatient_t payload;
    struct List *next;
};
typedef struct List sList_t;



sList_t *stack_sList_t(sList_t *head,sList_t *node);
sList_t *new_patient(sList_t *head,sList_t *node);
void print_entered_patients(sList_t *head, sList_t *node);
sList_t *delete_by_egn(sList_t *head,sList_t *node);
sList_t *patient_search(sList_t *head,sList_t *node);


int main()
{    
	int choice;
	

    FILE *fp=NULL;

    sList_t *head=NULL;
    sList_t *node=NULL;
	
	
	int fd[2];
	int childID = 0;
	
	pipe(fd);
	
	pid_t pid;
	pid = fork();

	
	if(-1 == pid){
		printf("Error occured udring fork()\n");
		exit(EXIT_FAILURE);
	}
	if(0 == pid){
		
		if((fp=fopen("patient.data", "ab"))==NULL){
			perror(NULL);
			exit(10);
		}

		do{
			printf("Menu:");
			printf("\n1. Add a patient;");
			printf("\n2. Show all patients;");
			printf("\n3. Decide patient's fate;");
			printf("\n4. Exit.");

			do{
			    printf("\n\nEnter your choice: ");
			    scanf("%d",&choice);
			    while(getchar()!='\n');
			}while(choice<1 || choice>4);

			if(choice == 1){
			    head = stack_sList_t(head,node);
			    printf("\nhead first call = %p\n", head);
			}
			
			if(choice == 2){
			    if(head == NULL){
			        printf("\nNo entered patients.\n");
			    }
			    else{
			    	print_entered_patients(head, node);
			    }
			}
			if(choice == 3){
			    if(head !=NULL)
			        printf("\nTBI\n");
			        
			        node = patient_search(head, node);
			        printf("\nint main after temp = %s\n", node->payload.names);
			        if(NULL == node){
			        	printf("patient not found");
			        } else{
			        	char arr[MAX];
			        	memcpy(arr, node->payload.egn, sizeof(node->payload.egn));
			        	memcpy(arr + sizeof(node->payload.egn) + 1, node->payload.names, sizeof(node->payload.names));
			        	memcpy(arr + sizeof(node->payload.egn) + sizeof(node->payload.names) + 1, node->payload.years, sizeof(node->payload.years));
			        	memcpy(arr + sizeof(node->payload.egn) + sizeof(node->payload.names) + sizeof(node->payload.diagnose) + 1, node->payload.years, sizeof(node->payload.diagnose));
			        	
						for(int i; i<sizeof(arr); i++){
							printf("%c", arr[i]);
						}
						
						close(fd[0]);
						dup(fd[1]);
						write(1, arr, sizeof(arr));
			        }
			}

			printf("\n");
		}while(choice != 4);
		/*
		close(fd[0]);

		dup(fd[1]);
		write(1, head, sizeof(sList_t));
		*/
		// printf("\n %s\n", test);

	} else{
		
		//int fd= open("purgatory.register", O_CREAT | O_RDWR);
		char buf[200];
		int file;
		char arr[MAX];
		int n;
		printf("\nParent: starting");
		printf("\nWaiting for dead bodies...\n\n");
		

		wait(NULL);

		if(read(fd[0], arr, sizeof(arr))){
			perror("couldn't read the patient's info");
		} else{
		
			if((file = open("purgatory.register", O_CREAT | O_RDWR, 0777)) < 0){
				perror("open");
				exit(1);
			}else{
				write(file, arr, sizeof(arr));
			}
		
		}
		

		printf("\nhead in parent = %p\n", head);
		printf("\nDead body found\n");


		// printf("\n %s\n", test);

		close(file);
		
	}
    
    
    return 0;
}


sList_t *patient_search(sList_t *head,sList_t *node)
{
    char wanted_patient[MAX_SIZE];
    int choice;

    printf("\nEnter patient's EGN: ");
    fgets(wanted_patient, MAX_SIZE, stdin);

    for(node = head; node!=NULL; node = node->next){
    	printf("\nnode = %p\n", node);
        if(!strcmp(wanted_patient, node->payload.egn)){
            printf("\nName: \t\t  %s\n", node->payload.names);
            printf("Years: \t\t  %d\n", node->payload.years);
            fflush(stdout);
            printf("EGN:  \t\t %s\n", node->payload.egn);
            printf("Diagnose: \t %s\n", node->payload.diagnose);
            
            printf("\n\n Decide patient's fate: 1. Live or 2. Die");
            scanf("%d", &choice);
            
            if(1 == choice){
            	printf("\n He will live\n");
            } else{
            	printf("\n He will die\n");
            	printf("\nnode = %p\n", node);
            	return node;
            }
        }
        else{
            printf("\nNot found.\n");
        }
    }
    
    return NULL;

}

sList_t *delete_by_egn(sList_t *head,sList_t *node)
{
    sList_t *temp = NULL;
    sList_t *temp2 = NULL;
    node = head;
    temp = node->next;
    char egn_to_delete[MAX_SIZE];

    printf("\nEnter the EGN of the patient you want to delete: ");
    scanf("%s", egn_to_delete);

    for(;temp!=NULL;temp=temp->next){
        if(!strcmp(temp->payload.egn, egn_to_delete)){
            //working
            if(head->next == NULL){
                free(head);
                return NULL;
            }

           temp2 = temp->next;
           free(temp);
           node->next = temp2;

        }
        if(!strcmp(head->payload.egn, egn_to_delete)){
            temp = head->next;
            free(head);
            head = temp;
        }

        node = node->next;
    }

    return head;

}


sList_t *new_patient(sList_t *head,sList_t *node){
    sList_t *result = NULL;

    result = (sList_t*)malloc(sizeof(sList_t));

    fflush(stdin);
    printf("\nEGN: ");
    fgets(result->payload.egn, 12, stdin);

    for(node = head; node!=NULL; node=node->next){

        //printf("node = %p\nhead = %p\nresu = %p",node, head, result);

        if(!strcmp(node->payload.egn,result->payload.egn)){
            printf("\nThere is already a patient with this EGN.");

            printf("\nName: \t\t  %s\n", node->payload.names);
            printf("Years: \t\t  %d\n", node->payload.years);
            fflush(stdout);
            printf("EGN:  \t\t %s\n", node->payload.egn);
            printf("Diagnose: \t %s\n", node->payload.diagnose);

            return NULL;
        }
    }


    fflush(stdin);
    printf("\nName: ");
    fgets(result->payload.names, MAX_SIZE, stdin);

    printf("\nYears: ");
    scanf("%d",&result->payload.years);
    
    fflush(stdin);
    printf("\nDiagnose: ");
    fgets(result->payload.diagnose, MAX_SIZE, stdin);

    result->next = NULL;

    return result;
}


sList_t *stack_sList_t(sList_t *head,sList_t *node)
{

    if(head != NULL){
        //temp = head;
        node = new_patient(head, node);
        if(node != NULL){
            node->next = head;
            head = node;
        }
    }
    else{
        head = new_patient(head, node);
    }

    return head;
}

void print_entered_patients(sList_t *head, sList_t *node)
{
	printf("\nhead in print all patients = %p\n", head);
    for(node=head;node!=NULL;node=node->next){
        printf("\nName: \t\t  %s",node->payload.names);
        printf("Years: \t\t  %d\n",node->payload.years);
        printf("EGN: \t\t  %s",node->payload.egn);
        printf("Diagnose: \t\t  %s",node->payload.diagnose);

    }
}
