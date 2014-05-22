all:	pibrot

PIS=pi2 pi3 pi4 pi5 pi6 pi7 pi8 pi9 pi1
LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont
INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont
CCFLAGS+= -DRASPI -mfloat-abi=hard -mfpu=vfp -ffast-math -O3

.PHONY:	clean

pibrot:	pibrot.c communication.c egl_utils.c fractal.c multi_tex.c
	mkdir -p bin
	mpicc -O3 $(CCFLAGS) $(INCLUDES) $(LDFLAGS) ogl_utils.c rectangle_gl.c renderer.c communication.c egl_utils.c image_gl.c lodepng.c cursor_gl.c start_menu_gl.c exit_menu_gl.c fractal.c multi_tex.c -o bin/pibrot

run: copy
	cd $(HOME) && mpirun --host 192.168.3.100,192.168.3.101,192.168.3.102,192.168.3.103 -n 4 ./pibrot && cd PiBrot 

copy:
	@for pi in $(PIS);\
	do \
		scp ./bin/pibrot $$pi:~/; \
	done

clean:
	rm -rf *.o
