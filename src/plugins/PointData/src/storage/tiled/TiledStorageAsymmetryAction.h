// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <PointData/pointdata_export.h>

#include <actions/HorizontalGroupAction.h>
#include <actions/IntegralAction.h>
#include <actions/StringAction.h>

namespace mv::gui
{

/**
 * @brief Displays and controls the row-to-column tile asymmetry.
 *
 * The action places explanatory row and column labels around a live slider so
 * that the meaning of the two endpoints remains visible wherever tiled storage
 * configuration is presented. The value is expressed as a percentage from
 * column-oriented at zero to row-oriented at one hundred.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT TiledStorageAsymmetryAction final : public HorizontalGroupAction
{
    Q_OBJECT

public:
    /** Constructs a row-to-column asymmetry control. */
    TiledStorageAsymmetryAction(QObject* parent, const QString& title = "Tile asymmetry");

    /** Returns the row-to-column asymmetry slider action. */
    IntegralAction& getAsymmetryAction();

    /** Returns the row-to-column asymmetry slider action. */
    const IntegralAction& getAsymmetryAction() const;

private:
    StringAction   _columnLabelAction;
    IntegralAction _asymmetryAction;
    StringAction   _rowLabelAction;
};

}
