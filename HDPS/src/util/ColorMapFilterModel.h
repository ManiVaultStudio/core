// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ColorMap.h"

#include <QSortFilterProxyModel>

namespace mv {

namespace util {

/**
 * Color map filter model class
 *
 * @author Thomas Kroes
 */
class ColorMapFilterModel : public QSortFilterProxyModel {
public:

    /** Constructor
     * @param parent Pointer to parent object
     * @param type Type of color map
    */
    ColorMapFilterModel(QObject* parent, const ColorMap::Type& type = ColorMap::Type::OneDimensional);

    /**
     * Returns whether a give row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /** Returns the type of color map */
    ColorMap::Type getType() const;

    /**
     * Sets the type of color map
     * @param type Type of color map (e.g. 1D, 2D)
     */
    void setType(const ColorMap::Type& type);

private:
    ColorMap::Type      _type;      /** Type of color map (e.g. 1D, 2D) */
};

}
}
