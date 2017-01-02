#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include "Common.h"

/** @defgroup i8042 i8042
 * @{
 * Constants for programming the i8042 controller.
 *
 */

/**< @brief Inform the keyboard driver to read a special key */
#define KB_SPECIAL_KEY 		0xE0


/**< @brief Our driver should stop when reading this */
#define DRIVER_END_SCODE	0x81

/**< @brief enter key scancode  */
#define KEY_ENTER			0x1C

/**< @brief R key scancode  */
#define KEY_R				0x93

/**< @brief up key scancode   */
#define KEY_UP				0xC8

/**< @brief down key scancode */
#define KEY_DOWN			0xD0

/**< @brief left key scancode */
#define KEY_LEFT			0xCB

/**< @brief right key scancode */
#define KEY_RIGHT			0xCD


/**< @brief keyboard IRQ line */
#define KB_IRQ 				1

/**< @brief keyboard data buffer address */
#define KBC_DATA_BUFFER		0x60

/**< @brief keyboard status buffer address */
#define KBC_STAT_REG		0x64

/**< @brief keyboard control buffer address */
#define KBC_CTRL_REG		0x64


// KBC IN_BUFFER responses

/**< @brief kbc ACK response */
#define KBC_ACK				0xFA

/**< @brief kbc RESEND response */
#define KBC_RESEND 			0xFE

/**< @brief kbc ERROR response */
#define KBC_ERROR			0xFC

/**< @brief kbc command to start scanning */
#define KBC_START_SCANNING	0xF6

/**< @brief kbc command to stop scanning */
#define KBC_STOP_SCANNING	0xF5


// Status register bits

/**< @brief kbc status register bit to signal that output buffer is available */
#define KBC_OBF				BIT(0)

/**< @brief kbc status register bit to signal that input buffer is available */
#define KBC_IBF				BIT(1)

#define KBC_SYS				BIT(2)
#define KBC_A2				BIT(3)
#define KBC_INH				BIT(4)
#define KBC_AUX				BIT(5)

/**< @brief kbc status register bit to signal error */
#define KBC_TO_ERR			BIT(6)

/**< @brief kbc status register bit to signal parity error */
#define KBC_PAR_ERR			BIT(7)


/**< @brief Time that kbc should wait for input or output buffers to be available */
#define WAIT_KBC			2000

/**< @brief number of tries that kbc should try to read or write from output and input buffer */
#define KBC_MAX_TRIES		10

/**< @brief mouse IRQ line*/
#define MOUSE_IRQ			12

/**< @brief mouse command to activate stream mode */
#define MOUSE_STREAM_MODE	0xEA

/**< @brief mouse message to signal that the next message will be a command*/
#define KBC_MOUSE_CMD		0xD4

/**< @brief mouse command to enable sending of data packets */
#define MOUSE_ENABLE_DR		0xF4

/**< @brief mouse command to disable sending of data packets*/
#define MOUSE_DISABLE_DR	0xF5

/**< @brief mouse message to request status */
#define MOUSE_STATUS_REQ	0xE9

/**< @brief mouse command to enable mouse */
#define MOUSE_ENABLE		0xA8



/**< @brief mouse packet bit that informs left button state*/
#define LEFT_BUTTON	 		BIT(0)

/**< @brief mouse packet bit that informs right button state*/
#define RIGHT_BUTTON	 	BIT(1)

/**< @brief mouse packet bit that informs x signal */
#define X_SIGN_BIT			BIT(4)

/**< @brief mouse packet bit that informs y signal */
#define Y_SIGN_BIT			BIT(5)

/**< @brief mouse packet bit that informs x overflow */
#define X_OVERFLOW_BIT		BIT(6)

/**< @brief mouse packet bit that informs y overflow */
#define Y_OVERFLOW_BIT		BIT(7)

/**< @brief mouse packet size */
#define MOUSE_PACKET_SIZE	3

/**< @brief mouse packet config size */
#define MOUSE_CONFIG_SIZE	3


/**@}*/
#endif /* _LCOM_I8042_H */
