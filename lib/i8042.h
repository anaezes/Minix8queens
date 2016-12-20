#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include "Common.h"

// Informs the keyboard driver to read a special key
#define KB_SPECIAL_KEY 		0xE0

// Our driver should stop when reading this
#define DRIVER_END_SCODE	0x81

#define KB_IRQ 				1

#define KBC_DATA_BUFFER		0x60
#define KBC_STAT_REG		0x64
#define KBC_CTRL_REG		0x64


// KBC IN_BUFFER responses
#define KBC_ACK				0xFA
#define KBC_RESEND 			0xFE
#define KBC_ERROR			0xFC

#define KBC_START_SCANNING	0xF6
#define KBC_STOP_SCANNING	0xF5


// Status register bits
#define KBC_OBF				BIT(0)
#define KBC_IBF				BIT(1)
#define KBC_SYS				BIT(2)
#define KBC_A2				BIT(3)
#define KBC_INH				BIT(4)
#define KBC_AUX				BIT(5)
#define KBC_TO_ERR			BIT(6)
#define KBC_PAR_ERR			BIT(7)

// Waitting delay
#define WAIT_KBC			2000
#define KBC_MAX_TRIES		10

// Keyboard leds
#define SCROLL_LOCK			0
#define NUM_LOCK			1
#define CAPS_LOCK			2

#define LED_ON				1
#define LED_OFF				-1
#define KBC_SWITCH_LEDS		0xED


// Mouse definitions
#define MOUSE_IRQ			12

#define MOUSE_STREAM_MODE	0xEA
#define KBC_MOUSE_CMD		0xD4
#define MOUSE_ENABLE_DR		0xF4
#define MOUSE_DISABLE_DR	0xF5
#define MOUSE_STATUS_REQ	0xE9
#define MOUSE_ENABLE		0xA8
#define X_SIGN_BIT			BIT(4)
#define Y_SIGN_BIT			BIT(5)

#define MOUSE_PACKET_SIZE	3
#define MOUSE_CONFIG_SIZE	3





#endif /* _LCOM_I8042_H */
