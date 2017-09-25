#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

struct client_struct {
  char task;
  int id;
};

pthread_t barber1;      // This barber shaves the client's beard
pthread_t barber2;      // This barber paints the client's hair
pthread_t barber3;      // This barber cuts the client's hair

sem_t barbershop_seats; // Grants access to update the 'seats' variable

//sem_t client_queue;     // Arranges the clients in a queue to be served

sem_t barber1_ready;    // Mutex that tells when barber 1 is ready to serve the clients
sem_t barber2_ready;    // Mutex that tells when barber 2 is ready to serve the clients
sem_t barber3_ready;    // Mutex that tells when barber 3 is ready to serve the clients
sem_t cust_ready;    // Mutex that tells when customer 1 is ready to serve the clients



int seats = 10;         // Amount of seats in the barbershop
int clients_attended = 0;

void*
shave()
{
  while(1)
  {
	sem_wait(&cust_ready);
    sem_wait(&barbershop_seats);
    seats++;

    printf("Barber number 1 is shaving a customer's beard\n");

	clients_attended++;
    sem_post(&barber1_ready);
    sem_post(&barbershop_seats); 
  }

  return NULL;
}

void*
paint()
{
  while(1)
  {
    sem_wait(&cust_ready);
    sem_wait(&barbershop_seats);
    seats++;
	
    printf("Barber number 2 is painting a customer's hair\n");

	clients_attended++;
    sem_post(&barber2_ready);
    sem_post(&barbershop_seats);
    
  }

  return NULL;
}

void*
haircut()
{
  while(1)
  {
    sem_wait(&cust_ready);
    sem_wait(&barbershop_seats);
    seats++;
	
    printf("Barber number 3 is cutting a customer's hair\n");

	clients_attended++;
    sem_post(&barber3_ready);
    sem_post(&barbershop_seats);
    
  }

  return NULL;
}

void*
client(void* new_client)
{
  sem_wait(&barbershop_seats);
  struct client_struct *this_client = new_client;
  char task = this_client->task;
  int id = this_client->id;
  if(seats > 0){

  char* message;
  
  switch(task)
  {
    case 's':
      message = "Client number %d is having his beard shaved\nIt's super effective!\n";
	  seats--;
	  sem_post(&cust_ready);
      sem_post(&barbershop_seats);
      sem_wait(&barber1_ready);

      printf(message, id);

      break;

    case 'p':
      message = "Client number %d is having his hair painted\nIt's super effective!\n";
	  seats--;
	  sem_post(&cust_ready);
      sem_post(&barbershop_seats);
      sem_wait(&barber2_ready);
	
      printf(message, id);
	
      break;

    case 'h':
      message = "Client number %d is having a haircut\nIt's super effective!\n";
	  seats--;
	  sem_post(&cust_ready);
      sem_post(&barbershop_seats);
      sem_wait(&barber3_ready);

      printf(message, id);
	
      break;
  }
	
  
  }
  else
  {
    sem_post(&barbershop_seats);

    printf("Client number %d left due to no available seats\n", id);

  }

  return NULL;
}

int
main(int argc, char** argv)
{
  sem_init(&barbershop_seats, 0, 1);


  sem_init(&cust_ready, 0, 0);

  sem_init(&barber1_ready, 0, 0);
  sem_init(&barber2_ready, 0, 0);
  sem_init(&barber3_ready, 0, 0);

  pthread_create(&barber1, NULL, &shave, NULL);
  pthread_create(&barber2, NULL, &paint, NULL);
  pthread_create(&barber3, NULL, &haircut, NULL);

  char* clients_arg;
  clients_arg = "sssssssssssssssssssssssssssssssssssssppphphpsphpsphsphppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppsssssssssssssshhhhhhphhppppppppppppppshshshshshspssssssssssssssssssssphphphhhhhhhhhhhhhhhhhhhhhhhpppppppppppppppppppssssssssssssssssshhhhhhp";

  printf("Client string %s\n", clients_arg);

  int client_num = strlen(clients_arg);

  pthread_t clients[client_num];

  int i;
  for(i = 0; i < client_num; i++)
  {
    char task = clients_arg[i];

    if(task != 's' &&
        task != 'p' &&
        task != 'h')
    {
      printf("-- Unknown symbol '%c' was entered\n", task);
      exit(1);
    }
	
    struct client_struct next_client;
    next_client.task = task;
    next_client.id = i;

    pthread_create(&clients[i], NULL, &client, (void*) &next_client);

	printf("A wild CLIENT %d appears!\n", i);

  }

	printf("Clients atendidos: %d\n", clients_attended);
  return 0;
}