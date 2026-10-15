// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MultiSelectComboBox.h"

#include <QAbstractItemView>
#include <QMouseEvent>

namespace mv::gui {

MultiSelectComboBox::MultiSelectComboBox(QWidget* parent) :
    QComboBox(parent),
    _preventHidePopup(true)
{
}

void MultiSelectComboBox::init()
{
    // Install an event filter to prevent automated popup hide
    view()->installEventFilter(this);

    // Install an event filter to hide the popup when clicked outside of it
    view()->parentWidget()->installEventFilter(this);
}

void MultiSelectComboBox::hidePopup()
{
    if (_preventHidePopup)
        return;

    QComboBox::hidePopup();

    _preventHidePopup = true;
}

bool MultiSelectComboBox::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        const auto mouseEvent = dynamic_cast<QMouseEvent*>(event);

        if (watched == view()) {
            const auto index = view()->indexAt(mouseEvent->pos());

            if (index.isValid()) {
                toggleItemCheckState(index);
                return true;
            }
        }

        if (watched == view()->parentWidget()) {
            _preventHidePopup = false;
            hidePopup();
        }
    }

    return QComboBox::eventFilter(watched, event);
}

void MultiSelectComboBox::toggleItemCheckState(const QModelIndex& index) const
{
    if (index.isValid())
        model()->setData(index, !model()->data(index, Qt::CheckStateRole).toBool(), Qt::CheckStateRole);
}

}
