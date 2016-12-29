/** @file */
#include "rtc.h"

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

	return 0;
}

int getDate(unsigned long* day, unsigned long *month, unsigned long *year) {

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

	return 0;
}

date_t get_curr_date()
{
	date_t date;

	getTime(&date.hour, &date.min, &date.sec);
	getDate(&date.day, &date.month, &date.year);

	return date;
}

void show_date(date_t* date)
{
	//printf("Date: %d/%d/%d\n", date->day, date->month, date->year);
	//printf("%d : %d : %d\n", date->hour, date->min, date->sec);

	//hour
	int first_digit = date->hour / 10;
	int second_digit = date->hour % 10;
	pixmap_t px = get_pixmap_digit(first_digit);
	vg_draw_pixmap(695, 220, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(second_digit);
	vg_draw_pixmap(740, 220, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(PXMAP_DPOINTS);
	vg_draw_pixmap(785, 235, px.pixmap, px.width, px.height);

	//minutes
	first_digit = date->min / 10;
	second_digit = date->min % 10;
	px = get_pixmap_digit(first_digit);
	vg_draw_pixmap(805, 220, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(second_digit);
	vg_draw_pixmap(850, 220, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(PXMAP_DPOINTS);
	vg_draw_pixmap(895, 235, px.pixmap, px.width, px.height);

	//seconds
	first_digit = date->sec / 10;
	second_digit = date->sec % 10;
	px = get_pixmap_digit(first_digit);
	vg_draw_pixmap(915, 220, px.pixmap, px.width, px.height);
	px = get_pixmap_digit(second_digit);
	vg_draw_pixmap(960, 220, px.pixmap, px.width, px.height);

	//day
	first_digit = date->day / 10;
	second_digit = date->day % 10;
	px = get_pixmap_small_digit(first_digit);
	vg_draw_pixmap(810, 185, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(second_digit);
	vg_draw_pixmap(830, 185, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(PXMAP_SPOINTS);
	vg_draw_pixmap(855, 205, px.pixmap, px.width, px.height);

	//month
	first_digit = date->month / 10;
	second_digit = date->month % 10;
	px = get_pixmap_small_digit(first_digit);
	vg_draw_pixmap(865, 185, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(second_digit);
	vg_draw_pixmap(885, 185, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(PXMAP_SPOINTS);
	vg_draw_pixmap(905, 205, px.pixmap, px.width, px.height);

	//year
	px = get_pixmap_small_digit(2);
	vg_draw_pixmap(920, 185, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(0);
	vg_draw_pixmap(940, 185, px.pixmap, px.width, px.height);

	date->year = date->year % 100;
	first_digit = date->year / 10;
	second_digit = date->year % 10;
	px = get_pixmap_small_digit(first_digit);
	vg_draw_pixmap(960, 185, px.pixmap, px.width, px.height);
	px = get_pixmap_small_digit(second_digit);
	vg_draw_pixmap(980, 185, px.pixmap, px.width, px.height);
}
