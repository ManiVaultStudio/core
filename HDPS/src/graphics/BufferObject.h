#ifndef BUFFER_OBJECT_H
#define BUFFER_OBJECT_H

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
    void setData(const std::vector<float>& data);
    void destroy();
private:
    GLuint _object;
};

} // namespace hdps

#endif // BUFFER_OBJECT_H
