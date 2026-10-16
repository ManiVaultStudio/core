// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pointdata_export.h"

#include <actions/HorizontalGroupAction.h>
#include <actions/IntegralAction.h>
#include <actions/StringAction.h>

namespace mv::gui
{

/**
 * @brief Displays and controls the row-to-column tile dominance preference.
 *
 * The action places explanatory row and column labels around a live slider so
 * that the meaning of the two endpoints remains visible wherever tiled storage
 * configuration is presented. The value is expressed as a percentage from
 * column-dominant at zero to row-dominant at one hundred.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT TiledStorageDominanceAction final : public HorizontalGroupAction
{
    Q_OBJECT

public:
    /** Constructs a row-to-column dominance control. */
    TiledStorageDominanceAction(QObject* parent, const QString& title = "Tile dominance");

    /** Returns the row-to-column dominance slider action. */
    IntegralAction& getDominanceAction();

    /** Returns the row-to-column dominance slider action. */
    const IntegralAction& getDominanceAction() const;

private:
    StringAction   _columnLabelAction;
    IntegralAction _dominanceAction;
    StringAction   _rowLabelAction;
};

}
