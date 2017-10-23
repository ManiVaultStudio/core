#ifndef VBO_H
#define VBO_H

#include <QOpenGLFunctions_3_3_Core>

#include <vector>

namespace hdps
{

class VBO : protected QOpenGLFunctions_3_3_Core
{
public:
    VBO();
    ~VBO();

    void bind();
    void setData(std::vector<float>& data);
private:
    GLuint _object;
};

} // namespace hdps

#endif // VBO_H
