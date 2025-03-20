// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Renderer.h"

namespace mv
{
    class CORE_EXPORT ImageRenderer : public Renderer
    {
    public:
        void init() override;
        void resize(QSize renderSize) override;
        void render() override;
        void destroy() override;
    private:
        ShaderProgram _shader;
    };
} // namespace mv
