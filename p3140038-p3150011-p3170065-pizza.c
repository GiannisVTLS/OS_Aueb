#include "p3140038-p3150011-p3170065-pizza.h"

int i, n_cust, failed = 0, success = 0, profits = 0;
int total_service_time = 0, max_service_time = 0, max_waiting_time = 0, total_waiting_time = 0, total_del_time = 0, max_del_time = 0;



void check_rc(int response_code)
{
	if (response_code != 0)
	{
		printf("Error Code: %d\n", response_code);

	pthread_exit(&response_code);
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
	
	int err;
	
	int order_id[n_cust];

	/* Init thread*/
	pthread_t *threadPool;
    threadPool = malloc(n_cust*sizeof(pthread_t));
    if(threadPool==NULL){
        printf("Error allocating memory.\n");
        exit(-1);
    }
	/*Init cond and mutexes */
	pthread_mutex_init(&tel_lock, NULL);
	pthread_cond_init(&tel_cond, NULL);
	pthread_mutex_init(&cook_lock, NULL);
	pthread_cond_init(&cook_cond, NULL);
	pthread_mutex_init(&oven_lock, NULL);
	pthread_cond_init(&oven_cond, NULL);
	pthread_mutex_init(&pack_lock, NULL);
	pthread_cond_init(&pack_cond, NULL);
	pthread_mutex_init(&del_lock, NULL);
	pthread_cond_init(&del_cond, NULL);
	pthread_mutex_init(&time_lock, NULL);

	/* Create one thread for each customer that arrives */
	for(i = 0; i < n_cust; i++) {
		order_id[i] = i + 1;
		err = pthread_create(&threadPool[i], NULL, pizza_thread, &order_id[i]);
		if (err != 0) {
            printf("pthread_create error: %d\n", err);
            exit(-1);
        }
		//Sleep for a random time in the time frame random(5-1)+1, so minimum is 1 and maximum is 5
		seed = atoi(argv[2]);
		sleep(rand_r(&seed) % (t_order_h - t_order_l) + t_order_l);
	}
	
	/* Join all the threads */
	for(i = 0; i < n_cust; i++){
        err = pthread_join(threadPool[i], NULL);
        if (err != 0) {
            printf("pthread_join error: %d\n", err);
            exit(-1);
        }
    }
	printf("Total profits: %d\n", profits);
	printf("Orders completed successfully: %d\nOrders failed: %d\n", success, failed);
	//total waiting time is calculated for all customers
	printf("Average waiting time: %d minutes\nMax waiting time %d minutes\n", (total_waiting_time / n_cust), max_waiting_time);
	//total service time is calculated only for customers whose order succeeded
	printf("Average service time: %d minutes\nMax service time %d minutes\n", (total_service_time / (n_cust - failed)), max_service_time);
	//total delivery time is calculated only for customers whose order succeeded
	printf("Average time of pizzas getting cold: %d minutes\nMax time of pizzas getting cold: %d minutes\n", (total_del_time / (n_cust - failed)), max_del_time);

	pthread_mutex_destroy(&tel_lock);
	pthread_cond_destroy(&tel_cond);
	pthread_mutex_destroy(&cook_lock);
	pthread_cond_destroy(&cook_cond);
	pthread_mutex_destroy(&oven_lock);
	pthread_cond_destroy(&oven_cond);
	pthread_mutex_destroy(&pack_lock);
	pthread_cond_destroy(&pack_cond);
	pthread_mutex_destroy(&del_lock);
	pthread_cond_destroy(&del_cond);
	free(threadPool);
	return 0;
}

void* pizza_thread(void *order_id) {
	
	int pizza_num;
	int oid = *(int *)order_id;
	unsigned int seed_thr = seed*oid;
	int rc;
	int ready_time;
	int del_time;
	int complete_time;
	struct timespec order_start;
	struct timespec order_finish;
	struct timespec call_finish;
	struct timespec cook_finish;
	
	clock_gettime(CLOCK_REALTIME, &order_start);
	
	//Call handling
	rc = pthread_mutex_lock(&tel_lock);
	check_rc(rc);
	while (n_tel == 0) {
		rc = pthread_cond_wait(&tel_cond, &tel_lock);
		check_rc(rc);
	}
	//Caller was assigned
	clock_gettime(CLOCK_REALTIME, &call_finish);
	n_tel--;

	rc = pthread_mutex_unlock(&tel_lock);
	check_rc(rc);

	//Sleep thread while payment is being processed
	sleep(rand_r(&seed_thr) % (t_pay_h - t_pay_l) + t_pay_l);
	
	rc = pthread_mutex_lock(&tel_lock);
	check_rc(rc);
	
	pizza_num = rand_r(&seed_thr) % (n_order_h - n_order_l) + n_order_l;
	n_tel++;
	
	pthread_cond_signal(&tel_cond);
	rc = pthread_mutex_unlock(&tel_lock);
	check_rc(rc);
	
	//Check probability of order failing
	if((rand_r(&seed_thr) % 100) <= p_fail){
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
	check_rc(rc);
	while (n_cook == 0) {
		rc = pthread_cond_wait(&cook_cond, &cook_lock);
		check_rc(rc);
	}
	n_cook--;

	rc = pthread_mutex_unlock(&cook_lock);
	check_rc(rc);

	//Sleep while all the pizzas are being prepared
	sleep(t_prep*pizza_num);

	//Oven handling
	rc = pthread_mutex_lock(&oven_lock);
	check_rc(rc);
	while (n_oven < pizza_num) {
		rc = pthread_cond_wait(&oven_cond, &oven_lock);
		check_rc(rc);
	}
	n_oven = n_oven - pizza_num;
	
	rc = pthread_mutex_lock(&cook_lock);
	check_rc(rc);
	n_cook++;
	
	//notify all threads
	pthread_cond_signal(&cook_cond);
	rc = pthread_mutex_unlock(&cook_lock);
	check_rc(rc);

	rc = pthread_mutex_unlock(&oven_lock);
	check_rc(rc);
	//all pizzas are in the oven and being baked simultaneously
	sleep(t_bake);
	
	//Need to keep the time it took for cooking to finish, no need to mutex since it doesn't affect any condition
	clock_gettime(CLOCK_REALTIME, &cook_finish);
	
	rc = pthread_mutex_lock(&pack_lock);
	check_rc(rc);
	while (n_pack == 0) {
		rc = pthread_cond_wait(&pack_cond, &pack_lock);
		check_rc(rc);
	}
	n_pack--;

	rc = pthread_mutex_unlock(&pack_lock);
	check_rc(rc);
	//Sleep for the time it takes to pack each pizza
	sleep(t_pack);
	
	rc = pthread_mutex_lock(&pack_lock);
	check_rc(rc);
	rc = pthread_mutex_lock(&oven_lock);
	check_rc(rc);
	
	ready_time = cook_finish.tv_sec - order_start.tv_sec;
	printf("Order %d is packed and ready to go! Time: %d minutes\n", oid, ready_time);
	n_oven = n_oven + pizza_num;
	n_pack++;
	pthread_cond_signal(&pack_cond);
	rc = pthread_mutex_unlock(&pack_lock);
	check_rc(rc);
	pthread_cond_broadcast(&oven_cond);
	rc = pthread_mutex_unlock(&oven_lock);
	check_rc(rc);

	//Delivery handling
	rc = pthread_mutex_lock(&del_lock);
	check_rc(rc);
	while (n_del == 0) {
		rc = pthread_cond_wait(&del_cond, &del_lock);
		check_rc(rc);
	}
	n_del--;
	del_time = rand_r(&seed_thr) % (t_del_h - t_del_l) + t_del_l;
	
	rc = pthread_mutex_unlock(&del_lock);
	check_rc(rc);

	//Sleep for the time it takes to deliver the pizza
	sleep(del_time);
	rc = pthread_mutex_lock(&del_lock);
	check_rc(rc);

	clock_gettime(CLOCK_REALTIME, &order_finish);
	complete_time = order_finish.tv_sec - order_start.tv_sec;
	printf("Order %d has been delivered! Time: %d minutes\n", oid, complete_time);
	rc = pthread_mutex_unlock(&del_lock);
	check_rc(rc);
	
	rc = pthread_mutex_lock(&time_lock);
	check_rc(rc);
	total_service_time += complete_time;
	total_waiting_time += call_finish.tv_sec - order_start.tv_sec;
	total_del_time += order_finish.tv_sec - cook_finish.tv_sec;
	
	if(complete_time > max_service_time){
		max_service_time = complete_time;
	}
	
	if(call_finish.tv_sec - order_start.tv_sec > max_waiting_time){
		max_waiting_time = call_finish.tv_sec - order_start.tv_sec;
	}
	
	if(order_finish.tv_sec - cook_finish.tv_sec > max_del_time){
		max_del_time = order_finish.tv_sec - cook_finish.tv_sec;
	}
	rc = pthread_mutex_unlock(&time_lock);
	check_rc(rc);
	
	//sleep for the time it takes for the delivery driver to return
	sleep(del_time);
	rc = pthread_mutex_lock(&del_lock);
	check_rc(rc);
	n_del++;
	pthread_cond_signal(&del_cond);
	rc = pthread_mutex_unlock(&del_lock);
	check_rc(rc);

	pthread_exit(NULL);
}