// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractColorSchemesModel.h"

namespace mv {

/**
 * Color schemes list model class
 *
 * List model for color schemes
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ColorSchemesListModel : public AbstractColorSchemesModel
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ColorSchemesListModel(QObject* parent = nullptr);

    /**
     * Add \p colorScheme to the model
     * @param colorScheme Color scheme
     */
    void addColorScheme(util::ColorScheme colorScheme);

    /**
     * Remove color scheme with \p colorSchemeName from the model
     * @param colorSchemeName Color scheme name
     */
    void removeColorScheme(const QString& colorSchemeName);

protected:

    /** Add built-in color schemes */
    void addBuiltInColorSchemes();

private:
    util::ColorSchemes  _colorSchemes;  /** Color schemes */

    friend class AbstractThemeManager;
};

}