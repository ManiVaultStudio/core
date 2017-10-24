#include "BufferObject.h"

namespace hdps
{

BufferObject::BufferObject()
{

}

BufferObject::~BufferObject()
{

}

void BufferObject::create()
{
    glGenBuffers(1, &_object);
}

void BufferObject::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, _object);
}

void BufferObject::setData(const std::vector<float>& data)
{
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
}

void BufferObject::destroy()
{
    glDeleteBuffers(1, &_object);
}

} // namespace hdps
