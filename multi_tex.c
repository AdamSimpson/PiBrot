#include <stdio.h>
#include <assert.h>

#include "multi_tex.h"
#include "ogl_utils.h"
#include "stdlib.h"
#include "fractal.h"

#define USE_MIPMAP 1

void create_textures(texture_t *state, FRAC_INFO *frac_left, FRAC_INFO *frac_right)
{
    int i,j,k;
    GLubyte *pixels;

    state->tex_width[LEFT] = frac_left->num_cols;
    state->tex_height[LEFT] = frac_left->num_rows;

    state->tex_width[RIGHT] = frac_right->num_cols;
    state->tex_height[RIGHT] = frac_right->num_rows;

    size_t pixel_bytes;
    pixel_bytes = state->tex_width[LEFT]*state->tex_height[LEFT]*sizeof(GLubyte)*frac_left->channels;
 
    // Left fractal
    pixels = malloc(pixel_bytes);

     // Pixel packing
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Generate texture
    glGenTextures(NUM_TEXTURES, state->textures);
    // Set sampler
    glActiveTexture(GL_TEXTURE0);
 
   
    // Initialize left texture to black
    for(i=0; i<state->tex_height[LEFT]; i++) {
        for(j=0; j<state->tex_width[LEFT]; j++) {
            for(k=0; k<frac_left->channels; k++) {
                pixels[(i*state->tex_width[LEFT] + j)*frac_left->channels + k] = 255;
            }
        }
    }
 
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, state->textures[LEFT]);

    // Load texture
    #ifdef GLFW
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, state->tex_width[LEFT], state->tex_height[LEFT], 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
    #else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, state->tex_width[LEFT], state->tex_height[LEFT], 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
    #endif

    // Set filtering modes
    #if !(USE_MIPMAP)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #endif

    free(pixels);

    #if USE_MIPMAP
    // Generate mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    #endif

    free(pixels);

    // Right fractal
    pixel_bytes = state->tex_width[RIGHT]*state->tex_height[RIGHT]*sizeof(GLubyte)*frac_right->channels; 
    pixels = malloc(pixel_bytes);

    // Initialize right texture to white
    for(i=0; i<state->tex_height[RIGHT]; i++) {
        for(j=0; j<state->tex_width[RIGHT]; j++) {
            for(k=0; k<frac_right->channels; k++) {
                pixels[(i*state->tex_width[RIGHT] + j)*frac_right->channels + k] = 0;
            }
        }
    }
 
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, state->textures[1]);

    // Load texture
    #ifdef GLFW
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, state->tex_width[RIGHT], state->tex_height[RIGHT], 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
    #else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, state->tex_width[RIGHT], state->tex_height[RIGHT], 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
    #endif

    #if USE_MIPMAP
    // Generate mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    #endif

    // Set filtering modes
    #if !(USE_MIPMAP)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    #if USE_MIPMAP
    // Generate mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    #endif

    // Free pixels
    free(pixels);

}

void update_texture_rows(texture_t *state, int fractal, GLsizei start_row, GLuint num_rows, GLubyte *row_pixels)
{
    glBindTexture(GL_TEXTURE_2D, state->textures[fractal]);

    #ifdef GLFW
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, start_row, state->tex_width[fractal], num_rows, GL_RED, GL_UNSIGNED_BYTE, row_pixels);
    #else
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, start_row, state->tex_width[fractal], num_rows, GL_LUMINANCE, GL_UNSIGNED_BYTE, row_pixels);
    #endif

    #if USE_MIPMAP
    glGenerateMipmap(GL_TEXTURE_2D);
    #endif
}

// GLUE between outside world and textures - still requires state which I don't like
void update_fractal_rows(texture_t *state, int fractal, unsigned int start_row, unsigned int num_rows, unsigned char *row_pixels)
{
   update_texture_rows(state, fractal, (GLsizei)start_row, (GLsizei)num_rows, (GLubyte*)row_pixels);
   // Draw textures
   draw_textures(state);

   // Swap buffers
   swap_ogl(state->gl_state);
}

void create_vertices()
{
    // Vertices: Pos(x,y) Tex(x,y)
    // For simplicity only single vbo is generated and offset used as needed
    float vertices[] = {
        // Image 0 vertices
        -1.0f,   1.0f, 0.0f, 0.0f, // Top left
        -0.005f, 1.0f, 1.0f, 0.0f, // Top right
        -0.005f,-1.0f, 1.0f, 1.0f, // Bottom right
	-1.0f,  -1.0f, 0.0f, 1.0f,  // Bottom left
        // Image 1 vertices
         0.005f, 1.0f, 0.0f, 0.0f, // Top left
         1.0f,   1.0f, 1.0f, 0.0f, // Top right
         1.0f,  -1.0f, 1.0f, 1.0f, // Bottom right
	 0.005f,-1.0f, 0.0f, 1.0f,  // Bottom left
         // Full screen vertices
        -1.0f,  1.0f, 0.0f, 0.0f, // Top left
         1.0f,  1.0f, 1.0f, 0.0f, // Top right
         1.0f, -1.0f, 1.0f, 1.0f, // Bottom right
	-1.0f, -1.0f, 0.0f, 1.0f  // Bottom left
    };

    // VAO is required for OpenGL 3+ when using VBO I believe
    #ifndef RASPI
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    #endif

    // Generate vertex buffer
    GLuint vbo;
    glGenBuffers(1, &vbo);
    // Set buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Fill buffer
    glBufferData(GL_ARRAY_BUFFER, 3*4*4*sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    // Elements
    GLubyte elements[] = {
        2, 3, 0,
        0, 1, 2
    };
    // Generate element buffer
    GLuint ebo;
    glGenBuffers(1, &ebo);
    // Set buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // Fill buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*3*sizeof(GLubyte), elements, GL_STATIC_DRAW);
}

void create_shaders(texture_t *state)
{
    #ifdef GLFW
    // Shader source
    const GLchar* vertexSource =
        "#version 150 core\n"
        "in vec2 position;"
        "in vec2 tex_coord;"
        "out vec2 frag_tex_coord;"
        "void main() {"
        "   gl_Position = vec4(position, 0.0, 1.0);"
        "   frag_tex_coord = tex_coord;"
        "}";
    const GLchar* fragmentSource =
        "#version 150 core\n"
        "in vec2 frag_tex_coord;"
        "uniform sampler2D tex;"
        "out vec4 OutColor;"
        "void main() {"
        "   OutColor = texture(tex, frag_tex_coord);"
        "}";
    #else
    // Shader source
    const GLchar* vertexSource =
        "attribute vec2 position;"
        "attribute vec2 tex_coord;"
        "varying vec2 frag_tex_coord;"
        "void main() {"
        "   gl_Position = vec4(position, 0.0, 1.0);"
        "   frag_tex_coord = tex_coord;"
        "}";
    const GLchar* fragmentSource =
        "precision mediump float;"
        "varying vec2 frag_tex_coord;"
        "uniform sampler2D tex;"
        "void main() {"
        "   gl_FragColor = texture2D(tex, frag_tex_coord);"
        "}";
    #endif

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    showlog(vertexShader);   

    // Compile frag shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    showlog(fragmentShader);

    // Create shader program
    state->program = glCreateProgram();
    glAttachShader(state->program, vertexShader);
    glAttachShader(state->program, fragmentShader);

    // Link and use program
    glLinkProgram(state->program);
    glUseProgram(state->program);
    check();
   
    // Get position location
    state->position_location = glGetAttribLocation(state->program, "position");
    // Get tex_coord location
    state->tex_coord_location = glGetAttribLocation(state->program, "tex_coord");
    // Get tex uniform location
    state->tex_location = glGetUniformLocation(state->program, "tex");
}

void show_left_tex_fullscreen(texture_t *state)
{
    // Size of each vertex in bytes
    size_t vert_size = 4*sizeof(GL_FLOAT);
    // Offset in our vertices array that the full screen vertices start
    size_t offset = 2*4*vert_size;

    // Draw left fractal
    glVertexAttribPointer(state->position_location, 2, GL_FLOAT, GL_FALSE, vert_size, (void*)offset);
    glEnableVertexAttribArray(state->position_location);
    glVertexAttribPointer(state->tex_coord_location, 2, GL_FLOAT, GL_FALSE, vert_size,(void*)offset+2*sizeof(GL_FLOAT));
    glEnableVertexAttribArray(state->tex_coord_location);
    glBindTexture(GL_TEXTURE_2D, state->textures[LEFT]);
    glUniform1i(state->tex_location, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

   // Swap buffers
   swap_ogl(state->gl_state);

}

void show_right_tex_fullscreen(texture_t *state)
{
    // Size of each vertex in bytes
    size_t vert_size = 4*sizeof(GL_FLOAT);
    // Offset in our vertices array that the full screen vertices start
    size_t offset = 2*4*vert_size;

    // Draw left fractal
    glVertexAttribPointer(state->position_location, 2, GL_FLOAT, GL_FALSE, vert_size, (void*)offset);
    glEnableVertexAttribArray(state->position_location);
    glVertexAttribPointer(state->tex_coord_location, 2, GL_FLOAT, GL_FALSE, vert_size,(void*)offset+2*sizeof(GL_FLOAT));
    glEnableVertexAttribArray(state->tex_coord_location);
    glBindTexture(GL_TEXTURE_2D, state->textures[RIGHT]);
    glUniform1i(state->tex_location, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    // Swap buffers
    swap_ogl(state->gl_state);

}

void show_both_textures(texture_t *state)
{
    draw_textures(state);
    swap_ogl(state->gl_state);
}

void draw_textures(texture_t *state)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Size of each vertex in bytes
    size_t vert_size = 4*sizeof(GL_FLOAT);

    // Draw left fractal
    glVertexAttribPointer(state->position_location, 2, GL_FLOAT, GL_FALSE, vert_size, 0);
    glEnableVertexAttribArray(state->position_location);
    glVertexAttribPointer(state->tex_coord_location, 2, GL_FLOAT, GL_FALSE, vert_size,(void*)(2*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(state->tex_coord_location);
    glBindTexture(GL_TEXTURE_2D, state->textures[LEFT]);
    glUniform1i(state->tex_location, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    // Draw right fractal
    // Offset in vertices array that right texture will use
    size_t offset = 4*vert_size; 
    glVertexAttribPointer(state->position_location, 2, GL_FLOAT, GL_FALSE, vert_size, (void*)offset);
    glEnableVertexAttribArray(state->position_location);
    glVertexAttribPointer(state->tex_coord_location, 2, GL_FLOAT, GL_FALSE, vert_size,(void*)(offset+2*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(state->tex_coord_location);
    glBindTexture(GL_TEXTURE_2D, state->textures[RIGHT]);
    glUniform1i(state->tex_location, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}
