#include "ANSV.h"
#include "cilk.h"
#include "gettime.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>

void show(int *a, int n) {
	printf("resut:\n");
	for (int i = 0; i < n; i++) {
		printf("%d\t", a[i]);
	}
	printf("\n");
}

void compare(int *a, int *b, int n) {
	for (int i = 0; i < n; i++) {
		if (a[i] != b[i]) {
			printf("Result incorrect! %d %d %d\n", i, a[i],b[i]);
			return;
		}
	}
	printf("Result correct.\n");
}

void Usage(char *program) {
	printf("Usage: %s [options]\n", program);
	printf("-p <num>\tNumber of processors to use\n");
	printf("-d <num>\t2^n of character will be processed\n");
	printf("-r [0/1]\twheher or not to random data will be generated [otherwise it will be just ordered data]\n");
	printf("-h \t\tDisplay this help\n");
}

char* itoa(int val, int base = 10){
        static char buf[32] = {0};
        int i = 30;
        for(; val && i ; --i, val /= base)
                buf[i] = "0123456789abcdef"[val % base];
        return &buf[i+1];
}

int main (int argc, char *argv[]) {
	int n = 1 << 24;
	int * a = new int[n];
	int opt;
	int random = 1;

	while ((opt = getopt(argc, argv, "p:d:r:i:o:")) != -1) {
		switch (opt) {
			case 'p': {
				int p = atoi(optarg);
				#ifdef OPENMP
				omp_set_num_threads(p);
				#elif CILK
				__cilkrts_set_param("nworkers", itoa(p));
				#endif
				break;
			}
			case 'd': {
				int d = atoi(optarg);
				n = 1 << d;
				break;
			}
			case 'r': {
				random = atoi(optarg);
				break;
			}
			default: {
				Usage(argv[0]);
				exit(1);
			}
		}
	}

	int *l1 = new int[n], *l2 = new int[n];
	int *r1 = new int[n], *r2 = new int[n];

	//generate random data, no duplicates
	for (int i = 0; i < n; i++) {
		a[i] = i;
	}
	if (random == 1) {
		for (int i = 0; i < n; i++) {
			int j = rand() % n;
		 	std::swap(a[i], a[j]);
		}
	}

	//show(a, n);

	startTime();
	ComputeANSV_Linear(a, n, l1, r1);
	nextTime("Linear stack\t");
	//show(l1, n);

	startTime();
	ComputeANSV(a, n, l2, r2);
	nextTime("nlogn parallel\t");

	//show(l2, n);

	compare(l1, l2, n);

	startTime();
	ComputeANSV_Opt(a, n, l2, r2);
	nextTime("test parallel\t");

	compare(l1, l2, n);
	//show(l2, n);

	delete a;
	delete l1; delete l2; delete r1; delete r2;
	return 0;
}
