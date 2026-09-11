#ifndef VBOMESH_H_INCLUDED
#define VBOMESH_H_INCLUDED

#include <Inventor/sys/SoGL.h>

#ifdef DEBUG
#define GLGETERROR() { GLenum err = glGetError(); if(err != GL_NO_ERROR) { char buf[256]; sprintf(buf, "GL error %d at %s:%d", err, __FILE__, __LINE__); std::cout << buf << std::endl; }}
#else
#define GLGETERROR()
#endif

class VBOMesh
{
    GLuint  m_vertexBuffer;
    GLuint  m_indexBuffer;
    GLsizei m_count;
    GLenum  m_indexType;

    // instanced rendering 
    GLuint  m_instancedDataBuffer;
    size_t  m_instanceCount;

public:

    struct InstancedData
    {
        float     propertyValue1;
        float     propertyValue2;
        SbVec3f   position;
        SbMatrix3 rotation;
    };

    VBOMesh();

    VBOMesh(GLuint vertexBuffer, GLuint indexBuffer, GLenum indexType, GLsizei count);

    void setInstancedParams(GLuint instancedDataBuffer, size_t count);

    bool valid() const;

    void free();

    void render();
};

#endif
