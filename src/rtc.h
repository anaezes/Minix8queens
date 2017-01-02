#ifndef __RTC_H
#define __RTC_H

#include <minix/drivers.h>
#include <stdio.h>
#include "i8254.h"
#include "Common.h"
#include "digits_pixmap.h"
#include "pixmap.h"
#include "vbe.h"

/** @defgroup rtc rtc
 * @{
 *
 * Constants and functions related to RTC.
 */

/**< @brief RTC IRQ line */
#define RTC_IRQ_LINE        8

/**< @brief RTC register address */
#define RTC_ADDR_REG        0x70

/**< @brief RTC data register */
#define RTC_DATA_REG        0x71

/**< @brief Register for second  */
#define RTC_SEC_ADDR        0

/**< @brief Register for minute  */
#define RTC_MIN_ADDR        2

/**< @brief Register for hour  */
#define RTC_HOUR_ADDR       4

/**< @brief Register for day  */
#define RTC_DAY_ADDR        7

/**< @brief Register for month */
#define RTC_MONTH_ADDR      8

/**< @brief Register for year  */
#define RTC_YEAR_ADDR       9

#define RTC_REG_A           10
#define RTC_REG_B           11
#define RTC_REG_C           12
#define RTC_REG_D           13

/**< @brief RTC register selection */
#define RTC_UIP_SET         BIT(7)


/**
 * @brief Struct that contains all elements.
 * of the current date and time.
 **/
typedef struct date_t
{
    unsigned long hour;
    unsigned long min;
    unsigned long sec;

    unsigned long day;
    unsigned long month;
    unsigned long year;

} date_t;


/**
 * @brief Init values for a date.
 * @return date.
 **/
date_t init_date();


/**
 * @brief Waiting for valid time/date.
 **/
void wait_valid_rtc();


/**
 * @brief Converts number on BCD to binary notation.
 * @param number on BCD notation.
 * @return Number on binary notation.
 **/
unsigned long BCD_TO_BIN(unsigned long value);


/**
 * @brief Retrieves time of the day.
 * @param pointer to a current hour.
 * @param pointer to a current minute.
 * @param pointer to a current second.
 * @return 0 on success, 1 otherwise.
 **/
int getTime(unsigned long* hour, unsigned long* min, unsigned long* sec);


/**
 * @brief Retrieves date of the day.
 * @param pointer to a current day.
 * @param pointer to a current month.
 * @param pointer to a current year.
 * @return 0 on success, 1 otherwise.
 **/
int getDate(unsigned long* day, unsigned long* month, unsigned long* year);


/**
 * @brief Compute digits of the time and the date.
 * @param pointer to a first digit.
 * @param pointer to a second digit.
 * @param parameter to parse.
 **/
void compute_digits(int* first_digit, int* second_digit, unsigned long part);


/**
 * @brief Obtains current date.
 * @return date
 **/
date_t get_curr_date();

/**
 * @brief Shows through the pixmap the digits.
 * @param pointer to current date's info
 **/
void show_date(date_t* date);



/**@}*/
#endif /* __RTC_H */
