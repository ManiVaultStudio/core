#pragma once

#include <QOpenGLFunctions_3_3_Core>

#include <vector>

namespace hdps
{

class BufferObject : protected QOpenGLFunctions_3_3_Core
{
public:
    BufferObject();
    ~BufferObject();

    void create();
    void bind();
    template<typename T>
    void setData(const std::vector<T>& data)
    {
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    }

    void destroy();
private:
    GLuint _object;
};

} // namespace hdps
