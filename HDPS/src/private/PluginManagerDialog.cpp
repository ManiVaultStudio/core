#include "PluginManagerDialog.h"

#include <Application.h>
#include <Plugin.h>

#include <QVBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <QHeaderView>

using namespace hdps;
using namespace hdps::gui;

#ifdef _DEBUG
    #define PLUGIN_MANAGER_DIALOG_VERBOSE
#endif

PluginManagerDialog::PluginManagerDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Plugin", _model, &_filterModel)
{
    const auto pluginIcon = Application::getIconFont("FontAwesome").getIcon("plug");

    setWindowIcon(pluginIcon);
    setModal(true);
    setWindowTitle("Plugin manager");
    
    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(pluginIcon);
    _hierarchyWidget.getTreeView().setRootIsDecorated(true);

    _hierarchyWidget.getFilterGroupAction() << _filterModel.getShowOnlyLoadedAction();

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setColumnHidden(1, true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->resizeSection(static_cast<int>(0), 300);
    treeViewHeader->resizeSection(static_cast<int>(1), 100);

    treeViewHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(1, QHeaderView::Fixed);

    connect(&treeView, &QTreeView::customContextMenuRequested, [this](const QPoint& point)
        {
            const auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();

            if (selectedRows.isEmpty())
                return;

            auto mayDestroyPlugins = true;

            for (const auto& selectedRow : selectedRows) {
                if (selectedRow.siblingAtColumn(1).data().toString() != "Instance") {
                    mayDestroyPlugins = false;
                    break;
                }
            }

            if (!mayDestroyPlugins)
                return;

            QMenu contextMenu;

            contextMenu.addAction(QString("Destroy %1 plugins").arg(QString::number(selectedRows.count())), [this, selectedRows] {
                for (const auto& selectedRow : selectedRows) {
                    Application::core()->getPluginManager().destroyPlugin(selectedRow.data(Qt::UserRole + 1).value<plugin::Plugin*>());

                    _model.removeItem(_filterModel.mapToSource(selectedRow));
                }
            });

            contextMenu.exec(QCursor::pos());
        });
}