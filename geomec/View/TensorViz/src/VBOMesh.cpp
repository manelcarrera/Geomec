#include "VBOMesh.h"
#include "BeachBall.h"
#include "stdafx.h"

VBOMesh::VBOMesh()
    : m_vertexBuffer(0), m_indexBuffer(0), m_count(0), m_indexType(GL_UNSIGNED_INT), m_instancedDataBuffer(0),
      m_instanceCount(0) {}

VBOMesh::VBOMesh(GLuint vertexBuffer, GLuint indexBuffer, GLenum indexType, GLsizei count)
    : m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer), m_count(count), m_indexType(indexType),
      m_instancedDataBuffer(0), m_instanceCount(0) {}

void VBOMesh::setInstancedParams(GLuint instancedDataBuffer, size_t instanceCount) {
  m_instancedDataBuffer = instancedDataBuffer;
  m_instanceCount = instanceCount;
}

bool VBOMesh::valid() const { return m_vertexBuffer != 0 && m_indexBuffer != 0 && m_count != 0; }

void VBOMesh::free() {
  GLuint buffers[] = {m_vertexBuffer, m_indexBuffer, m_instancedDataBuffer};

  glDeleteBuffers(3, buffers);

  m_vertexBuffer = 0;
  m_indexBuffer = 0;
  m_instancedDataBuffer = 0;
}

void VBOMesh::render() {
  GLuint prevVAO;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint *)&prevVAO);
  // glBindVertexArray(0);

  // These values must match the locations specified in the shader, using the layout qualifier
  const GLuint positionAttribIndex = 0;       // vertex position
  const GLuint normalAttribIndex = 1;         // vertex normal
  const GLuint colorAttribIndex = 2;          // vertex color
  const GLuint propertyValue1AttribIndex = 3; // primary property value for beachball (instanced)
  const GLuint propertyValue2AttribIndex = 4; // secondary property value for beachball (instanced)
  const GLuint centerAttribIndex = 5;         // position of beachball (instanced)
  const GLuint rotationAttribIndex = 6;       // rotation of beachball (instanced)
  const GLuint totalAttribIndices = 9;        // rotationAttribIndex takes up 3 slots

  GLuint prevArrayBuffer, prevElementArrayBuffer;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint *)&prevArrayBuffer);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint *)&prevElementArrayBuffer);
  GLGETERROR();

  // Enable all vertex arrays that we're going to use
  for (int i = 0; i < totalAttribIndices; ++i)
    glEnableVertexAttribArray(i);
  GLGETERROR();

  // Setup the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

  GLsizei stride = sizeof(Vertex);
  glVertexAttribPointer(positionAttribIndex, 3, GL_FLOAT, false, stride, (const GLvoid *)0);
  glVertexAttribPointer(normalAttribIndex, 3, GL_FLOAT, false, stride, (const GLvoid *)12);
  glVertexAttribPointer(colorAttribIndex, 1, GL_FLOAT, false, stride, (const GLvoid *)24);
  GLGETERROR();

  // Setup the instanced data
  glBindBuffer(GL_ARRAY_BUFFER, m_instancedDataBuffer);

  stride = sizeof(InstancedData);
  glVertexAttribPointer(propertyValue1AttribIndex, 1, GL_FLOAT, false, stride, (const GLvoid *)0);
  glVertexAttribPointer(propertyValue2AttribIndex, 1, GL_FLOAT, false, stride, (const GLvoid *)4);
  glVertexAttribPointer(centerAttribIndex, 3, GL_FLOAT, false, stride, (const GLvoid *)8);

  for (int i = 0; i < 3; ++i)
    glVertexAttribPointer(rotationAttribIndex + i, 3, GL_FLOAT, false, stride,
                          (const GLvoid *)(20 + 12 * (long long)i));

  for (int i = 0; i < 6; ++i)
    glVertexAttribDivisor(propertyValue1AttribIndex + i, 1);

  GLGETERROR();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
  GLGETERROR();

  glDrawElementsInstanced(GL_TRIANGLES, m_count, m_indexType, 0, (GLsizei)m_instanceCount);
  GLGETERROR();

  // Reset vertexAttribDivisor for instanced data
  for (int i = 0; i < 6; ++i)
    glVertexAttribDivisor(propertyValue1AttribIndex + i, 0);

  for (int i = 0; i < totalAttribIndices; ++i)
    glDisableVertexAttribArray(i);

  GLGETERROR();

  glBindBuffer(GL_ARRAY_BUFFER, prevArrayBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementArrayBuffer);
  GLGETERROR();
}
