#include "Utils.h"

void ClearGLError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool PrintGLError()
{
    bool NoError = true;
    while (GLenum error = glGetError()) {

        std::cout << error << "\n";
        NoError = false;
    }
    return NoError;
}