#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
  Compute binomial coefficient C(n, k).
  Returns 0 on invalid input or overflow risk not handled here.
*/
static size_t binomial_size_t(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;

    if (k > n - k) k = n - k;

    size_t res = 1;
    for (int i = 1; i <= k; i++) {
        res = (res * (size_t)(n - k + i)) / (size_t)i;
    }
    return res;
}

/*
  Generate all non-decreasing vectors of length k
  with values in {0, ..., n}.

  Output:
    - *out_count = number of vectors
    - returns pointer to contiguous memory of size (*out_count * k)

  Access vector i, coordinate j as:
    data[i * k + j]

*/
int *generate_vectors_flat(int k, int n, size_t *out_count) {
    if (k <= 0 || n < 0 || out_count == NULL) {
        return NULL;
    }

    size_t count = binomial_size_t(n + k, k);
    *out_count = count;

    int *data = malloc(count * (size_t)k * sizeof(int));
    if (!data) {
        return NULL;
    }

    int *v = malloc((size_t)k * sizeof(int));
    if (!v) {
        free(data);
        return NULL;
    }

    for (int i = 0; i < k; i++) {
        v[i] = 0;
    }

    size_t idx = 0;

    while (1) {
        // Store current vector into flat array
        for (int j = 0; j < k; j++) {
            data[idx * (size_t)k + (size_t)j] = v[j];
        }
        idx++;

        // Find rightmost position that can be incremented
        int i;
        for (i = k - 1; i >= 0; i--) {
            if (v[i] < n) {
                break;
            }
        }

        // Finished
        if (i < 0) {
            break;
        }

        // Increment and propagate to keep non-decreasing order
        v[i]++;
        for (int j = i + 1; j < k; j++) {
            v[j] = v[i];
        }
    }

    free(v);
    return data;
}

void print_vectors_flat(const int *data, size_t count, int k) {
    for (size_t i = 0; i < count; i++) {
        for (int j = 0; j < k; j++) {
            printf("%d ", data[i * (size_t)k + (size_t)j]);
        }
        printf("\n");
    }
}

