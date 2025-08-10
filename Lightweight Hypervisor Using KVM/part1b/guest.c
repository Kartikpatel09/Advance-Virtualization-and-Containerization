#include <stddef.h>
#include <stdint.h>
char buff1[32], buff2[32];
int flag = 0;


static void outb(uint16_t port, uint8_t value)
{
	asm volatile("outb %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}
static void outl(uint16_t port, uint32_t value)
{
	asm volatile("outl %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}
static uint32_t inl(uint16_t port)
{
	uint32_t value;
	asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}
void HC_print8bit(uint8_t val)
{
	outb(0xE9, val);
}

void HC_print32bit(uint32_t val)
{
	// val++;
	uint32_t *ptr=&val;
	outl(0xE8,(uint32_t)(uintptr_t)ptr);
	
}

uint32_t HC_numExits()
{
	uint32_t val = inl(0xE7);
	return val;
}

void HC_printStr(char *str)
{
	char *ptr=str;
	outl(0xE6,(uint32_t)(uintptr_t)ptr);
	
}

char *HC_numExitsByType()
{
	if (flag)
	{
		outl(0xE5, (uint32_t)(uintptr_t)buff1);
		if(!inl(0xE2)){return NULL;}
		return buff1;
	}
	else
	{
		outl(0xE5, (uint32_t)(uintptr_t)buff2);
		if(!inl(0xE2)){return NULL;}
		flag = 1;
		return buff2;
	}
}

uint32_t HC_gvaToHva(uint32_t gva)
{
	uint32_t hva = 0;
	outl(0xE4, gva);
	hva = inl(0xE3);
	return hva;
}

void
	__attribute__((noreturn))
	__attribute__((section(".start")))
	_start(void)
{
	const char *p;

	for (p = "Hello 695!\n"; *p; ++p)
		HC_print8bit(*p);

	/*----------Don't modify this section. We will use grading script---------*/
	/*---Your submission will fail the testcases if you modify this section---*/
	HC_print32bit(2048);
	HC_print32bit(4294967295);

	uint32_t num_exits_a, num_exits_b;
	num_exits_a = HC_numExits();

	char *str = "CS695 Assignment 2\n";
	HC_printStr(str);

	num_exits_b = HC_numExits();

	HC_print32bit(num_exits_a);
	HC_print32bit(num_exits_b);

	char *firststr = HC_numExitsByType();
	uint32_t hva;
	hva = HC_gvaToHva(1024);
	HC_print32bit(hva);
	hva = HC_gvaToHva(4294967295);
	HC_print32bit(hva);
	char *secondstr = HC_numExitsByType();

	HC_printStr(firststr);
	HC_printStr(secondstr);
	/*------------------------------------------------------------------------*/

	*(long *)0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a"(42) : "memory");
}
