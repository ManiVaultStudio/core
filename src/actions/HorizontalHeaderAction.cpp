// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HorizontalHeaderAction.h"

#include "OptionsAction.h"

#include <QEvent>
#include <QHeaderView>

namespace mv::gui {

HorizontalHeaderAction::HorizontalHeaderAction(QObject* parent, const QString& title, QHeaderView* horizontalHeaderView /*= nullptr*/) :
    VerticalGroupAction(parent, title),
    _horizontalHeaderView(nullptr),
    _columnsAction(new OptionsAction(this, "table-columns")),
    _showHeaderAction(this, "Visible")
{
    setText(title);
    setToolTip("Columns toggle");
    setIconByName("table");
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    setPopupSizeHint(QSize(200, 0));

    addAction(&_showHeaderAction);
    addAction(_columnsAction);

    if (horizontalHeaderView)
        initialize(horizontalHeaderView);
}

void HorizontalHeaderAction::initialize(QHeaderView* horizontalHeaderView)
{
    Q_ASSERT(horizontalHeaderView);

    if (!horizontalHeaderView)
        return;

    _horizontalHeaderView = horizontalHeaderView;

    connect(_horizontalHeaderView, &QHeaderView::sectionResized, this, &HorizontalHeaderAction::updateColumnsOptionsAction);

    connect(&_showHeaderAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
        _horizontalHeaderView->setVisible(toggled);
    });

    connect(_columnsAction, &OptionsAction::selectedOptionsChanged, this, [this](const QStringList& selectedOptions) -> void {
        std::int32_t currentSectionIndex = 0;

        for (const auto& option : _columnsAction->getOptions())
            _horizontalHeaderView->setSectionHidden(++currentSectionIndex, !selectedOptions.contains(option));
    });

    updateColumnsOptionsAction();

    _horizontalHeaderView->installEventFilter(this);
}

bool HorizontalHeaderAction::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Show:
        case QEvent::Hide:
        {
            if (target == _horizontalHeaderView)
                _showHeaderAction.setChecked(_horizontalHeaderView->isVisible());

            break;
        }

        default:
            break;
    }

    return VerticalGroupAction::eventFilter(target, event);
}

void HorizontalHeaderAction::updateColumnsOptionsAction()
{
    QStringList columnsOptions, selectedColumnsOptions;

    for (int columnIndex = 0; columnIndex < _horizontalHeaderView->count(); ++columnIndex) {
        const auto columnName = _horizontalHeaderView->model()->headerData(columnIndex, Qt::Horizontal).toString();

        columnsOptions << columnName;

        if (!_horizontalHeaderView->isSectionHidden(columnIndex))
            selectedColumnsOptions << columnName;
    }

    _columnsAction->initialize(columnsOptions, selectedColumnsOptions);
}

}
