#include <stdio.h>
#include <omp.h>

int main()
{
	int i,j,th;
	omp_set_nested(1);
	#pragma omp parallel for private(i,j,th)
	for (i=0; i<2; i++)
	{
		th = omp_get_thread_num();
		#pragma omp parallel for private(j)
		for(j=0; j<4; j++)
		{
			printf("thread = %d %d, i = %d, j = %d\n", th, omp_get_thread_num(),i,j);
		}
	}
	return 0;
}
