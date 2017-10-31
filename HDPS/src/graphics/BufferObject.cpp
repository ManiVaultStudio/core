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
    initializeOpenGLFunctions();
    glGenBuffers(1, &_object);
}

void BufferObject::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, _object);
}

template<typename T>
void BufferObject::setData(const std::vector<T>& data)
{
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
}

void BufferObject::destroy()
{
    glDeleteBuffers(1, &_object);
}

} // namespace hdps
