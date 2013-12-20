#include "fractal.h"
#include "communication.h"
#include "egl_utils.h"
#include "multi_tex.h"
#include "stdio.h"
#include "linux/input.h"

int main(int argc, char *argv[])
{
    // Initialize MPI
    int myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    FRAC_INFO info;
    init_fractal(&info);

    if (myrank == 0){

        // Setup initial OpenGL ES state
        STATE_T state;
        memset(&state, 0, sizeof(STATE_T));

        bcm_host_init();

        // Start OGLES
        init_ogl(&state.egl_state);

        // Create and set textures
        create_textures(&state, &info);

        // Create and set vertices
        create_vertices();

        // Create and set shaders
        create_shaders(&state);

        // Start master loop
        master(&info, &state);

        // Wait for key press
	while(!state.terminate) {
            // Check for keyboard input
            int key_press = get_key_press(&state.egl_state);        
            if(key_press == KEY_Q)
                state.terminate=1;
        }
        exit_func(&state.egl_state);	
    }
    else // Start slave loop
        slave(&info);

    MPI_Finalize();

    return 0;
}
