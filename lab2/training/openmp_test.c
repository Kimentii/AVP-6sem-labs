#include <stdio.h>
#include <omp.h>

int omp_thread_count() {
    int n = 0;
    #pragma omp parallel reduction(+:n)
    n += 1;
    return n;
}


int    main()
{

omp_set_num_threads(4); //initialise thread count for 4 core cpu        
int this_thread = omp_get_thread_num(), num_threads = omp_get_num_threads();
 
  // int my_start = (this_thread  ) * 10 / num_threads;
 
  // int my_end   = (this_thread+1) * 10 / num_threads;
//int c[10000*10000];
 #pragma omp parallel for
  for(int n=0; n<20; ++n) {
  	printf("%d-%d\n", n, omp_get_thread_num());
  }
 
    


	// #pragma omp parallel for num
	// for(int n=0; n<10; ++n) {
	// 	printf(" %d", n); 
	// }

	printf("\n%d-%d-%d-%d\n", this_thread, num_threads, omp_get_max_threads(), omp_thread_count());
	return 0;
}
