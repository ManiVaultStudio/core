// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MultiSelectComboBox.h"

namespace mv::gui {

MultiSelectComboBox::MultiSelectComboBox(QWidget* parent) :
	QComboBox(parent),
    _preventHidePopup(true)
{
    setEditable(true);

    // Connect the activated signal to handle item toggling
    
    updateDisplayText();
}

void MultiSelectComboBox::init()
{
    // Install an event filter to prevent automated popup hide
    view()->installEventFilter(this);
    view()->parentWidget()->installEventFilter(this);
    // Install an event filter to track clicks outside
    //QApplication::instance()->installEventFilter(this);
}

void MultiSelectComboBox::showPopup()
{
    QComboBox::showPopup();

    _popupOpen = true;
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
        //auto* mouseEvent = static_cast<QMouseEvent*>(event);
        //QWidget* popup = view()->window(); // Get the popup window
        //if (popup && _popupOpen && !popup->geometry().contains(mouseEvent->globalPos())) {
        //    hidePopup(); // Close the popup if the click is outside
        //}

        const auto mouseEvent = dynamic_cast<QMouseEvent*>(event);

        if (watched == view()) {
            qDebug() << "Clicked inside";
            
            const auto index        = view()->indexAt(mouseEvent->pos());

            if (index.isValid()) {
                toggleItemCheckState(index);

                return true;
            }
        } else {
            qDebug() << "Clicked outside";
            _preventHidePopup = false;
            hidePopup();
        }
    }

    if (event->type() == QEvent::FocusOut)
        qDebug() << "Focus out";

    return QComboBox::eventFilter(watched, event);
}

void MultiSelectComboBox::toggleItemCheckState(const QModelIndex& index) const
{
    if (index.isValid())
        model()->setData(index, !model()->data(index, Qt::CheckStateRole).toBool(), Qt::CheckStateRole);
}

void MultiSelectComboBox::updateDisplayText()
{
    QStringList selectedItems;

    auto* model = dynamic_cast<QStandardItemModel*>(this->model());

	for (int i = 0; i < model->rowCount(); ++i) {
        auto* item = model->item(i);

        if (item && item->checkState() == Qt::Checked) {
            selectedItems << item->text();
        }
    }

	setEditText(selectedItems.join(", "));
}

}
