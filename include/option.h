

double binomial_american_option(
    double S,     // Current stock price
    double K,     // Strike price
    double T,     // Time to maturity (years)
    double r,     // Risk-free rate
    double sigma, // Volatility
    int n,        // Steps in binomial tree
    const char* type // "call" or "put"
);
