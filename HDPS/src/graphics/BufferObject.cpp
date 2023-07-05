// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

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

void BufferObject::destroy()
{
    glDeleteBuffers(1, &_object);
}

} // namespace hdps
