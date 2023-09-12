// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionWidget.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>

namespace hdps::gui {

WidgetActionWidget::WidgetActionWidget(QWidget* parent, WidgetAction* action,  std::int32_t widgetFlags /*= 0*/) :
    WidgetActionViewWidget(parent, action, widgetFlags)
{
    setAction(action);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

QSize WidgetActionWidget::sizeHint() const
{
    if (getWidgetFlags() & WidgetFlag::PopupLayout) {
        auto popupSizeHint = const_cast<WidgetActionWidget*>(this)->getAction()->getPopupSizeHint();

        if (!popupSizeHint.isNull())
            return popupSizeHint;
    }

    return QWidget::sizeHint();
}

void WidgetActionWidget::setLayout(QLayout* layout)
{
    if (isPopup())
        setPopupLayout(layout);
    else
        WidgetActionViewWidget::setLayout(layout);
}

void WidgetActionWidget::setPopupLayout(QLayout* popupLayout)
{
    auto mainLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(4, 4, 4, 4);

    WidgetActionViewWidget::setLayout(mainLayout);

    if (getWidgetFlags() & WidgetActionViewWidget::NoGroupBoxInPopupLayout) {
        mainLayout->addLayout(popupLayout);
    }
    else {
        auto groupBox = new QGroupBox(getAction()->text());

        groupBox->setLayout(popupLayout);
        groupBox->setCheckable(getAction()->isCheckable());

        mainLayout->addWidget(groupBox);

        const auto update = [this, groupBox]() -> void {
            QSignalBlocker blocker(getAction());

            groupBox->setTitle(getAction()->text());
            groupBox->setToolTip(getAction()->text());
            groupBox->setChecked(getAction()->isChecked());
        };

        connect(groupBox, &QGroupBox::toggled, this, [this](bool toggled) {
            getAction()->setChecked(toggled);
        });

        connect(getAction(), &WidgetAction::changed, this, [update]() {
            update();
        });

        update();
    }
}

bool WidgetActionWidget::isPopup() const
{
    return getWidgetFlags() & PopupLayout;
}

}
