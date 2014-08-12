/*
The MIT License (MIT)

Copyright (c) 2014 Adam Simpson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>
#include <unistd.h>
#include "start_menu_gl.h"
#include "renderer.h"
#include "image_gl.h"
#include "cursor_gl.h"
#include "stdlib.h"

void init_start_menu(start_menu_t *state, gl_t *gl_state)
{
    // Initialize program launcher images OpenGL state
    int image_width = 753;
    int image_height = 270;

    float half_height = (image_height/(float)gl_state->screen_height);
    float half_width = (image_width/(float)gl_state->screen_width);

    float lower_left_y = -half_height;
    float lower_left_x = -half_width;
    state->start_state = malloc(sizeof(image_t));

    #ifdef RASPI
    init_image(state->start_state,
               gl_state,
               "PiBrot/images/start.png",
               "PiBrot/images/start-selected.png",
               lower_left_x, lower_left_y,
               image_width, image_height);
    #else
    init_image(state->start_state,
               gl_state,
               "images/start.png",
               "images/start-selected.png",
               lower_left_x, lower_left_y,
               image_width, image_height);
    #endif

    // Initialize cursor
    state->cursor_state = malloc(sizeof(cursor_t));
    int cursor_width = 120;
    int cursor_height = 160;
    #ifdef RASPI
    init_cursor(state->cursor_state, gl_state, "PiBrot/images/cursor.png", cursor_width, cursor_height);
    #else
    init_cursor(state->cursor_state, gl_state, "images/cursor.png", cursor_width, cursor_height); 
    #endif
}

void exit_start_menu(start_menu_t *state)
{
    // Need to free up other resources as well...
    free(state->start_state);
    free(state->cursor_state);
}

void render_start_menu(start_menu_t *state, float cursor_x, float cursor_y)
{
    // Update center of cursor
    set_cursor_position(state->cursor_state, cursor_x, cursor_y);

    // Check if anything selected
    check_cursor_in_image(state->cursor_state, state->start_state);

    // Draw start image
    draw_image(state->start_state);

    // Draw cursor
    draw_cursor(state->cursor_state);
}
