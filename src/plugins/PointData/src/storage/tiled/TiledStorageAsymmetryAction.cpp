// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageAsymmetryAction.h"

namespace mv::gui
{

TiledStorageAsymmetryAction::TiledStorageAsymmetryAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _columnLabelAction(this, "Column", "Column"),
    _asymmetryAction(this, "Asymmetry", 0, 100, 50),
    _rowLabelAction(this, "Row", "Row")
{
    setShowLabels(false);

    _columnLabelAction.setDefaultWidgetFlags(StringAction::Label);
    _rowLabelAction.setDefaultWidgetFlags(StringAction::Label);
    _asymmetryAction.setDefaultWidgetFlags(IntegralAction::Slider);
    _asymmetryAction.setStretch(1);
    _asymmetryAction.setUpdateDuringDrag(true);
    _asymmetryAction.setToolTip("0 = column-oriented tiles, 50 = balanced tiles, 100 = row-oriented tiles");

    addAction(&_columnLabelAction);
    addAction(&_asymmetryAction);
    addAction(&_rowLabelAction);
}

IntegralAction& TiledStorageAsymmetryAction::getAsymmetryAction()
{
    return _asymmetryAction;
}

const IntegralAction& TiledStorageAsymmetryAction::getAsymmetryAction() const
{
    return _asymmetryAction;
}

}
