all:	pibrot

LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont
INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont

.PHONY:	clean

pibrot:	pibrot.c communication.c egl_utils.c fractal.c multi_tex.c
	mkdir -p bin
	mpicc $(INCLUDES) $(LDFLAGS) pibrot.c communication.c egl_utils.c fractal.c multi_tex.c -o bin/pibrot

clean:
	rm -rf *.o
