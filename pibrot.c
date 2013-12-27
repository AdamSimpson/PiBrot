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

    FRAC_INFO frac_left, frac_right;
    init_fractal(&frac_left, 300);
    init_fractal(&frac_right, 1024);

    if (myrank == 0){

        // Setup initial OpenGL ES state
        STATE_T state;
        memset(&state, 0, sizeof(STATE_T));

        bcm_host_init();

        // Start OGLES
        init_ogl(&state.egl_state);

        // Create and set textures
        create_textures(&state, &frac_left, &frac_right);

        // Create and set vertices
        create_vertices();

        // Create and set shaders
        create_shaders(&state);

        // Initial Draw before data arrives
        draw_textures(&state);
        egl_swap(&state.egl_state);

        // Start master loop
        master(&frac_left, &frac_right, &state);

        // Wait for key press
	while(!state.terminate) {
            // Check for keyboard input
            int key_press = get_key_press(&state.egl_state);        
	    switch(key_press){
	        case KEY_Q: 
		    state.terminate=1;
	            break;
		case KEY_A:
		    show_left_tex_fullscreen(&state);
		    break;     
		case KEY_D:
		    show_right_tex_fullscreen(&state);
		    break;
		case KEY_S:
		    show_both_textures(&state);
		    break;
		default:
		    break;
            }

        }
        exit_ogl(&state.egl_state);	
    }
    else if(myrank == 1)
	slave(&frac_left);
    else{
        slave(&frac_right);
    }

    MPI_Finalize();

    return 0;
}
