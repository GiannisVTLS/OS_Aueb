#include "p3150011-p3170065-pizza.h"

int i, n_cust, code;
int income = 0;
unsigned int seed;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_MUTEX_INITIALIZER;

void check_mutex(int code)
{
	if (code != 0)
	{
		printf("Error Code: %d \n", code);
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
		
		code = pthread_mutex_lock(&lock);
		check_mutex(code);
		
		sleep(rand_r(&seed) % (time_order_h - time_order_l) + time_order_l);
	}
	
	return 0;
}

void * pizza_thread(void *cust_id) {
	int pizza_num;
	int check_prob;
	int oid = (int) cust_id;
	struct timespec order_start;
	struct timespec order_finish;
	
	clock_gettime(CLOCK_REALTIME, &order_start);
	
	//Call handling
	while (n_tel == 0) {
		pthread_cond_wait(&cond, &lock);
	}
	n_tell--;
	
	code = pthread_mutex_lock(&lock);
	check_mutex(code);
	
	pizza_num = rand_r(&seed) % (n_order_h - n_order_l) + n_order_l;
	sleep(rand_r(&seed) % (time_pay_h - time_pay_l) + time_pay_l);
	
	check_prob = rand_r(&seed) % 100;
	n_tel++;
	if(check_prob <= p_fail*100){
		printf("Order with OID %d failed\n Cancelling order...", oid)
		pthread_exit(NULL);
	}else{
		printf("Order with OID %d succeeded\n Finding available cooks...", oid)
		income = income + c_pizza * pizza_num;
	}
	
	//Cook handling
	while (n_cook == 0) {
		pthread_cond_wait(&cond, &lock);
	}
	
	//Oven handling
	while (n_oven == 0) {
		pthread_cond_wait(&cond, &lock);
	}
	
	//Delivery handling
	while (n_del == 0) {
		pthread_cond_wait(&cond, &lock);
	}
	
	pthread_exit(NULL);
}
