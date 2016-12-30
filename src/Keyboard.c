/* Keyboard functions */

#include "Keyboard.h"

int special = 0;
unsigned int kbc_kb_hook_id = KB_IRQ;

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
