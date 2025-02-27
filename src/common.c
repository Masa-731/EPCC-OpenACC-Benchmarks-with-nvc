/* Copyright (c) 2013 The University of Edinburgh. */

/* Licensed under the Apache License, Version 2.0 (the "License"); */
/* you may not use this file except in compliance with the License. */
/* You may obtain a copy of the License at */

/*     http://www.apache.org/licenses/LICENSE-2.0 */

/* Unless required by applicable law or agreed to in writing, software */
/* distributed under the License is distributed on an "AS IS" BASIS, */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/* See the License for the specific language governing permissions and */
/* limitations under the License. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <openacc.h>
#include "common.h"

unsigned int datasize = -1;     /* Datasize for test in bytes. */
int reps = -1;                  /* Repetitions. */

double *times;                  /* Array of doubles storing the benchmark times in microseconds. */
double testtime;                /* The average test time in microseconds for reps runs. */
double testsd;                  /* The standard deviation in the test time in microseconds for reops runs. */
int flag = 0;                   /* 0 indicates CPU. */

void usage(char **argv) {
    printf("Usage: %s \n"
            "\t--reps <repetitions> (default %d)\n"
            "\t--datasize <datasize> *default %d bytes)\n",
            argv[0],
            DEFAULT_REPS, DEFAULT_DATASIZE);
}


/*
 * This function parses the parameters from the command line.
 */
void parse_args(int argc, char **argv) {
    int arg;
    for (arg = 1; arg < argc; arg++) {

        if (strcmp(argv[arg], "--reps") == 0) {
            reps = atoi(argv[++arg]);
            if (reps == 0) {
                fprintf(stderr, "Invalid integer:--reps: %s\n", argv[arg]);
                usage(argv);
                exit(EXIT_FAILURE);
            }

        } else if (strcmp(argv[arg], "--datasize") == 0) {
            datasize == atoi(argv[++arg]);
            if (datasize == 0) {
                fprintf(stderr, "Invalid integer:--datasize: %s\n", argv[arg]);
                usage(argv);
                exit(EXIT_FAILURE);
            }

        } else if (strcmp(argv[arg], "-h") == 0) {
            usage(argv);
            exit(EXIT_SUCCESS);

        } else {
            fprintf(stderr, "Invalid parameters: %s\n", argv[arg]);
            usage(argv);
            exit(EXIT_FAILURE);
        }
    }
}


void stats(double *mtp, double *sdp) {
    double meantime, totaltime, sumsq, mintime, maxtime, sd;
    int i, good_reps;

    mintime = 1.0e+10;
    maxtime = 0.0;
    totaltime = 0.0;
    good_reps = 0;

    for (i = 0; i < reps; i++) {
        /* Skip entries where times is 0, this indicates an error occured */
        if (times[i] != 0) {
            mintime = (mintime < times[i]) ? mintime : times[i];
            maxtime = (maxtime > times[i]) ? maxtime : times[i];
            totaltime += times[i];
            good_reps++;
        }
    }

    meantime = totaltime / good_reps;
    sumsq = 0;

    for (i = 0; i < reps; i++) {
        if (times[i] != 0) {
            sumsq += (times[i] - meantime) * (times[i] - meantime);
        }
    }
    sd = sqrt(sumsq / good_reps);

    *mtp = meantime;
    *sdp = sd;
}


/*
 * This function prints the results of the tests.
 * If you use a compiler which sets a different preprocessor flag,
 * you may wish to add it here.
 */
void print_results(char *name, double testtime, double tested) {
    char compiler[20];

    /* Set default compiler identifier. */
    sprintf(compiler, "COMPILER");

    /* Set compiler identifier based on known preprocessor flags. */
#ifdef __NVCOMPILER
    sprintf(compiler, "NVC");
#elif __clang__
    sprintf(compiler, "CLANG");
#elif __GNUC__
    sprintf(compiler, "GCC");
#endif

    printf("%7.7s\t%20.20s\t\t%d\t\t%.6f\t\t%.6f\n", compiler, name, datasize, testtime * 1e+6, CONF95 * testsd * 1e+6);
}


/*
 * This function initializes the storage for the test results and set the defaults.
 */
void init(int argc, char **argv) {
    parse_args(argc, argv);

    if (reps == -1) {
        reps = DEFAULT_REPS;
    }

    if (datasize == (unsigned int)-1) {
        datasize = DEFAULT_DATASIZE;
    }

    times = (double *)malloc((reps) * sizeof(double));


    /*
     * Alternative `wul()` device wake up functions.
     */
    /*
#ifdef __NVCOMPILER
    acc_init(acc_device_nvidia);
    printf("NVC INIT\n");
#elif __clang__
    int a[5] = {1, 2, 3, 4, 5};
#pragma acc data copyin(a[0:5])
{}
#elif __GNUC__
    int a[5] = {1, 2, 3, 4, 5};
#pragma acc data copyin(a[0:5])
{}
#endif
    */
}

void finalize(void) {
    free(times);
}


/*
 * This function runs the benchmark specified.
 */
void benchmark(char *name, double (*test)(void)) {
    int i = 0;
    double tmp = 0;

    for (i = 0; i < reps; i++) {
        tmp = test();
        if (tmp == -10000) {
            fprintf(stderr, "Memory allocation failure in %s\n", name);
            times[i] = 0;
        }
        else if (tmp == -11000) {
            fprintf(stderr, "CPU/GPU mismatch in %s\n", name);
            times[i] = 0;
        }
        else {
            times[i] = tmp;
        }
    }

    stats(&testtime, &testsd);
    print_results(name, testtime, testsd);
}