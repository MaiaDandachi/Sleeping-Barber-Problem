#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Done by Aly Assem & Maia Dandachi 

                               
sem_t Mutex;        // A Semaphore for the waiting room
sem_t customers;        // A Semaphore for customers waiting in the room
sem_t sBarber;        // A Semaphore for the barber

int runningTime;        // The time the main thread will need to run 
int numberOfFreeChairs; 
int customersCount;     // Number of customers 
int customerWait;       // The max time that the customer will wait to come back to the shop 
int hiarcut;             // Number of customers who got the  haircut

void barber();   // barber thread function
void customer(); //customer thread function

pthread_t barberThread;           // Barber's thread
pthread_t customersThreads[25];   // An array that hold the customers threads and its max size of 25

                   
int main() {

    printf("Enter the running time: ");
    scanf("%d",&runningTime);
	
    printf("Enter the number of free Chairs please: ");
    scanf("%d",&numberOfFreeChairs);
	
    printf("Enter the customers count please: ");
    scanf("%d",&customersCount);
	
    printf("Enter the maximun waiting time for the customer : ");
    scanf("%d",&customerWait);
    
    hiarcut = 0;   
   
    
   
    // We have 3  semaphores:
    // 0is when the semaphore is not allowed to be shared between processes, 1 is the initial value
    sem_init(&Mutex,0,1); // the mutex is 1 so that one thread access
    sem_init(&customers,0,0);
    sem_init(&sBarber,0,0);
	
   
    // Creating barber thread:
    pthread_create(&barberThread, NULL, barber, NULL); // the thread will start at barber  function
    printf("Barber has been intialized \n");
    
    // Creating customers threads:
    for (int i = 0; i < customersCount; i++){
	   pthread_create(&customersThreads[i], NULL, customer, NULL); //  the thread will start at customer function
	   printf("Customer %u has been intialized \n",customersThreads[i]);
    }
   
    // stop the main thread so that the other threads run
    sleep(runningTime);
     
    
    printf("%d out of %d customers got an  haircut.",hiarcut,customersCount);
    exit(0);
}

// barber thread function:
void barber() {
    int workingTime;    // Will be generated randomly for the time the barber needs to cut a customer's hair

    while(1) {
		
        // ENTRY SECTION 
       
        // acquire customers to wait for a customer (signal)
	    sem_wait(&customers);
		
        // acquire Mutex to access the  Chairs count  
	    sem_wait(&Mutex);

//____________________________________________

	    // CRITICAL SECTION 
	  
        // increasing the number of free Chairs by one
	    numberOfFreeChairs += 1;
	  
	    // generate random time between 1-5 seconds for the haircut
	    workingTime = (rand() % 5) + 1;
	    printf("Barber will take %d seconds to finish the haircut foe the current customer\n",workingTime);
	  
	    printf("\t And the number of free Chairs: %d\n",numberOfFreeChairs);
		
//________________________________________________		
	    //EXIT SECTION 
	  
        // signal to customer so that the barber is ready  
	    sem_post(&sBarber);
		
        // release the lock on seat count 
	    sem_post(&Mutex);
   	    sleep(workingTime); 
    } 
}

//  The Customer thread function:
void customer() {
    int waitingTime;
    int notEnd = 1;  // to start and end the customer thread
    while(notEnd == 1) {
		
	    //ENTRY SECTION 
	    
        // acquire a Mutex semaphore to access Chairs count  
	    sem_wait(&Mutex);
	    
//_____________________________________________
        // if there is no free Chairs
		
	    if(numberOfFreeChairs <= 0){
	         
		    //EXIT SECTION 
		  
	        // generate  time randomaly  for the next round
	        waitingTime = (rand() % customerWait) + 1;
		    printf("Customer %u left without a haircut, and will come back after %d seconds .\n", pthread_self(),waitingTime);
		    sem_post(&Mutex); // release the semaphore
		    
	        sleep(waitingTime);
	    }
	  
	  
        // there are free Chairs 
	    else{
		  
		    //CRITICAL SECTION 
		   
            // decrease the number of free Chairs by one
		    numberOfFreeChairs -= 1;
			
			
			printf("\tNumber of free Chairs: %d\n",numberOfFreeChairs);
		    printf("Customer %u is waiting.\n",pthread_self());
		    
		   
			//_______________________________
			
			
		    //EXIT SECTION 
		    
            // customer is ready (released a semaphore) 
		    sem_post(&customers);
			
            // release Mutex lock on the seat count 
		    sem_post(&Mutex);
			
            // waiting for barber to be ready 
		    sem_wait(&sBarber);
			
		    printf("Customer %u got a haircut \n",pthread_self());
			
		    // the customer thread will end 
		    notEnd = 0;
			
		    // increse the number of customers who got  a haircut by one
		    hiarcut += 1;
	    }
      
     }
}