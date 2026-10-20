// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/StandardItemModel.h"

#include <QMap>

namespace mv {

/**
 * Abstract hardware spec model class
 *
 * Base standard item model for hardware specification.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractHardwareSpecModel : public StandardItemModel
{
    Q_OBJECT

public:

    /** Model columns */
    enum class Column {
        Title,          /** Title of the hardware component (parameter) item */
        SystemValue,    /** System value of the hardware component (parameter) item */
        RequiredValue,  /** Required value of the hardware component (parameter) item */

        Count           /** Number of columns in the model */
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractHardwareSpecModel(QObject* parent = nullptr);

private:
};

}
