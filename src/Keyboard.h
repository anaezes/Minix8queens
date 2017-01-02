#ifndef __KB_H
#define __KB_H

#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/drivers.h>

#include "Driver.h"
#include "i8042.h"

/** @defgroup Keyboard Keyboard
 * @{
 *
 * Functions to interact with keyboard.
 *
 */

/**
 * @brief  Subscribes and enables keyboard interrupts.
 * @return 0 if success
 * */
int kb_subscribe_int(void);


/**
 * @brief Unsubscribes keyboard interrupts.
 * @return 0 if success
 * */
int kb_unsubscribe_int(void);


/**
 * @brief This function is called when a keyboard
 * interruption is triggered and reads the
 * scancode from KB_OUT_BUFFER.
 * @param scancode
 * @return 0 if success
 * */
int kb_int_handler(unsigned long*);


/**
 * @brief Given a scancode checks if it`s a special scancode
 * @param scancode
 * @return 0 if success
 * */
int kb_is_special_scancode(unsigned long scancode);


/**
 * @brief Given a scancode checks if it`s a break code
 * @param scancode
 * @return 1 if success
 * */
int is_break_code(unsigned long);


/** @} end of Keyboard */
#endif /* __KB_H */
