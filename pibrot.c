#include "fractal.h"
#include "communication.h"
#include "egl_utils.h"
#include "multi_tex.h"
#include "stdio.h"

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
        create_textures(&state);

        // Create and set vertices
        create_vertices();

        // Create and set shaders
        create_shaders(&state);

        // Start master loop
        master(&info, &state);
    }
    else // Start slave loop
        slave(&info);

    MPI_Finalize();

    return 0;
}
