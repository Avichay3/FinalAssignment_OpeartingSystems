#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>

// Function to do modular exponentiation. It returns (base^exponent) % modulus
void power_mod(mpz_t r, const mpz_t base, const mpz_t exponent, const mpz_t modulus) {
    mpz_powm(r, base, exponent, modulus);
}

// Rabin Miller primality test
int is_prime(const mpz_t n, int iterations) {
    if (mpz_cmp_ui(n, 2) < 0) return 0;
    if (mpz_cmp_ui(n, 2) == 0) return 1;
    if (mpz_even_p(n)) return 0;

    mpz_t d, r, a, x, n_minus_1;
    mpz_inits(d, r, a, x, n_minus_1, NULL);
    mpz_sub_ui(n_minus_1, n, 1);
    mpz_set(d, n_minus_1);

    // Find d * 2^r = n - 1
    unsigned long int s = 0;
    while (mpz_even_p(d)) {
        mpz_fdiv_q_2exp(d, d, 1); // d /= 2
        s++;
    }

    gmp_randstate_t randstate;
    gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate, time(NULL));

    for (int i = 0; i < iterations; i++) {
        mpz_urandomm(a, randstate, n_minus_1); // Random a in [2, n-2]
        mpz_add_ui(a, a, 1);
        power_mod(x, a, d, n); // x = a^d % n

        if (mpz_cmp_ui(x, 1) == 0 || mpz_cmp(x, n_minus_1) == 0) continue;

        int continueLoop = 0;
        for (unsigned long int j = 1; j < s; j++) {
            mpz_powm_ui(x, x, 2, n); // x = x^2 % n
            if (mpz_cmp_ui(x, 1) == 0) return 0;
            if (mpz_cmp(x, n_minus_1) == 0) {
                continueLoop = 1;
                break;
            }
        }
        if (!continueLoop) {
            gmp_randclear(randstate);
            mpz_clears(d, r, a, x, n_minus_1, NULL);
            return 0;
        }
    }

    gmp_randclear(randstate);
    mpz_clears(d, r, a, x, n_minus_1, NULL);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1;
    }

    mpz_t n;
    mpz_init(n);
    if (mpz_set_str(n, argv[1], 10) != 0) { // Set string value in base 10
        printf("Invalid number.\n");
        mpz_clear(n);
        return 1;
    }

    int k = 5; // Number of iterations for the Rabin Miller test
    if (is_prime(n, k))
        gmp_printf("%Zd is probably prime.\n", n);
    else
        gmp_printf("%Zd is not prime.\n", n);

    mpz_clear(n);
    return 0;
}
