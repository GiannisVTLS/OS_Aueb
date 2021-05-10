#include "p3150011-p3170065-pizza.h"

int i, n_cust, failed = 0, success = 0, profits = 0;
double total_service_time = 0, max_service_time = 0, max_waiting_time = 0, total_waiting_time = 0, total_del_time = 0, max_del_time = 0;
unsigned int seed;

pthread_mutex_t lock;
pthread_cond_t cond;

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
	
	int err, error;
	
	int order_id[n_cust];

	/* Get the default attributes */
	pthread_t threadPool[n_cust];
	
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond, NULL);
	/* Create one thread for each customer that arrives */
	for(i = 0; i < n_cust; i++) {
		order_id[i] = i + 1;
		err = pthread_create(&threadPool[i], NULL, pizza_thread, &order_id[i]);
		if (err != 0) {
            printf("pthread_create error: %d\n", err);
            exit(-1);
        }
		sleep(rand_r(&seed) % (t_order_h - t_order_l) + t_order_l);
	}
	
	/* Join all the threads */
	for(int i = 0; i < n_cust; i++){
        error = pthread_join(threadPool[i], NULL);
        if (error != 0) {
            printf("pthread_join error: %d on thread %d\n", error, order_id[i]);
            exit(-1);
        }
    }
	printf("Total profits: %d\n", profits);
	printf("Orders completed successfully: %d\nOrders failed: %d\n", success, failed);
	printf("Average waiting time: %f minutes\nMax waiting time %f minutes\n", (total_waiting_time / n_cust)/60, max_waiting_time);
	printf("Average service time: %f minutes\nMax service time %f minutes\n", (total_service_time / (n_cust - failed))/60, max_service_time);
	printf("Average time of pizzas getting cold: %f minutes\nMax time of pizzas getting cold: %f minutes\n", (total_del_time / (n_cust - failed))/60, max_del_time);
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
	return 0;
}

void* pizza_thread(void *order_id) {
	
	int pizza_num;
	int oid = *(int *)order_id;
	int rc;
	double del_time;
	struct timespec order_start;
	struct timespec order_finish;
	struct timespec call_finish;
	struct timespec cook_finish;
	
	clock_gettime(CLOCK_REALTIME, &order_start);
	
	//Call handling
	rc = pthread_mutex_lock(&lock);
	while (n_tel == 0) {
		rc = pthread_cond_wait(&cond, &lock);
	}
	clock_gettime(CLOCK_REALTIME, &call_finish);
	n_tel--;
	rc = pthread_cond_signal(&cond);
	rc = pthread_mutex_unlock(&lock);
	
	sleep(rand_r(&seed) % (t_pay_h - t_pay_l) + t_pay_l);
	
	rc = pthread_mutex_lock(&lock);
	pizza_num = rand_r(&seed) % (n_order_h - n_order_l) + n_order_l;
	if((rand_r(&seed) % 100) <= p_fail * 100){
		failed++;
		printf("Order with OID %d failed. Cancelling order...\n", oid);
		pthread_exit(NULL);
	}else{
		success++;
		printf("Order with OID %d succeeded\n", oid);
		profits = profits + c_pizza * pizza_num;
	}
	n_tel++;
	//Cook handling
	while (n_cook == 0) {
		pthread_cond_wait(&cond, &lock);
	}
	n_cook--;
	rc = pthread_cond_signal(&cond);
	rc = pthread_mutex_unlock(&lock);
	sleep(t_prep*pizza_num);
	
	rc = pthread_mutex_lock(&lock);
	//Oven handling
	while (n_oven < pizza_num) {
		pthread_cond_wait(&cond, &lock);
	}
	n_oven = n_oven - pizza_num;
	n_cook++;
	
	rc = pthread_cond_broadcast(&cond);
	rc = pthread_mutex_unlock(&lock);
	sleep(t_bake);
	
	rc = pthread_mutex_lock(&lock);
	clock_gettime(CLOCK_REALTIME, &cook_finish);
	rc = pthread_cond_broadcast(&cond);
	rc = pthread_mutex_unlock(&lock);
	
	sleep(t_pack * pizza_num);
	
	rc = pthread_mutex_lock(&lock);
	printf("Order %d is packed and ready to go!\n", oid);
	n_oven = n_oven + pizza_num;
	//Delivery handling
	while (n_del == 0) {
		printf("No delivery drivers available for order %d, waiting...\n", oid);
		pthread_cond_wait(&cond, &lock);
	}
	n_del--;
	
	del_time = rand_r(&seed) % (t_del_h - t_del_l) + t_del_l;
	
	rc = pthread_cond_signal(&cond);
	rc = pthread_mutex_unlock(&lock);
	sleep(del_time);
	rc = pthread_mutex_lock(&lock);
	printf("Order %d has beed delivered!\n", oid);
	n_del++;
	clock_gettime(CLOCK_REALTIME, &order_finish);
	
	total_service_time += order_finish.tv_sec - order_start.tv_sec;
	total_waiting_time += call_finish.tv_sec - order_start.tv_sec;
	total_del_time += order_finish.tv_sec - cook_finish.tv_sec;
	
	if(order_finish.tv_sec - order_start.tv_sec > max_service_time){
		max_service_time = total_service_time;
	}
	
	if(call_finish.tv_sec - order_start.tv_sec > max_waiting_time){
		max_waiting_time = total_waiting_time;
	}
	
	if(order_finish.tv_sec - cook_finish.tv_sec > max_del_time){
		max_del_time = total_del_time;
	}
	
	rc = pthread_cond_signal(&cond);
	rc = pthread_mutex_unlock(&lock);
	sleep(del_time);
	pthread_exit(NULL);
}