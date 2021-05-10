#include "p3150011-p3170065-pizza.h"

int i, n_cust, code, failed, success;
int profits = 0;
unsigned int seed;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t no_phones = PTHREAD_MUTEX_INITIALIZER;

void check_mutex(int code){
	if (code != 0){
		printf("Mutex error code: %d \n", code);
		pthread_exit(&code);
	}
}

int main(int argc, char *argv[]) {
	
	if (argc < 2) {
		printf("Please provide all arguments.\n");
		printf("Exiting...");
		exit(-1);
	}

	if(atoi(argv[1]) > 0){
		n_cust = atoi(argv[1]);
	}else{
		printf("We need at least 1 customer");
		printf("Exiting...");
		exit(-1);
	}

	seed = atoi(argv[2]);
	
	int max_time = 0;
	int total_time = 0;
	
	cooks_num = N_cook;
    ovens_num = N_oven;
	
	pthread_attr_t attr;
	int order_id[n_cust];
	
	/* Get the deafult attributes */
	pthread_attr_init(&attr)
	pthread_t threadPool[n_cust];
	
	/* Create one thread for each customer that arrives */
	for(i = 0; i < n_cust; i++) {
		cust_id[i] = i + 1;
		err = pthread_create(&threadPool[i], &attr, pizza_thread, &order_id[i]);
		if (err != 0) {
            printf("pthread_create error: %d\n", err);
            exit(-1);
        }
		code = pthread_mutex_lock(&lock);
		check_mutex(code);
		sleep(rand_r(&seed) % (t_order_h - t_order_l) + t_order_l);
	}
	
	/* Join all the threads */
	for(int i = 0; i < N_cust; i++){
        error = pthread_join(threads[i], &status);
        if (error != 0) {
            printf("pthread_join error: %d on thread %d\n", error, order_id[i]);
            exit(-1);
        }
    }
	
	printf("Total profits: %d\n", profits);
	printf("Orders completed successfully: %d\n", success);
	printf("Orders failed: %d\n", failed);
	printf("Average waiting time: %d\n", );
	printf("Average and max service time: %d, %d\n", total_time / N_cust, max_time);
	printf("Average and max time of pizzas getting cold: %d\n", ,);
	return 0;
}

void* pizza_thread(void *cust_id) {
	
	int pizza_num, check_prob;
	int oid = (int) cust_id;
	struct timespec order_start;
	struct timespec order_finish;
	struct timespec order_prepare;
    struct timespec order_bake;
	
	clock_gettime(CLOCK_REALTIME, &order_start);
	
	//Call handling
	while (n_tel == 0) {
		pthread_cond_wait(&cond, &lock);
	}
	
	n_tel--;
	
	code = pthread_mutex_lock(&lock);
	check_mutex(code);
	
	pizza_num = rand_r(&seed) % (n_order_h - n_order_l) + n_order_l;
	sleep(rand_r(&seed) % (t_pay_h - t_pay_l) + t_pay_l);
	
	n_tel++;
	
	if((rand_r(&seed) % 100) <= p_fail * 100){
		failed++;
		printf("Order with OID %d failed\n Cancelling order...", oid);
		pthread_exit(NULL);
	}else{
		printf("Order with OID %d succeeded\n Finding available cooks...", oid);
		income = income + c_pizza * pizza_num;
	}
	
	//Cook handling
	while (n_cook == 0) {
		pthread_cond_wait(&cond, &lock);
	}
	cooks_num--;
	
	//Oven handling
	while (n_oven == 0) {
		pthread_cond_wait(&cond, &lock);
	}
	
	//Delivery handling
	while (n_del == 0) {
		pthread_cond_wait(&cond, &lock);
	}
	
	order_time = ((order_stop.tv_sec - order_bake.tv_sec) + (order_bake.tv_sec - order_prep.tv_sec) + (order_prep.tv_sec - order_start.tv_sec));
    total_order_time += order_time;
	
	success++;
	pthread_exit(NULL);
}
