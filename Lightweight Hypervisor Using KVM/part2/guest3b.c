#include <stddef.h>
#include <stdint.h>

static void outl(uint16_t port, uint32_t value)
{
	asm volatile("outl %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}

typedef struct {
	int count;
	int buff[5];
} sharedData;

void
	__attribute__((noreturn))
	__attribute__((section(".start")))
	_start(void)
{
	sharedData data;  
	sharedData *q = &data;
	
	
	for (int i = 0; i < 5; i++) {
		q->buff[i] = -1;
	}

	
	for (int i = 0; i < 20; i++) {
		outl(0xE7, (uint32_t)(uintptr_t)q);
		outl(0xE6, (uint32_t)(uintptr_t)q);
	}

	
	*(long *)0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a"(42) : "memory");
}
