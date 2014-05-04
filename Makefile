all:	pibrot

LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont
INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont
CCFLAGS+= -mfloat-abi=hard -mfpu=vfp -ffast-math -O3
DEBUG+=-g -pg

.PHONY:	clean

pibrot:	pibrot.c communication.c egl_utils.c fractal.c multi_tex.c
	mkdir -p bin
	mpicc -O3 $(CCFLAGS) $(INCLUDES) $(LDFLAGS) pibrot.c communication.c egl_utils.c fractal.c multi_tex.c -o bin/pibrot

debug:	pibrot.c communication.c egl_utils.c fractal.c multi_tex.c
	mkdir -p bin
	mpicc $(CCFLAGS) $(DEBUG) $(INCLUDES) $(LDFLAGS) pibrot.c communication.c egl_utils.c fractal.c multi_tex.c -o bin/debug

run: copy
	cd $(HOME) && mpirun --host 192.168.3.100,192.168.3.101,192.168.3.102,192.168.3.103 -n 4 ./pibrot && cd PiBrot 

copy:
	scp ./bin/pibrot pi0:~/
	scp ./bin/pibrot pi1:~/
	scp ./bin/pibrot pi2:~/
	scp ./bin/pibrot pi3:~/

clean:
	rm -rf *.o
