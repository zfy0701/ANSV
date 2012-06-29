#include "ANSV.h"
#include "Base.h"
#include "cilk.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

using namespace std;

#define LEFT(i) ((i) << 1)
#define RIGHT(i) (((i) << 1) | 1)
#define PARENT(i) ((i) >> 1)

inline int getLeft(int **table, int depth, int n, int index) {
	int value = table[0][index];
	if (value == table[depth - 1][0]) return -1;

	int cur = PARENT(index), d, dist = 2;
	for (d = 1; d < depth; d++) {
		if ((cur + 1) * dist > index + 1) cur --;
		if (cur < 0) return -1;

		if (table[d][cur] >= value) cur = PARENT(cur);
		else break;

		dist <<= 1;
	}

	for ( ; d > 0; d--) {
		if (table[d - 1][RIGHT(cur)] < value) cur = RIGHT(cur);
		else cur = LEFT(cur);
	}
	return cur;
} 

inline int getRight(int **table, int depth, int n, int index) {
	int value = table[0][index];
	if (value == table[depth - 1][0]) return -1;

	int cur = PARENT(index), d, dist = 2;
	for (d = 1; d < depth; d++) {
		if (cur * dist < index) cur ++;
		if (cur * dist >= n) return -1;

		if (table[d][cur] >= value) cur = PARENT(cur);
		else break;

		dist <<= 1;
	}

	for ( ; d > 0; d--) {
		if (table[d - 1][LEFT(cur)] < value) cur = LEFT(cur);
		else cur = RIGHT(cur);
	}
	return cur;
}

void ComputeANSV(int *a, int n, int *left, int *right) {
	int depth = getDepth(n);
	int *all = new int[n * 2];
	int **table = new int*[depth];

	table[0] = all;
	for (int i = 1; i < depth; i++) {
		table[i] = table[i - 1] + (1 << (depth - i));
	}

	cilk_for (int i = 0; i < n; i++) {
		table[0][i] = a[i];
	}

	int m = n;
	for (int d = 1; d < depth; d++) {
		int m2 = m / 2;

		cilk_for (int i = 0; i < m2; i++) {
			table[d][i] = min(table[d - 1][LEFT(i)], table[d - 1][RIGHT(i)]);
		}

		if (m % 2) {
			table[d][m2] = table[d - 1][LEFT(m2)];
		}

		m = (m + 1) / 2;
	}

	cilk_for (int i = 0; i < n; i++) {
		left[i] = getLeft(table, depth, n, i);
		right[i] = getRight(table, depth, n, i);
	}
	delete table;
	delete all;
}

void ComputeANSV_Opt(int * a, int n, int *left, int *right) {
    int l2 = cflog2(n);
    int depth = l2 + 1;

	int *all = new int[n * 2];
	int **table = new int*[depth];

	table[0] = all;
	for (int i = 1; i < depth; i++) {
		table[i] = table[i - 1] + (1 << (depth - i));
	}

	cilk_for (int i = 0; i < n; i++) {
		table[0][i] = a[i];
	}

	int m = n;
	for (int d = 1; d < depth; d++) {
		int m2 = m / 2;

		cilk_for (int i = 0; i < m2; i++) {
			table[d][i] = min(table[d - 1][LEFT(i)], table[d - 1][RIGHT(i)]);
		}

		if (m % 2) {
			table[d][m2] = table[d - 1][LEFT(m2)];
		}

		m = (m + 1) / 2;
	}

    int p = omp_get_max_threads();
    //printf("num of proc %d.\n", p);
    int size = (n + p - 1) / p;

  	cilk_for (int i = 0; i < n; i += size) {
  		int j = min(i + size, n);
  		ComputeANSV_Linear(a + i, j - i, left + i, right + i);

  		int tmp = -1;
  		for (int k = i; k < j; k++) {
  			if (left[k] == -1) {
  				if (tmp != -1 && a[tmp] < a[k]) left[k] = tmp;
				else left[k] = tmp = getLeft(table, depth, n, k);
  			} else left[k] += i;
  		}

  		tmp = -1;
  		for (int k = j - 1; k >=  i; k--) {
  			if (right[k] == -1) {
  				if (tmp != -1 && a[tmp] < a[k]) right[k] = tmp;
  				else right[k] = tmp = getRight(table, depth, n, k);
  			} else right[k] += i;
  		}
  	}

  	delete table;
	delete all;
}

void ComputeANSV_Linear(int a[], int n, int leftElements[], int rightElements[]) {
    int i, top;
    int *stack = new int[n];

    for (i = 0, top = -1; i < n; i++) {
        while (top > -1 && a[stack[top]] > a[i]) top--;
        if (top == -1) leftElements[i] = -1;
        else leftElements[i] = stack[top];
        stack[++top] = i;
    }

    for (i = n - 1, top = -1; i >= 0; i--) {
        while (top > -1 && a[stack[top]] > a[i]) top--;
        if (top == -1) rightElements[i] = -1;
        else rightElements[i] = stack[top];
        stack[++top] = i;
    }
    delete stack;
}
