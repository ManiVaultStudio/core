#include "VBO.h"

namespace hdps
{

BufferObject::BufferObject()
{
    glGenBuffers(1, &_object);
}

BufferObject::~BufferObject()
{
    glDeleteBuffers(1, &_object);
}

void BufferObject::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, _object);
}

void BufferObject::setData(std::vector<float>& data)
{
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
}

} // namespace hdps
