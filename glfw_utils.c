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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "glfw_utils.h"
#include "renderer.h"
#include "exit_menu_gl.h"
#include "start_menu_gl.h"

void check_user_input(gl_t *state)
{
    // Poll GLFW for key press or mouse input
    glfwPollEvents();
}

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}


bool window_should_close(gl_t *state)
{
    if(glfwWindowShouldClose(state->window))
	    return true;
    else
	    return false;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Get render_state from GLFW user pointer
    render_t *render_state = glfwGetWindowUserPointer(window);

    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {

        switch(key)
        { 
            case GLFW_KEY_ESCAPE:
                toggle_quit_mode(render_state);              
	        break;
            case GLFW_KEY_A:
                if(!render_state->started)
                    check_start_race(render_state);
                if(render_state->quit_mode)
                    check_exit_with_selected_program(render_state, window);
                break;
        }
    }
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Get render_state from GLFW user pointer
    render_t *render_state = glfwGetWindowUserPointer(window);

    float new_x, new_y;
    new_y = (render_state->screen_height - ypos); // Flip y = 0
    new_y = new_y/(0.5*render_state->screen_height) - 1.0;
    new_x = xpos/(0.5*render_state->screen_width) - 1.0;

    render_state->mouse_x = new_x;
    render_state->mouse_y = new_y;
//    set_mover_gl_center(render_state, new_x, new_y);
}

// scroll wheel callback
void wheel_callback(GLFWwindow* window, double x, double y)
{
    // Get render_state from GLFW user pointer
    render_t *render_state = glfwGetWindowUserPointer(window);

    // Call increase/decrease mover calls
/*
    if(y > 0.0)
    else if(y < 0.0)
    if(x > 0.0)
    else if(x < 0.0)
*/
}

// Description: Sets the display, OpenGL context and screen stuff
void init_ogl(gl_t *state, render_t *render_state)
{
    // Set error callback
    glfwSetErrorCallback(error_callback);

    // Initialize GLFW
    if(!glfwInit())
        exit(EXIT_FAILURE);

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Try to get full screen
    // Retina screen is a pain...
    int width, height;
    width = 1920;
    height = 1080;
    state->window = glfwCreateWindow(width, height, "Mandelbrot", glfwGetPrimaryMonitor(), NULL);

    glfwGetFramebufferSize(state->window, &state->screen_width, &state->screen_height);
    glViewport(0, 0, state->screen_width, state->screen_height);

    if(!state->window)
	exit(EXIT_FAILURE);

    // Set current context to window
    glfwMakeContextCurrent(state->window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    check();
    printf("ignore glewExperimental error\n");

    // Set key callback
    glfwSetKeyCallback(state->window, key_callback);

    // Set mouse cursor callback
    glfwSetCursorPosCallback(state->window, mouse_callback);

    // Set scroll wheel callback
    glfwSetScrollCallback(state->window, wheel_callback);

    // Add render state to window pointer
    // Used for key callbacks
    glfwSetWindowUserPointer(state->window, render_state);

    // Disable vsync for true FPS testing
    // Default limit 60 fps
//    glfwSwapInterval(0);

    // Set background color and clear buffers
    glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
    glClear( GL_COLOR_BUFFER_BIT );
}

void swap_ogl(gl_t *state)
{
    glfwSwapBuffers(state->window);
}

void exit_ogl(gl_t *state)
{
//    glDeleteProgram(state->shaderProgram);
//    glDeleteShader(fragmentShader);
//    glDeleteShader(vertexShader);
//    glDeleteBuffers(1, &vbo);
//    glDeleteVertexArrays(1, &vao);

    glfwDestroyWindow(state->window);
    glfwTerminate();

    printf("close\n");
}

// Convert pixel coordinates, lower left origin, to gl coordinates, center origin
void pixel_to_gl(gl_t *state, int pixel_x, int pixel_y, float *gl_x, float *gl_y)
{
    float half_x = state->screen_width/2.0;
    float half_y = state->screen_height/2.0;
    *gl_x = pixel_x/half_x - 1.0;
    *gl_y = pixel_y/half_y - 1.0;

}

// Exit and set return value for specific program if one selected
void check_exit_with_selected_program(render_t *render_state, GLFWwindow* window)
{
    if(render_state->exit_menu_state->mandelbrot_state->selected) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        render_state->return_value = 10;
    }
    else if (render_state->exit_menu_state->sph_state->selected) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        render_state->return_value = 20;
    }
    else if (render_state->exit_menu_state->terminal_state->selected) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        render_state->return_value = 0;
    }
}

void check_start_race(render_t *render_state)
{
    if(render_state->start_menu_state->start_state->selected && render_state->started == false)
        render_state->started = true;
}

void toggle_quit_mode(render_t *state)
{
    state->quit_mode = !state->quit_mode;
}
