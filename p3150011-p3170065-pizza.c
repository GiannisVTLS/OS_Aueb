#include "p3150011-p3170065-pizza.h"

int i, n_cust, failed = 0, success = 0, profits = 0;
double total_service_time = 0, max_service_time = 0, max_waiting_time = 0, total_waiting_time = 0, total_del_time = 0, max_del_time = 0;
unsigned int seed;

pthread_mutex_t tel_lock;
pthread_cond_t tel_cond;

pthread_mutex_t cook_lock;
pthread_cond_t cook_cond;

pthread_mutex_t oven_lock;
pthread_cond_t oven_cond;

pthread_mutex_t del_lock;
pthread_cond_t del_cond;

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

	/* Init thread and mutexes */
	pthread_t threadPool[n_cust];
	pthread_mutex_init(&tel_lock, NULL);
	pthread_cond_init(&tel_cond, NULL);
	pthread_mutex_init(&cook_lock, NULL);
	pthread_cond_init(&cook_cond, NULL);
	pthread_mutex_init(&oven_lock, NULL);
	pthread_cond_init(&oven_cond, NULL);
	pthread_mutex_init(&del_lock, NULL);
	pthread_cond_init(&del_cond, NULL);

	/* Create one thread for each customer that arrives */
	for(i = 0; i < n_cust; i++) {
		order_id[i] = i + 1;
		err = pthread_create(&threadPool[i], NULL, pizza_thread, &order_id[i]);
		if (err != 0) {
            printf("pthread_create error: %d\n", err);
            exit(-1);
        }
		//Sleep for a random time in the time frame random(5-1)+1, so minimum is 1 and maximum is 5
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
	//total waiting time is calculated for all customers
	printf("Average waiting time: %f minutes\nMax waiting time %f minutes\n", (total_waiting_time / n_cust)/60, max_waiting_time/60);
	//total service time is calculated only for customers whose order succeeded
	printf("Average service time: %f minutes\nMax service time %f minutes\n", (total_service_time / (n_cust - failed))/60, max_service_time/60);
	//total delivery time is calculated only for customers whose order succeeded
	printf("Average time of pizzas getting cold: %f minutes\nMax time of pizzas getting cold: %f minutes\n", (total_del_time / (n_cust - failed))/60, max_del_time/60);

	pthread_mutex_destroy(&tel_lock);
	pthread_cond_destroy(&tel_cond);
	pthread_mutex_destroy(&cook_lock);
	pthread_cond_destroy(&cook_cond);
	pthread_mutex_destroy(&oven_lock);
	pthread_cond_destroy(&oven_cond);
	pthread_mutex_destroy(&del_lock);
	pthread_cond_destroy(&del_cond);
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
	rc = pthread_mutex_lock(&tel_lock);
	while (n_tel == 0) {
		rc = pthread_cond_wait(&tel_cond, &tel_lock);
	}
	//Caller was assigned
	clock_gettime(CLOCK_REALTIME, &call_finish);
	n_tel--;

	rc = pthread_mutex_unlock(&tel_lock);
	//Sleep thread while payment is being processed
	sleep(rand_r(&seed) % (t_pay_h - t_pay_l) + t_pay_l);
	
	rc = pthread_mutex_lock(&tel_lock);
	pizza_num = rand_r(&seed) % (n_order_h - n_order_l) + n_order_l;
	n_tel++;
	rc = pthread_cond_signal(&tel_cond);
	rc = pthread_mutex_unlock(&tel_lock);
	
	if((rand_r(&seed) % 100) <= p_fail){
		failed++;
		printf("Order %d failed. Cancelling order...\n", oid);
		pthread_exit(NULL);
	}else{
		success++;
		printf("Order %d succeeded\n", oid);
		profits = profits + c_pizza * pizza_num;
	}

	//Cook handling
	rc = pthread_mutex_lock(&cook_lock);
	while (n_cook == 0) {
		pthread_cond_wait(&cook_cond, &cook_lock);
	}
	n_cook--;

	rc = pthread_mutex_unlock(&cook_lock);
	//Sleep while all the pizzas are being prepared
	sleep(t_prep*pizza_num);

	//Oven handling
	rc = pthread_mutex_lock(&oven_lock);
	while (n_oven < pizza_num) {
		pthread_cond_wait(&oven_cond, &oven_lock);
	}
	n_oven = n_oven - pizza_num;
	
	rc = pthread_mutex_lock(&cook_lock);
	n_cook++;
	
	//notify all threads
	rc = pthread_cond_signal(&cook_cond);
	rc = pthread_mutex_unlock(&cook_lock);

	rc = pthread_mutex_unlock(&oven_lock);
	//all pizzas are in the oven and being baked simultaneously
	sleep(t_bake);
	
	//Need to keep the time it took for cooking to finish, no need to mutex since it doesn't affect any condition
	clock_gettime(CLOCK_REALTIME, &cook_finish);
	
	//Sleep for the time it takes to pack each pizza
	sleep(t_pack * pizza_num);
	
	rc = pthread_mutex_lock(&oven_lock);
	printf("Order %d is packed and ready to go!\n", oid);
	n_oven = n_oven + pizza_num;
	rc = pthread_cond_broadcast(&oven_cond);
	rc = pthread_mutex_unlock(&oven_lock);

	//Delivery handling
	rc = pthread_mutex_lock(&del_lock);
	while (n_del == 0) {
		pthread_cond_wait(&del_cond, &del_lock);
	}
	n_del--;
	del_time = rand_r(&seed) % (t_del_h - t_del_l) + t_del_l;
	
	rc = pthread_mutex_unlock(&del_lock);
	//Sleep for the time it takes to deliver the pizza
	sleep(del_time);
	rc = pthread_mutex_lock(&del_lock);
	printf("Order %d has been delivered!\n", oid);
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
	
	rc = pthread_mutex_unlock(&del_lock);

	//sleep for the time it takes for the delivery driver to return
	sleep(del_time);
	rc = pthread_mutex_lock(&del_lock);
	n_del++;
	rc = pthread_cond_signal(&del_cond);
	rc = pthread_mutex_unlock(&del_lock);
	pthread_exit(NULL);
}