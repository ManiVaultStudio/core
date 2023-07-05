// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Texture.h"

#include <QOpenGLFunctions_3_3_Core>

#include <vector>
#include <QDebug>

namespace hdps
{
    class Framebuffer : protected QOpenGLFunctions_3_3_Core
    {
    public:
        Framebuffer() :
            _handle(0),
            colorTexture{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
            depthTexture(nullptr)
        {

        }

        void create()
        {
            initializeOpenGLFunctions();

            glGenFramebuffers(1, &_handle);
        }

        void bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, _handle);
        }

        void release() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void destroy()
        {
            glDeleteFramebuffers(1, &_handle);
        }

        Texture2D& getColorTexture(int attachment) const {
            return *colorTexture[attachment];
        }

        void addColorTexture(int colorAttachment, Texture2D* texture) {
            colorTexture[colorAttachment] = texture;
            GLint attachment = GL_COLOR_ATTACHMENT0 + colorAttachment;
            setTexture(attachment, *texture);
            addDrawBuffer(attachment);
        }

        void setTexture(GLuint attachment, Texture2D& texture) {
            glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture.getHandle(), 0);
        }

        void addDrawBuffer(GLenum target) {
            drawBuffers.push_back(target);
            glDrawBuffers((GLsizei) drawBuffers.size(), drawBuffers.data());
        }

        void enableColor(int target) {
            glReadBuffer(target);
            glDrawBuffer(target);
        }

        void disableColor() {
            glReadBuffer(GL_NONE);
            glDrawBuffer(GL_NONE);
        }

        void validate() {
            GLuint error = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            if (error != GL_FRAMEBUFFER_COMPLETE) {
                switch (error) {
                case GL_FRAMEBUFFER_UNDEFINED:
                    qDebug() << "Target is the default framebuffer, but the default framebuffer does not exist"; break;
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    qDebug() << "Any of the framebuffer attachment points are framebuffer incomplete"; break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    qDebug() << "The framebuffer does not have any texture attached to it"; break;
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                    qDebug() << "The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi"; break;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                    qDebug() << "GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER"; break;
                case GL_FRAMEBUFFER_UNSUPPORTED:
                    qDebug() << "The combination of internal formats of the attached textures violates an implementation-dependent set of restrictions."; break;
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                    qDebug() << "The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers"; break;
                default:
                    qDebug() << "There is a problem with the framebuffer";
                }
            }
        }
    private:
        GLuint _handle;

        Texture2D* colorTexture[8];
        Texture2D* depthTexture;

        std::vector<GLenum> drawBuffers;
    };
} // namespace hdps
