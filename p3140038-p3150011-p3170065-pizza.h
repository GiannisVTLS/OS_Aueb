#pragma once
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int n_tel = 3;            		// Number of call handlers
int n_cook = 2;           		// Number of cooks
int n_oven = 10;          		// Number of ovens
int n_del = 7;            		// Number of deliverers

const int t_order_l = 1;      	// Time range in which a new order arrives
const int t_order_h = 5;

const int n_order_l = 1;      	// Amount range of pizzas ordered
const int n_order_h = 5;

const int t_pay_l = 1;    		// Time range in which the payment is completed
const int t_pay_h = 2;

const int p_fail = 0.05;        // Probability that payment fails

const int c_pizza = 10;         // Pizza cost in Euros

const int t_prep = 1;           // Time required to prepare each pizza
const int t_bake = 10;          // Time required to bake each pizza
const int t_pack = 2;           // Time required to pack each pizza

const int t_del_l = 5;   		// Time range that deliveries take
const int t_del_h = 15;

unsigned int seed;
pthread_mutex_t tel_lock;
pthread_cond_t tel_cond;

pthread_mutex_t cook_lock;
pthread_cond_t cook_cond;

pthread_mutex_t oven_lock;
pthread_cond_t oven_cond;

pthread_mutex_t del_lock;
pthread_cond_t del_cond;

void* pizza_thread(void* cust_id);
void check_rc(int response_code);