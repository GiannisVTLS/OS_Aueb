#include "p3150011-p3170065-pizza.h"

int i, n_cust, code;
unsigned int seed;
pthread_mutex_t lock;
pthread_mutex_t mutex_rand_r = PTHREAD_MUTEX_INITIALIZER;

void check_mutex(int code)
{
	if (code != 0)
	{
		printf("Error in mutex lock/unlock! Error Code: %d \n", code);
		pthread_exit(&code);
	}
}

int main(int argc, char *argv[]) {
	int code;
	if (argc < 2) {
		printf("Need at least two arguments.\n");
		printf("Exiting...");
		exit(-1);
	}

	if(atoi(argv[1])>0){
		n_cust = atoi(argv[1]);
	}else{
		printf("Need at least 1 customer");
		exit(-1);
	}

	seed = atoi(argv[2]);
	int cust_id[n_cust];
	
	pthread_t thread[n_cust];

	for(i = 0; i<n_cust; i++) {
		cust_id[i] = i + 1;
		pthread_create(&thread[i], NULL, &pizza_thread, &cust_id[i]);
		
		code = pthread_mutex_lock(&mutex_rand_r);
		check_mutex(code);
		
		sleep(rand_r(&seed) % time_order_h + time_order_l);
	}
	
	return 0;
}

void * pizza_thread(void *cust_id) {
	pthread_exit(NULL);
}
