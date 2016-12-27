# Makefile for 8queens

COMPILER_TYPE= gnu

CC= gcc

PROG= 8queens
SRCS= main.c 8queens.c vbe.c video_gr.c timer.c Keyboard.c Mouse.c Driver.c pixmap.c sprite.c algorithm.c rtc.c

CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .
MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>


clean:
	rm *.d