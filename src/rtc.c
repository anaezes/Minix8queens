/** @file */
#include "rtc.h"

date_t init_date()
{
	date_t date;

	date.hour = 0;
	date.min = 0;
	date.sec = 0;

	date.day = 0;
	date.month = 0;
	date.year = 0;

	return date;
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

int getTime(unsigned long* hour, unsigned long* min, unsigned long*sec) {

	//get seconds
	wait_valid_rtc();
	if(sys_outb(RTC_ADDR_REG, RTC_SEC_ADDR) || sys_inb(RTC_DATA_REG, sec))
		return 1;

	//get minutes
	wait_valid_rtc();
	if(sys_outb(RTC_ADDR_REG, RTC_MIN_ADDR) || sys_inb(RTC_DATA_REG, min))
		return 1;

	//get hours
	wait_valid_rtc();
	if (sys_outb(RTC_ADDR_REG, RTC_HOUR_ADDR) || sys_inb(RTC_DATA_REG, hour))
		return 1;

	unsigned long time;
	if(sys_outb(RTC_ADDR_REG, RTC_REG_B) || sys_inb(RTC_DATA_REG, &time))
		return 1;


	if(!(time & BIT(2)))
	{
		*sec = BCD_TO_BIN(*sec);
		*min = BCD_TO_BIN(*min);
		*hour = BCD_TO_BIN(*hour);
	}

	return 0;
}

int getDate(unsigned long* day, unsigned long *month, unsigned long *year) {

	//get day
	if(sys_outb(RTC_ADDR_REG, RTC_DAY_ADDR) || sys_inb(RTC_DATA_REG, day))
		return 1;

	//get month
	if(sys_outb(RTC_ADDR_REG, RTC_MONTH_ADDR) || sys_inb(RTC_DATA_REG, month))
		return 1;

	//get year
	if(sys_outb(RTC_ADDR_REG, RTC_YEAR_ADDR) || sys_inb(RTC_DATA_REG, year))
		return 1;

	unsigned long date;
	if(sys_outb(RTC_ADDR_REG, RTC_REG_B) || sys_inb(RTC_DATA_REG, &date))
		return 1;

	if(!(date & BIT(2)))
	{
		*day = BCD_TO_BIN(*day);
		*month = BCD_TO_BIN(*month);
		*year = BCD_TO_BIN(*year);
	}

	return 0;
}

date_t get_curr_date()
{
	date_t date;

	if(getTime(&date.hour, &date.min, &date.sec) || getDate(&date.day, &date.month, &date.year))
		return init_date();

	return date;
}

void compute_digits(int* first_digit, int* second_digit, unsigned long part)
{
	*first_digit = part / 10;
	*second_digit = part % 10;
}

void show_date(date_t* date)
{
	int first_digit;
	int second_digit;
	pixmap_t px;

	//hour
	compute_digits(&first_digit, &second_digit, date->hour);
	px = get_pixmap_digit(first_digit);
	vg_draw_pixmap(695, 200, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(second_digit);
	vg_draw_pixmap(740, 200, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(PXMAP_DPOINTS);
	vg_draw_pixmap(785, 215, px.pixmap, px.width, px.height);

	//minutes
	compute_digits(&first_digit, &second_digit, date->min);
	px = get_pixmap_digit(first_digit);
	vg_draw_pixmap(805, 200, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(second_digit);
	vg_draw_pixmap(850, 200, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(PXMAP_DPOINTS);
	vg_draw_pixmap(895, 215, px.pixmap, px.width, px.height);

	//seconds
	compute_digits(&first_digit, &second_digit, date->sec);
	px = get_pixmap_digit(first_digit);
	vg_draw_pixmap(915, 200, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(second_digit);
	vg_draw_pixmap(960, 200, px.pixmap, px.width, px.height);

	//day
	compute_digits(&first_digit, &second_digit, date->day);
	px = get_pixmap_small_digit(first_digit);
	vg_draw_pixmap(810, 165, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(second_digit);
	vg_draw_pixmap(830, 165, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(PXMAP_SPOINTS);
	vg_draw_pixmap(855, 185, px.pixmap, px.width, px.height);

	//month
	compute_digits(&first_digit, &second_digit, date->month);
	px = get_pixmap_small_digit(first_digit);
	vg_draw_pixmap(865, 165, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(second_digit);
	vg_draw_pixmap(885, 165, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(PXMAP_SPOINTS);
	vg_draw_pixmap(905, 185, px.pixmap, px.width, px.height);

	//year
	px = get_pixmap_small_digit(2);
	vg_draw_pixmap(920, 165, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(0);
	vg_draw_pixmap(940, 165, px.pixmap, px.width, px.height);

	date->year = date->year % 100;
	compute_digits(&first_digit, &second_digit, date->year);
	px = get_pixmap_small_digit(first_digit);
	vg_draw_pixmap(960, 165, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(second_digit);
	vg_draw_pixmap(980, 165, px.pixmap, px.width, px.height);
}
