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
{
	sharedBuff q;
	sharedBuff *consumer = &q;
	outl(0xE4,(uint32_t)(uintptr_t)consumer);

	while (1)
	{
		consumer->prod_p = inl(0xE9);
		consumer->cons_p = inl(0xE8);
		consumer->count=0;
		int n = random_0_10() ;
		for (int i = 0; i < n; i++)
		{
			if (consumer->prod_p == consumer->cons_p)
			{
				break;
			}
			// Consume
			consumer->buff[consumer->cons_p] = -1;
			consumer->count++;
			consumer->cons_p = (consumer->cons_p + 1) % 20;
		}

		outl(0xE5, (uint32_t)(uintptr_t)consumer);
	}

	*(long *)0x400 = 42;
	for (;;)
		asm("hlt" : : "a"(42) : "memory");
}
