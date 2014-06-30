#ifndef EXIT_H
#define EXIT_H

#include "cursor_gl.h"
#include "image_gl.h"

typedef struct exit_menu_t {
    image_t *mandelbrot_state;
    image_t *sph_state;
    image_t *terminal_state;
    cursor_t *cursor_state;
} exit_menu_t;

void exit_exit_menu(exit_menu_t *state);
void init_exit_menu(exit_menu_t *state, gl_t *gl_state);
void render_exit_menu(exit_menu_t *state, float cursor_x, float cursor_y);

#endif             
