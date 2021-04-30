#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

const int N_tel = 3;            // Number of call handlers
const int N_cook = 2;           // Number of cooks
const int N_oven = 10;          // Number of ovens
const int N_deliveres = 7;      // Number of deliverers

const int T_order_low = 1*60;      // Time range in which a new order arrives
const int T_order_high = 5*60;

const int N_order_low = 1;      // Amount range of pizzas ordered
const int N_order_high = 5;

const int T_payment_low = 1*60;    // Time range in which the payment is completed
const int T_payment_high = 2*60;

const int P_fail = 0.05;        // Probability that payment fails

const int C_pizza = 10;         // Pizza cost in Euros

const int T_prep = 1*60;           // Time required to prepare each pizza
const int T_bake = 10*60;          // Time required to bake each pizza
const int T_pack = 2*60;           // Time required to pack each pizza

const int T_delivery_low = 5*60;   // Time range that deliveries take
const int T_delivery_low = 15*60;
