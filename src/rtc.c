/** @file */
#include "../lib/rtc.h"

char read_rtc(unsigned int addr)
{
	unsigned long temp;
	char ret;

	if(sys_outb(RTC_ADDR_REG, addr))
		return 1;
	if(sys_inb(RTC_DATA_REG, &temp))
		return 1;

	ret = (char) temp;
	return ret;
}

void rtc_disable_interrupts() {

	//read rtc
	char reg;

	reg = read_rtc(RTC_REG_B);
	reg = reg | (~UIE);

	//write rtc
	sys_outb(RTC_ADDR_REG, RTC_REG_B);
	sys_outb(RTC_DATA_REG, reg);
}

void rtc_enable_interrupts() {

	//read rtc
	char reg;

	reg = read_rtc(RTC_REG_B);
	reg = reg | UIE | 0x06;

	//write rtc
	sys_outb(RTC_ADDR_REG, RTC_REG_C);
	sys_outb(RTC_DATA_REG, 0x16);
}

void wait_valid_rtc() {

	unsigned long registA = 0;

	do
	{
		if(sys_outb(RTC_ADDR_REG,RTC_REG_A))
			return;
		if(sys_inb(RTC_DATA_REG, &registA))
			return;
	}
	while(registA & RTC_UIP_SET);
}

//convert from BCD to binary
unsigned long BCD_TO_BIN(unsigned long value) {

	return ((value >> 4) * 10) + (value & 0x0F);
}

int getTime(unsigned long *hour, unsigned long *min, unsigned long *sec) {

	//get seconds
	wait_valid_rtc();

	if(sys_outb(RTC_ADDR_REG, RTC_SEC_ADDR))
		return 1;
	if(sys_inb(RTC_DATA_REG, sec))
		return 1;


	//get minutes
	wait_valid_rtc();

	if(sys_outb(RTC_ADDR_REG, RTC_MIN_ADDR))
		return 1;
	if(sys_inb(RTC_DATA_REG, min))
		return 1;

	//get hours
	wait_valid_rtc();

	if (sys_outb(RTC_ADDR_REG, RTC_HOUR_ADDR))
		return 1;
	if (sys_inb(RTC_DATA_REG, hour))
		return 1;

	unsigned long time;

	if(sys_outb(RTC_ADDR_REG, RTC_REG_B))
		return 1;
	if(sys_inb(RTC_DATA_REG, &time))
		return 1;

	if(!(time & BIT(2)))
	{
		*sec = BCD_TO_BIN(*sec);
		*min = BCD_TO_BIN(*min);
		*hour = BCD_TO_BIN(*hour);
	}

	printf("Hour: %d", *hour);
	printf("h\n");
	printf("Minutes: %d", *min);
	printf("m\n");
	printf("Seconds: %d", *sec);
	printf("s\n");

	return 0;
}

int getDate(unsigned long *day, unsigned long *month, unsigned long *year) {

	//get day
	if(sys_outb(RTC_ADDR_REG, RTC_DAY_ADDR))
		return 1;
	if(sys_inb(RTC_DATA_REG, day))
		return 1;

	//get month
	if(sys_outb(RTC_ADDR_REG, RTC_MONTH_ADDR))
		return 1;
	if(sys_inb(RTC_DATA_REG, month))
		return 1;

	//get year
	if(sys_outb(RTC_ADDR_REG, RTC_YEAR_ADDR))
		return 1;
	if(sys_inb(RTC_DATA_REG, year))
		return 1;

	unsigned long date;
	if(sys_outb(RTC_ADDR_REG, RTC_REG_B))
		return 1;
	if(sys_inb(RTC_DATA_REG, &date))
		return 1;

	if(!(date & BIT(2)))
	{
		*day = BCD_TO_BIN(*day);
		*month = BCD_TO_BIN(*month);
		*year = BCD_TO_BIN(*year);
	}

	printf("Date: %d/%d/%d\n", *day, *month, *year);

	return 0;
}


//void loop()
//{
//	unsigned long hour;
//	unsigned long min;
//	unsigned long sec;
//
//	unsigned long day;
//	unsigned long month;
//	unsigned long year;
//
//	getTime(&hour, &min, &sec);
//	getDate(&day, &month, &year);
//
//}
