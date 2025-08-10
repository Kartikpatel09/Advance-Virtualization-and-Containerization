#include <stddef.h>
#include <stdint.h>
static void outl(uint16_t port, uint32_t value)
{
	asm("outl %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}
void
	__attribute__((noreturn))
	__attribute__((section(".start")))
	_start(void)
{
	int arr[5];
	int j=0;
	while(1){
		for(int i=0;i<5;i++){
			arr[i]=j;
			j++;
		}
		outl(0xE9, (uint32_t)(uintptr_t)arr);
	}
	/* write code here */

	*(long *)0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a"(42) : "memory");
}
