#include <stdio.h>

int main() 
{
	double a[2];
	double b[2];
	double c[2];
	double g[2];

	a[0] = 1.5f;
	a[1] = 2.5f;

	b[0] = 1.0f;
	b[1] = 2.0f;

	g[0] = 0.1f;
	g[1] = 0.2f;

	int d = 3;
	int e = 4;
	int f = 0;

	asm volatile(	"movl %1, %%eax; \
			 movl %2, %%ebx; \
			 addl %%ebx, %%eax; \
			 movl %%eax, %0" 
			: "=m"(f) 
			:"r"(d), "r"(e) 
			: "%eax", "%ebx");
	
	printf("%d\n", f);
	c[0] = 0;//a[0] + b[0];
	c[1] = 0;//a[1] + b[1];

	// asm volatile(  "movhps (%0), %%xmm0; \
	// 		movlps 0x8(%0), %%xmm0; \
	// 		movhps (%1), %%xmm1; \
	// 		movlps 0x8(%1), %%xmm1; \
	// 		mulpd %%xmm1, %%xmm0; \
	// 		movups (%3), %%xmm1; \
	// 		addpd %%xmm1, %%xmm0; \
	// 		movhps %%xmm0, (%2); \
	// 		movlps %%xmm0, 0x8(%2) \
	// 		"
	// 		: 
	// 		: "r"(a), "r"(b), "r"(g), "r"(c) 
	// 		: "%xmm0", "%xmm1");

	// asm volatile(  "movhps (%0), %%xmm0; \
	// 		movlps 0x8(%0), %%xmm0; \
	// 		movhps (%1), %%xmm1; \
	// 		movlps 0x8(%1), %%xmm1; \
	// 		mulpd %%xmm1, %%xmm0; \
	// 		movups %2, %%xmm1; \
	// 		addpd %%xmm1, %%xmm0; \
	// 		movhps %%xmm0, (%3); \
	// 		movlps %%xmm0, 0x8(%3)"
	// 		: 
	// 		: "r"(a+1), "r"(b), "r"(0.1f), "r"(c) 
	// 		: "%xmm0", "%xmm1");

	asm volatile(  "movups (%1), %%xmm0; \
						movhps (%2), %%xmm1; \
						movlps (%2), %%xmm1; \
						mulpd %%xmm1, %%xmm0; \
						movups (%0), %%xmm1; \
						addpd %%xmm1, %%xmm0; \
						movups %%xmm0, (%0)"
			: 
			: "r"(a+1), "r"(b), "r"(0.1f), "r"(c) 
			: "%xmm0", "%xmm1");

	printf("c[0] = %F, c[1] = %F\n", c[0], c[1]);
	return 0;
}
