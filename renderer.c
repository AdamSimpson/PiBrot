#include "fractal.h"
#include "communication.h"
#include "ogl_utils.h"
#include "multi_tex.h"
#include "stdio.h"
#include "string.h"
#include "renderer.h"
#include "exit_menu_gl.h"

int main(int argc, char *argv[])
{
    // Initialize MPI
    int myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    FRAC_INFO frac_left, frac_right;
    init_fractal(&frac_left, true, 1300);
    init_fractal(&frac_right, true, 1300);

    if (myrank == 0){

        // Setup initial OpenGL state
        gl_t gl_state;
        memset(&gl_state, 0, sizeof(gl_t));

        // Start OpenGL
        render_t render_state;
        init_ogl(&gl_state, &render_state);

        // Initialize render parameters
        render_state.pause = false;
        render_state.quit_mode = false;
        render_state.screen_width = gl_state.screen_width;
        render_state.screen_height = gl_state.screen_height;
        render_state.return_value = 0;

        // Initialize exit menu
        exit_menu_t exit_menu_state;
        init_exit_menu(&exit_menu_state, &gl_state);
        render_state.exit_menu_state = &exit_menu_state;

        // Setup texture state
        texture_t texture_state;
        memset(&texture_state, 0, sizeof(texture_t));
        texture_state.gl_state = &gl_state;

        // Create and set textures
        create_textures(&texture_state, &frac_left, &frac_right);
        // Create and set vertices
        create_vertices(&texture_state);
        // Create and set shaders
        create_shaders(&texture_state);

        // Initial Draw before data arrives
        draw_textures(&texture_state);

        // Swap front/back buffers
        swap_ogl(&gl_state);

        // Start master loop
        master(&render_state, &frac_left, &frac_right, &texture_state);

        // Wait for key press
	while(!window_should_close(&gl_state)) {
            check_user_input(&gl_state);

            // Draw fractals
            draw_textures(&texture_state);

            // Render exit menu
            if(render_state.quit_mode)
                render_exit_menu(&exit_menu_state, 0.0f, 0.0f);

            // Swap buffers
            swap_ogl(&gl_state);
        }

        // Clean up and exit
        exit_ogl(&gl_state);
        exit_exit_menu(&exit_menu_state);

        MPI_Finalize();
        return render_state.return_value;

    }
    else if(myrank == 1)
	slave(&frac_left);
    else{
        slave(&frac_right);
    }

    MPI_Finalize();

    return 0;
}
