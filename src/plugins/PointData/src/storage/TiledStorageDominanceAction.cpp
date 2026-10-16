// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageDominanceAction.h"

namespace mv::gui
{

TiledStorageDominanceAction::TiledStorageDominanceAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _columnLabelAction(this, "Column", "Column"),
    _dominanceAction(this, "Asymmetry", 0, 100, 50),
    _rowLabelAction(this, "Row", "Row")
{
    setShowLabels(false);

    _columnLabelAction.setDefaultWidgetFlags(StringAction::Label);
    _rowLabelAction.setDefaultWidgetFlags(StringAction::Label);
    _dominanceAction.setDefaultWidgetFlags(IntegralAction::Slider);
    _dominanceAction.setStretch(1);
    _dominanceAction.setUpdateDuringDrag(true);
    _dominanceAction.setToolTip("0 = column-oriented tiles, 50 = balanced tiles, 100 = row-oriented tiles");

    addAction(&_columnLabelAction);
    addAction(&_dominanceAction);
    addAction(&_rowLabelAction);
}

IntegralAction& TiledStorageDominanceAction::getDominanceAction()
{
    return _dominanceAction;
}

const IntegralAction& TiledStorageDominanceAction::getDominanceAction() const
{
    return _dominanceAction;
}

}
