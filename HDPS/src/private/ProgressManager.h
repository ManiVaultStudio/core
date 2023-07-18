// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractProgressManager.h"

namespace hdps
{

class ProgressManager final : public AbstractProgressManager
{
public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    ProgressManager(QObject* parent = nullptr);

    /** Reset when destructed */
    ~ProgressManager();

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the data hierarchy manager */
    void reset() override;
};

}
