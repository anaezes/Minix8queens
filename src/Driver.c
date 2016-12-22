// Implementation of simple mouse controller

#include "../lib/Driver.h"



/* General functions*/

int kbc_read_status(unsigned long* stat)
{
	return sys_inb(KBC_STAT_REG, stat);
}

/* wait for inbuffer to be available */
int kbc_wait_inbuffer()
{
	unsigned long stat;
	unsigned int n_tries = 0;

	while(n_tries < KBC_MAX_TRIES)
	{
		// can read status ?
		if(kbc_read_status(&stat) == OK)
			// if IBF is not defined, we can write
			if(!(stat & KBC_IBF))
				return 0;

		// wait
		tickdelay(micros_to_ticks(WAIT_KBC));
		n_tries++;
	}

	return 1;
}



/* wait for outbuffer to be available */
int kbc_wait_outbuffer(unsigned long *stat)
{
	unsigned int n_tries = 0;

	while(n_tries < KBC_MAX_TRIES)
	{
		// can read status ?
		if(kbc_read_status(stat) == OK)
			// if OBF is defined, we can read
			if(*stat & KBC_OBF == 1)
				return 0;

		// wait
		tickdelay(micros_to_ticks(WAIT_KBC));
		n_tries++;
	}

	return 1;
}



int kbc_read_input(unsigned long *output)
{
	unsigned long stat;
	unsigned int n_tries = 0;

	while(n_tries < KBC_MAX_TRIES)
	{
		//wait to read
		if(kbc_wait_outbuffer(&stat) == OK)
		{
			// read data to output
			sys_inb(KBC_DATA_BUFFER,output);
			//data is not corrupt ?
			if((stat & (KBC_PAR_ERR | KBC_TO_ERR)) == 0)
				return 0;
		}
		// wait
		tickdelay(micros_to_ticks(WAIT_KBC));
		n_tries++;
	}

	return 1;
}



int kbc_send_command(unsigned long cmd)
{
	unsigned int n_tries = 0;

	while(n_tries < KBC_MAX_TRIES)
	{
		// can write ?
		if(kbc_wait_inbuffer() == 0)
			return sys_outb(KBC_DATA_BUFFER, cmd);

		// wait
		tickdelay(micros_to_ticks(WAIT_KBC));
		n_tries++;
	}
	return 1;
}



// warn kbc that we want to write to the mouse
int kbc_signal_mouse_write()
{
	// can write ?
	if(kbc_wait_inbuffer() == 0)
		return sys_outb(KBC_CTRL_REG, KBC_MOUSE_CMD);

	return 1;
}



int kbc_write_to_mouse(unsigned long cmd)
{
	unsigned int n_tries = 0;

	while(n_tries < KBC_MAX_TRIES)
	{
		//can not write: exit
		if(kbc_signal_mouse_write() == 1)
			return 1;

		kbc_flush_data_buffer();

		if(kbc_send_command(cmd) == 1)
			return 1;

		unsigned long output;
		if(kbc_read_input(&output) == 1)
			return 1;

		if(output == KBC_ACK)
			return 0;

		else if(output == KBC_ERROR)
			return 1;

		tickdelay(micros_to_ticks(WAIT_KBC));
		n_tries++;
	}

	return 1;
}


void kbc_flush_data_buffer(void)
{
	unsigned long trash;
	sys_inb(KBC_DATA_BUFFER, &trash);
}

int kbc_mouse_init()
{
	if(sys_outb(KBC_CTRL_REG, MOUSE_ENABLE))
		return 1;
	if(kbc_write_to_mouse(MOUSE_STREAM_MODE) != 0)
		return 1;
	if(kbc_write_to_mouse(MOUSE_ENABLE_DR) != 0)
		return 1;

	return 0;
}
