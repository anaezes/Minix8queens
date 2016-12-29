//#include <minix/syslib.h>
//#include <minix/drivers.h>
//#include <machine/int86.h>

#include "vbe.h"

#include "lmlib.h"



int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p)
{
	struct reg86u r;

	mmap_t map;

	lm_alloc(sizeof(vbe_mode_info_t), &map);

	r.u.w.ax = 0x4F01;
	r.u.w.es = PB2BASE(map.phys);
	r.u.w.di = PB2OFF(map.phys);
	r.u.w.cx = mode;
	r.u.b.intno = 0x10;
	if( sys_int86(&r) != OK ) {
		printf("call bios\n");
		//call bios
	}

	*vmi_p = *(vbe_mode_info_t*) map.virtual; //virtual memory
	lm_free(&map); //free memory

	printf("SUCESSO GET MODE INFO\n");
	return 0;
}


int vbe_get_info(unsigned short mode, vbe_mode_info_t *vmi_p)
{
	struct reg86u r;

	mmap_t map;

	lm_alloc(sizeof(vbe_mode_info_t), &map);

	r.u.w.ax = 0x4F00;
	r.u.w.es = PB2BASE(map.phys);
	r.u.w.di = PB2OFF(map.phys);
	r.u.w.cx = mode;
	r.u.b.intno = 0x10;

	if( sys_int86(&r) != OK ) {
		printf("call bios\n");
		//call bios
	}

	*vmi_p = *(vbe_mode_info_t*) map.virtual; //virtual memory
	lm_free(&map); //free memory

	printf("SUCESSO GET INFO\n");
	return 0;
}
