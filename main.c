#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

struct client_struct {
  char task;
  int id;
};
pthread_t barber3;      // This barber cuts the client's hair

sem_t barbershop_seats; // Grants access to update the 'seats' variable
sem_t client_queue3;     // Arranges the clients in a queue to be served

sem_t barber3_ready;    // Mutex that tells when barber 3 is ready to serve the clients

int seats = 10;         // Amount of seats in the barbershop
void* haircut()
{
  while(1)
  {
    sem_wait(&client_queue3);
    sem_wait(&barbershop_seats);

    seats++;
    printf("Barber number 3 is cutting a customer's hair\n");

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

  sem_t* barber;
  char* message;
      barber = &barber3_ready;
      message = "Client number %d is having a haircut\n";
      if(seats > 0)
		  {
		    seats--;
		    sem_post(&client_queue3);
		    sem_post(&barbershop_seats);
		    sem_wait(barber);
		    printf(message, id);
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
  sem_init(&client_queue3, 0, 0);
  sem_init(&barber3_ready, 0, 0);
  pthread_create(&barber3, NULL, &haircut, NULL);

  if(argc < 1)
  {
    printf("-- Clients string wasn't specified\n");
    exit(1);
  }

  char* clients_arg;
  clients_arg = argv[1];

  printf("Client string %s\n", clients_arg);

  int client_num = strlen(clients_arg);

  pthread_t clients[client_num];

  int i;
  for(i = 0; i < client_num; i++)
  {
    char task = clients_arg[i];

    if(
        task != 'h')
    {
      printf("-- Unknown symbol '%c' was entered\n", task);
      exit(1);
    }

   	printf("Client %d entered the barber shop\n", i);

    struct client_struct next_client;
    next_client.task = task;
    next_client.id = i;

    pthread_create(&clients[i], NULL, &client, (void*) &next_client);
  }

  for(i = 0; i < client_num; i++){
	  (void) pthread_join(clients[i], NULL);
  }
  return 0;
}
