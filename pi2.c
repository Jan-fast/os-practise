#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define NR_TOTAL 9999

struct param {
	int start;
	int end;
};

struct result {
	double sum;
};

void *compute(void *arg)
{
	struct param *param;
	struct result *result;
	double sum = 0, num = 0, n;
	int i;

	param = (struct param *)arg;
	for (i = param->start; i<param->end; i++)
	{
		n = (double)i;
		num = pow(-1, n) * (1 / (2*n+1));
		sum += num;
	}

	result = malloc(sizeof(struct result));
	result->sum = sum;
	return result;
}

int main()
{
	int NR_CPU = 60, NR_CHILD;
	printf("Number of thread:");
	while (NR_CPU > 59 || NR_CPU < 1)
	{
		scanf("%d", &NR_CPU);
	}
	NR_CHILD = NR_TOTAL / NR_CPU;

	pthread_t workers[NR_CPU];
	struct param params[NR_CPU];

	int i;
	for (i=0; i< NR_CPU; i++)
	{
		struct param *param;
		param = &params[i];
		param->start = i * NR_CHILD;
		param->end = (i+1) * NR_CHILD;
		pthread_create(&workers[i], NULL, compute, param);
	}

	double sum = 0;
	for (i=0; i< NR_CPU; i++)
	{
		struct result *result;
		pthread_join(workers[i], (void **)&result);
		sum += result->sum;
		free(result);
	}


	return 0;
}

