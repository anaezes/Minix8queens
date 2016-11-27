/* Keyboard functions */

#include "../lib/Keyboard.h"

int special = 0;
unsigned int kbc_kb_hook_id = KB_IRQ;

void kb_print_scancode(unsigned long scancode, int n_bytes)
{
	if(special == 0)
	{
		if(is_break_code(scancode))
		{
			if(n_bytes == 2)
				special = 1;
			else
				printf("Breakcode: 0x%02X\n\n", scancode);
		}
		else
		{
			if(n_bytes == 2)
				special = 1;
			else
				printf("Makecode: 0x%02X\n", scancode);
		}
	}
	else
	{
		if(is_break_code(scancode))
		{
			printf("Breakcode: 0xE0%02X \n\n", scancode);
			special = 0;
		}
		else
		{
			printf("Makecode: 0xE0%02X \n", scancode);
			special = 0;
		}
	}


}

// subscribes and enables keyboard interrupts
int kb_subscribe_int(void)
{
	// subscribes to interrupts IRQ 1
	unsigned int tmp = kbc_kb_hook_id;

	if(sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_kb_hook_id) == OK)
		if(sys_irqenable(&kbc_kb_hook_id) == OK)
			return BIT(tmp);
	printf("Error while subscribing to kb interruptions\n");
	return -1;
}

// unsubscribes keyboard interrupts
int kb_unsubscribe_int()
{
	if(sys_irqrmpolicy(&kbc_kb_hook_id) == OK)
		if(sys_irqdisable(&kbc_kb_hook_id) == OK)
			return 0;

	return 1;
}



// Read scancodes from KBC_OUT_BUFFER
// store in fst_byte the scancode
// returns 1 if it's 1 byte scancode
// or 2 if it's 2 byte scancode
int kb_int_handler(unsigned long *scancode)
{
	if(kbc_read_input(scancode)==0)
		if(kb_is_special_scancode(*scancode))
			return 2;
		else
			return 1;

	return -1;
}

int kb_int_handler_asm(unsigned long scancode)
{
	if(kb_is_special_scancode(scancode))
		return 2;
	else
		return 1;
}





int kb_is_special_scancode(unsigned long scancode)
{
	short* tmp = malloc(sizeof(short));

	tmp = (short*)&scancode;
	if(tmp[0]== KB_SPECIAL_KEY)
		return 1;

	return 0;
}

int is_break_code(unsigned long scancode)
{
	char* tmp = malloc(sizeof(char));

	tmp = (char*)&scancode;

	if(scancode & BIT(7))
		return 1;

	return 0;
}
