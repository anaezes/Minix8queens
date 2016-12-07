#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <math.h>

#include "../lib/8queens.h"
#include "../lib/pixmap.h"
#include "../lib/sprite.h"


int game_loop() {


	char* vram_address = vg_init(GRAPHIC_MODE);
	if(vram_address == NULL)
	{
		printf("Error\n");
		vg_exit();
		return 1;
	}

	int timer_irq = timer_subscribe_int();
	if(timer_irq == -1)
	{
		printf("ERROR: to subscribe timer");
		vg_exit();
		return 1;
	}

	int irq_set = kb_subscribe_int();
	if(irq_set == -1)
	{
		printf("ERROR: to subscribe kb");
		timer_unsubscribe_int();
		vg_exit();
		return 1;
	}


	vg_start();

	int finished = 0;
	int second_time = 0;
	int start_time = timer_get_ellapsed_time();

	unsigned long scancode = 0x00;
	int ipc_status, r;
	message msg;
	while(finished != 1) {
		if (driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & timer_irq)
					timer_int_handler();

				if (msg.NOTIFY_ARG & irq_set) {
					int n_bytes = kb_int_handler(&scancode);
					kb_print_scancode(scancode, n_bytes);

					if(scancode == DRIVER_END_SCODE)
						finished=1;
				}
				break;
			default:
				break;
			}
		}

		if(second_time == 0)
			if((timer_get_ellapsed_time() - start_time) >= 5)
			{
				second_time = 1;
				vg_game();
			}
	}

	vg_exit();
	kb_unsubscribe_int();
	timer_unsubscribe_int();
	return 0;
}



