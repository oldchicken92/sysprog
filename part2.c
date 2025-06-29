#include "slow_functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

// How nice of me to include a global that tells you how many commands there were :)
int total_commands = 0;
pthread_mutex_t lock; 
pthread_cond_t cond_writer; 
pthread_cond_t cond_reader;


// ####################################################################################
// ## Please write some code in the following two functions

void * writer(void * in_ptr)
{	
	char**cmds = (char **)in_ptr;
	for (int i = 0; i < total_commands; i++){
		//lock pthread for writing
		pthread_mutex_lock(&lock);
		while(get_written() == 1){
			pthread_cond_wait(&cond_writer, &lock);
		}

		bad_write(cmds[i]);
		pthread_cond_signal(&cond_reader); //signal the reader
		//unlock for reader
		pthread_mutex_unlock(&lock);

	}
	return NULL; 

	//must include bad_write;
}

void * reader(void * empty)
{
	for (int j = 0; j < total_commands; j++){
		pthread_mutex_lock(&lock);
		while (get_written() == 0){
			pthread_cond_wait(&cond_reader, &lock); //wait for writer
		}

		bad_read(NULL);
		pthread_cond_signal(&cond_writer);
		pthread_mutex_unlock(&lock);
	}
	return NULL; 

	//must include bad_read
}




int main()
{
	// ## SOME SPACE IN CASE YOU NEED TO INITIALISE VARIABLES



	// ################################################################################
	// ## DO NOT MODIFY BELOW HERE

	// ## Parse STDIN and read into commands
	char * commands[100];
	char line[256];
    while (fgets(line, 256, stdin))
    {
		commands[total_commands] = (char*)(malloc(strlen(line) * sizeof(char)));
 		strcpy(commands[total_commands], line);
		total_commands = total_commands + 1;
	}

	pthread_t write1;
	pthread_t read1;

	// Creates a thread which executes writer!
	if(pthread_create(&write1, NULL, writer, commands))
	{
		fprintf(stderr, "Error creating write thread\n");
		return 1;
	}

	// Creates a thread which executes reader!
	if(pthread_create(&read1, NULL, reader, NULL))
	{
		fprintf(stderr, "Error creating read thread\n");
		return 1;
	}

	// Ensure Threads Join Well
	if(pthread_join(write1, NULL)) 
	{
		fprintf(stderr, "Error joining write thread\n");
		return 2;
	}

	if(pthread_join(read1, NULL)) 
	{
		fprintf(stderr, "Error joining read thread\n");
		return 2;
	}

	// Did you really call the right functions?
	get_output();

	// I am a responsible memory user!
	int command_i;
	for (command_i = 0; command_i < total_commands; command_i++)
	{
		free(commands[command_i]);
	}
	return 0;
	// ## DO NOT MODIFY ABOVE HERE
	// ################################################################################
}

