#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/drivers.h>
#include "../lib/i8254.h"

// global variables for timer_test_int()
unsigned int hook_id = TIMER0_IRQ;
unsigned int counter = 0;
unsigned int time_in_seconds = 0;


unsigned int timer_get_ellapsed_time()
{
	return time_in_seconds;
}

// subscribes and enables timer 0 interrupts
int timer_subscribe_int(void)
{
	unsigned int tmp = hook_id;

	if(sys_irqsetpolicy( TIMER0_IRQ, IRQ_REENABLE, &hook_id) == OK)
		if(sys_irqenable(&hook_id) == OK)
			return BIT(tmp);

	printf("Error subscribing to timer interrupts");
	return -1;
}

// unsubscribes timer 0 interrupts
int timer_unsubscribe_int()
{
	if(sys_irqrmpolicy(&hook_id) == OK)
		if(sys_irqdisable(&hook_id) == OK)
			return 0;

	return 1;
}

// increments counter
void timer_int_handler()
{
	counter ++;

	if(counter%60 ==0)
		time_in_seconds++;
}

// tests timer 0 interrupt handling
int timer_delay(unsigned long time)
{
	counter = 0;
	int ipc_status, r;
	message msg;
	int irq_set = timer_subscribe_int();

	while(counter <= (time *60))
	{  //You may want to use a different condition
		// Get a request message.
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			 //received notification
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				//hardware interrupt notification
				if (msg.NOTIFY_ARG & irq_set)
					timer_int_handler();
				break;
			default:
				break;
				//no other notifications expected: do nothing
			}
		}
		else {
			//received a standard message, not a notification no standard messages expected: do nothing
		}
	}
	
	timer_unsubscribe_int();
	return 0;
}
