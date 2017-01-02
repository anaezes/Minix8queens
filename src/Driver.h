#ifndef _DRIVER_H
#define _DRIVER_H

#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "i8042.h"
#include "Mouse.h"
#include "Keyboard.h"

/** @defgroup Driver Driver
 * @{
 *
 * Functions to interact with kbc.
 * Common to mouse and keyboard.
 *
 */


/**
 * @brief Reads a byte from the kbc state address.
 * @param state byte
 * @return 0 if success
 * */
int kbc_read_status(unsigned long*);


/**
 * @brief Wait for kbc inbuffer to be available.
 * @return 0 if success
 * */
int kbc_wait_inbuffer();


/**
 * @brief wait for kbc outbuffer to be available.
 * @param state byte
 * @return 0 if success
 * */
int kbc_wait_outbuffer(unsigned long*);


/**
 * @brief Read byte from kbc input buffer.
 * @param state byte
 * @return 0 if success
 * */
int kbc_read_input(unsigned long*);


/**
 * @brief Write command to kbc output buffer.
 * @param state byte
 * @return 0 if success
 * */
int kbc_send_command(unsigned long);


/**
 * @brief Signal mouse that we want to write a command to it.
 * @return 0 if success
 * */
int kbc_signal_mouse_write();


/**
 * Write a command to the mouse
 * @param mouse command
 * @return 0 if success
 * */
int kbc_write_to_mouse(unsigned long);


/**
 * @brief Clean the kbc input buffer
 * */
void kbc_flush_data_buffer(void);


/**
 * @brief Enable mouse stream mode and sending of data packets
 * @return 0 if success
 * */
int kbc_mouse_init();




/** @} end of driver */
#endif /* __DRIVER_H */
