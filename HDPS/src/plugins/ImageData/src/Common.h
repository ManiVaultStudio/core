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