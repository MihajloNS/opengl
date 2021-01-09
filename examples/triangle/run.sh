#!/bin/bash

export LD_LIBRARY_PATH=${PWD}/../../dependencies/glfw/lib/:${PWD}/../../dependencies/glew/lib/

# Override opengl implementation version
export MESA_GL_VERSION_OVERRIDE=3.3

export SHADER_PATH=${PWD}/res/Basic.shader

./triangle
