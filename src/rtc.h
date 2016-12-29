#ifndef __RTC_H
#define __RTC_H

#include <minix/drivers.h>
#include <stdio.h>
#include "i8254.h"
#include "Common.h"
#include "digits_pixmap.h"
#include "pixmap.h"
#include "vbe.h"

#define RTC_IRQ_LINE		8

#define RTC_ADDR_REG		0x70
#define RTC_DATA_REG		0x71

#define RTC_SEC_ADDR		0
#define RTC_MIN_ADDR		2
#define RTC_HOUR_ADDR	    4
#define RTC_DAY_ADDR	   	7
#define RTC_MONTH_ADDR	   	8
#define RTC_YEAR_ADDR	   	9

#define RTC_REG_A 			10
#define RTC_REG_B 			11
#define RTC_REG_C 			12
#define RTC_REG_D			13

#define RTC_UIP_SET			BIT(7)

#define UIP 				(1 << 7)
#define UIE 				(1 << 4)

typedef struct date_t
{
	unsigned long hour;
	unsigned long min;
	unsigned long sec;

	unsigned long day;
	unsigned long month;
	unsigned long year;

} date_t;


void rtc_enable_interrupts();
void rtc_disable_interrupts();

void wait_valid_rtc();

unsigned long BCD_TO_BIN(unsigned long value);

int getTime(unsigned long* hour, unsigned long* min, unsigned long* sec);

int getDate(unsigned long* day, unsigned long* month, unsigned long* year);

date_t get_curr_date();

void show_date(date_t* date);

void loop();



#endif /* __RTC_H */
