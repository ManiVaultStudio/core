#include "VBO.h"

namespace hdps
{

VBO::VBO()
{
    glGenBuffers(1, &_object);
}

VBO::~VBO()
{
    glDeleteBuffers(1, &_object);
}

void VBO::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, _object);
}

void VBO::setData(std::vector<float>& data)
{
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
}

} // namespace hdps
