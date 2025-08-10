#include <stddef.h>
#include <stdint.h>

typedef struct
{
	int buff[20];
	int prod_p;
	int cons_p;
	int count;

} sharedBuff;

static inline uint64_t rdtsc(void)
{
	uint32_t lo, hi;
	asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64_t)hi << 32) | lo;
}

static void outl(uint16_t port, uint32_t value)
{
	asm volatile("outl %0,%1" : : "a"(value), "Nd"(port) : "memory");
}

static uint32_t inl(uint16_t port)
{
	uint32_t value;
	asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

int random_0_10()
{
	return (int)(rdtsc() % 11);
}

void __attribute__((noreturn)) __attribute__((section(".start"))) _start(void)
{	sharedBuff p;
	sharedBuff *producer = &p;
	outl(0xE6,(uint32_t)(uintptr_t)producer);


	while (1)
	{
		producer->prod_p = inl(0xE9);
		producer->cons_p = inl(0xE8);
		producer->count=0;
		int n = random_0_10();
		
		for (int i = 0; i < n; i++)
		{
			if ((producer->prod_p + 1) % 20 == producer->cons_p)
			{
				break; 
			}
			
			producer->buff[producer->prod_p] = random_0_10();
			producer->count++;
			producer->prod_p = (producer->prod_p + 1) % 20;
			
			
			
		}

		
		outl(0xE7, (uint32_t)(uintptr_t)producer);
	}

	*(long *)0x400 = 42;
	for (;;)
		asm("hlt" : : "a"(42) : "memory");
}
