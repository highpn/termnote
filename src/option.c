#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/option.h"
double max(double a, double b) {
    return a > b ? a : b;
}

double binomial_american_option(
    double S,     // Current stock price
    double K,     // Strike price
    double T,     // Time to maturity (years)
    double r,     // Risk-free rate
    double sigma, // Volatility
    int n,        // Steps in binomial tree
    const char* type // "call" or "put"
) {
    double dt = T / n;
    double u = exp(sigma * sqrt(dt));
    double d = 1.0 / u;
    double p = (exp(r * dt) - d) / (u - d);
    double disc = exp(-r * dt);

    // Allocate arrays
    double *option = (double *)malloc((n + 1) * sizeof(double));
    double *stock = (double *)malloc((n + 1) * sizeof(double));

    // Compute option values at maturity
    for (int i = 0; i <= n; ++i) {
        stock[i] = S * pow(u, i) * pow(d, n - i);
        if (strcmp(type, "call") == 0)
            option[i] = max(0.0, stock[i] - K);
        else
            option[i] = max(0.0, K - stock[i]);
    }

    // Backward induction
    for (int step = n - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            stock[i] = S * pow(u, i) * pow(d, step - i);
            double hold = disc * (p * option[i + 1] + (1 - p) * option[i]);
            double exercise = strcmp(type, "call") == 0
                ? max(0.0, stock[i] - K)
                : max(0.0, K - stock[i]);
            option[i] = max(hold, exercise);
        }
    }

    double result = option[0];
    free(option);
    free(stock);
    return result;
}

