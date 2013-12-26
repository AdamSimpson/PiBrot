#ifndef MULTI_TEX_H
#define MULTI_TEX_H

#include "GLES2/gl2.h"
#include "egl_utils.h"
#include "fractal.h"

#define NUM_TEXTURES 2
#define LEFT 0
#define RIGHT 1

typedef struct
{
    // OpenGL|ES state
    EGL_STATE_T egl_state;

    // Program handle
    GLuint program;

    // Locations
    GLint position_location;
    GLint tex_coord_location;
    GLint tex_location;

    // Texture handles
    GLuint textures[NUM_TEXTURES];

    // Texture attributes
    GLsizei tex_width[NUM_TEXTURES];
    GLsizei tex_height[NUM_TEXTURES];

    int terminate;
} STATE_T;

void update_texture_rows(STATE_T *state, int fractal, GLsizei start_row, GLuint num_rows, GLubyte *row_pixels);
void update_fractal_rows(const STATE_T *state, int fractal, unsigned int start_row, unsigned int num_rows, unsigned char *row_pixels);
void create_textures(STATE_T *state, FRAC_INFO *frac_left, FRAC_INFO *frac_right);
void create_vertices();
void create_shaders(STATE_T *state);
void draw_textures(STATE_T *state);
void show_left_tex_fullscreen(STATE_T *state);
void show_right_tex_fullscreen(STATE_T *state);
void show_both_textures(STATE_T *state);
void set_min_filter(int mode);

#endif
