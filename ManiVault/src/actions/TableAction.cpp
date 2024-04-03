// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TableAction.h"

#include <QVBoxLayout>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

namespace mv::gui {

TableAction::TableAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _model(nullptr),
    _filterModel(nullptr),
    _selectionModel(),
    _toolbarGroupAction(this, "Toolbar"),
    _modelFilterAction(this, "Filtering"),
    _modelSelectionAction(this, "Selection"),
    _showHeaderSettingsAction(this, "Show header settings", true)
{
    _toolbarGroupAction.setShowLabels(false);

    _modelFilterAction.setStretch(1);

    _toolbarGroupAction.addAction(&getModelFilterAction());
    _toolbarGroupAction.addAction(&getModelSelectionAction());
}

void TableAction::initialize(QAbstractItemModel* model, QSortFilterProxyModel* filterModel, const QString& itemTypeName)
{
    Q_ASSERT(model != nullptr);

    if (model == nullptr)
        return;

    _model          = model;
    _filterModel    = filterModel;
    _itemTypeName   = itemTypeName;

    if (_filterModel) {
        _filterModel->setSourceModel(_model);
        _selectionModel.setModel(_filterModel);
    }
    else {
        _selectionModel.setModel(_model);
    }

    _modelFilterAction.initialize(_model, _filterModel, _itemTypeName);
    _modelSelectionAction.initialize(&_selectionModel);
}

TableAction::Widget::Widget(QWidget* parent, TableAction* tableAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, tableAction, widgetFlags),
    _tableAction(tableAction),
    _tableView(),
    _infoOverlayWidget(&_tableView),
    _noItemsDescription(QString("No %1s available").arg(_tableAction->getItemTypeName())),
    _horizontalHeaderAction(this, "Header", _tableView.horizontalHeader())
{
    setWindowIcon(tableAction->icon());

    _tableAction->getToolbarGroupAction().addAction(&_horizontalHeaderAction, GroupAction::Vertical);

    _tableView.setObjectName("TableView");

    if (auto filterModel = tableAction->getFilterModel())
        _tableView.setModel(filterModel);
    else
        _tableView.setModel(tableAction->getModel());

    _tableView.setSelectionModel(&tableAction->getSelectionModel());

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(_tableAction->getToolbarGroupAction().createWidget(this));
    layout->addWidget(&_tableView);

    setLayout(layout);

    auto& widgetFader = _infoOverlayWidget.getWidgetFader();

    widgetFader.setOpacity(0.0f);
    widgetFader.setMaximumOpacity(0.5f);
    widgetFader.setFadeInDuration(100);
    widgetFader.setFadeOutDuration(300);

    auto filterModel = _tableAction->getFilterModel();

    connect(filterModel, &QAbstractItemModel::rowsInserted, this, &Widget::updateOverlayWidget);
    connect(filterModel, &QAbstractItemModel::rowsRemoved, this, &Widget::updateOverlayWidget);
    connect(filterModel, &QAbstractItemModel::layoutChanged, this, &Widget::updateOverlayWidget);

    const auto updateHeaderActionVisibility = [this]() -> void {
        _horizontalHeaderAction.setVisible(_tableAction->getShowHeaderSettingsAction().isChecked());
    };

    updateHeaderActionVisibility();

    connect(&tableAction->getShowHeaderSettingsAction(), &ToggleAction::toggled, this, updateHeaderActionVisibility);
}

void TableAction::Widget::updateOverlayWidget()
{
    auto& widgetFader = _infoOverlayWidget.getWidgetFader();

    auto filterModel    = _tableAction->getFilterModel();
    auto model          = _tableAction->getModel();

    const auto& itemTypeName = _tableAction->getItemTypeName();

    if (!filterModel) {
        if (model->rowCount() == 0) {
            _infoOverlayWidget.set(windowIcon(), QString("No %1s to display").arg(itemTypeName.toLower()), _noItemsDescription);
            widgetFader.fadeIn();
        }
        else {
            widgetFader.fadeOut();
        }
    }
    else {
        if (model->rowCount() >= 1) {
            if (filterModel->rowCount() == 0) {
                _infoOverlayWidget.set(windowIcon(), QString("No %1s found").arg(itemTypeName.toLower()), (_tableAction->getToolbarGroupAction().isVisible() ? "Try changing the filter parameters..." : ""));
                widgetFader.fadeIn();
            }
            else {
                _infoOverlayWidget.set(windowIcon(), QString("No %1s found").arg(itemTypeName.toLower()), "");
                widgetFader.fadeOut();
            }
        }
        else {
            _infoOverlayWidget.set(windowIcon(), QString("No %1s to display").arg(itemTypeName.toLower()), _noItemsDescription);
            widgetFader.fadeIn();
        }
    }
}

}
