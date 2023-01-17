#include "StartPageActionsWidget.h"
#include "StartPageActionDelegate.h"
#include "StartPageWidget.h"

#include <Application.h>

#include <QDebug>
#include <QHeaderView>
#include <QBuffer>

using namespace hdps;
using namespace hdps::gui;

StartPageActionsWidget::StartPageActionsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Item", _model, &_filterModel, true, false)    
{
    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _filterModel.setFilterKeyColumn(static_cast<int>(StartPageActionsModel::Column::Title));

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _hierarchyWidget.setWindowIcon(fontAwesome.getIcon("play"));

    _hierarchyWidget.getFilterGroupAction().setVisible(false);
    _hierarchyWidget.getCollapseAllAction().setVisible(false);
    _hierarchyWidget.getExpandAllAction().setVisible(false);
    _hierarchyWidget.getColumnsGroupAction().setVisible(false);
    _hierarchyWidget.getSelectionGroupAction().setVisible(false);
    _hierarchyWidget.setHeaderHidden(true);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setRootIsDecorated(false);
    treeView.setItemDelegateForColumn(static_cast<int>(StartPageActionsModel::Column::SummaryDelegate), new StartPageActionDelegate());
    treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView.setSelectionMode(QAbstractItemView::SingleSelection);
    treeView.setIconSize(QSize(24, 24));
    treeView.setMouseTracking(true);
    treeView.setStyleSheet(" \
        QTreeView { \
            border: none; \
        } \
        QToolTip { \
            min-width: 150; \
        } \
    ");

    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Icon), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Title), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Description), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Comments), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Tags), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::PreviewImage), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Tooltip), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::ClickedCallback), true);

    QPalette treeViewPalette(treeView.palette());

    QStyleOption styleOption;

    styleOption.initFrom(&treeView);

    treeViewPalette.setColor(QPalette::Base, styleOption.palette.color(QPalette::Normal, QPalette::Midlight));

    treeView.setPalette(treeViewPalette);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSectionResizeMode(static_cast<int>(StartPageActionsModel::Column::SummaryDelegate), QHeaderView::Stretch);

    connect(&treeView, &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        auto callback = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::ClickedCallback)).data(Qt::UserRole + 1).value<StartPageActionsModel::ClickedCB>();
        callback();

        _hierarchyWidget.getSelectionModel().clear();
    });

    connect(&treeView, &QTreeView::entered, this, [this](const QModelIndex& index) -> void {
        const auto title        = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Title)).data(Qt::EditRole).toString();
        const auto description  = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Description)).data(Qt::EditRole).toString();
        const auto previewImage = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::PreviewImage)).data(Qt::UserRole + 1).value<QImage>();
        const auto tags         = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Tags)).data(Qt::EditRole).toStringList();
        const auto tooltip      = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Tooltip)).data(Qt::EditRole).toString();

        QString tooltipTextHtml;

        if (!previewImage.isNull()) {
            QBuffer buffer;

            buffer.open(QIODevice::WriteOnly);

            QPixmap::fromImage(previewImage).save(&buffer, "JPG");

            auto image = buffer.data().toBase64();

            tooltipTextHtml = QString("<img style='padding: 100px;'src='data:image/jpg;base64,%1'></p>").arg(image);
        }
        else {
            tooltipTextHtml = tooltip;
        }

        auto tooltipHtml = QString(" \
            <html> \
                <head> \
                    <style> \
                        body { \
                            width: 250px; \
                        } \
                    </style> \
                </head> \
                <body> \
                    <div style='width: 300px;'>%1</div> \
                </body> \
            </html> \
        ").arg(tooltipTextHtml);

        _model.item(index.row(), index.column())->setToolTip(tooltipHtml);
    });
}

StartPageActionsModel& StartPageActionsWidget::getModel()
{
    return _model;
}

HierarchyWidget& StartPageActionsWidget::getHierarchyWidget()
{
    return _hierarchyWidget;
}
