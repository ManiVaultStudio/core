// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QString>

#include "imagedata_export.h"

enum class SelectionModifier
{
    Replace,
    Add,
    Remove,
    All,
    None,
    Invert
};

inline QString selectionModifierName(const SelectionModifier& selectionModifier)
{
    switch (selectionModifier)
    {
        case SelectionModifier::Replace:
            return "Replace";

        case SelectionModifier::Add:
            return "Add";

        case SelectionModifier::Remove:
            return "Remove";

        case SelectionModifier::All:
            return "All";

        case SelectionModifier::None:
            return "None";

        case SelectionModifier::Invert:
            return "Invert";

        default:
            break;
    }

    return "";
}
