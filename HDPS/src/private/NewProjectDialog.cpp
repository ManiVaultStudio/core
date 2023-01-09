#include "NewProjectDialog.h"

#include <AbstractWorkspaceManager.h>

#include <Application.h>

#include <QVBoxLayout>
#include <QPainter>
#include <QHeaderView>

#ifdef _DEBUG
    #define NEW_PROJECT_DIALOG_VERBOSE
#endif

using namespace hdps;
using namespace hdps::gui;

NewProjectDialog::NewProjectDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Workspace", _model, &_filterModel),
    _createAction(this, "Create"),
    _cancelAction(this, "Cancel")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    setModal(true);
    setWindowTitle("New Project");

    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_createAction.createWidget(this));
    bottomLayout->addWidget(_cancelAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    _filterModel.setFilterKeyColumn(static_cast<int>(WorkspaceInventoryModel::Column::Name));

    _hierarchyWidget.setWindowIcon(Application::core()->getWorkspaceManager().getIcon());
    _hierarchyWidget.getFilterGroupAction().setVisible(false);
    _hierarchyWidget.getCollapseAllAction().setVisible(false);
    _hierarchyWidget.getExpandAllAction().setVisible(false);
    _hierarchyWidget.getColumnsGroupAction().setVisible(false);
    _hierarchyWidget.getSelectionGroupAction().setVisible(false);
    _hierarchyWidget.setHeaderHidden(true);

    const auto updateActions = [this]() -> void {
        _createAction.setEnabled(_hierarchyWidget.getSelectionModel().hasSelection());
    };

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, updateActions);

    updateActions();

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setRootIsDecorated(false);
    treeView.setItemDelegateForColumn(static_cast<int>(WorkspaceInventoryModel::Column::Summary), new WorkspaceDelegate());
    treeView.setSelectionMode(QAbstractItemView::SingleSelection);
    treeView.setIconSize(QSize(24, 24));

    treeView.setColumnHidden(static_cast<int>(WorkspaceInventoryModel::Column::Icon), true);
    treeView.setColumnHidden(static_cast<int>(WorkspaceInventoryModel::Column::Name), true);
    treeView.setColumnHidden(static_cast<int>(WorkspaceInventoryModel::Column::Description), true);
    treeView.setColumnHidden(static_cast<int>(WorkspaceInventoryModel::Column::Tags), true);
    treeView.setColumnHidden(static_cast<int>(WorkspaceInventoryModel::Column::FilePath), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->resizeSection(static_cast<int>(WorkspaceInventoryModel::Column::Icon), 32);

    _createAction.setToolTip("Create the project with the selected workspace");
    _cancelAction.setToolTip("Exit the dialog without creating a new project");

    connect(&_createAction, &TriggerAction::triggered, this, [this]() -> void {
        if (!_hierarchyWidget.getSelectionModel().hasSelection())
            return;

        const auto workspaceFilePath = _hierarchyWidget.getSelectionModel().selectedRows().first().siblingAtColumn(static_cast<int>(WorkspaceInventoryModel::Column::FilePath)).data(Qt::EditRole).toString();

        Application::core()->getProjectManager().newProject(workspaceFilePath);

        QDialog::accept();
    });

    connect(&_cancelAction, &TriggerAction::triggered, this, &QDialog::reject);
}

void WorkspaceDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem options = option;

    initStyleOption(&options, index);

    painter->save();

    QTextDocument textDocument;

    textDocument.setHtml(getHtml(index));

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    painter->translate(options.rect.left(), options.rect.top());
    
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    
    textDocument.drawContents(painter, clip);

    painter->restore();
    
    QStyledItemDelegate::paint(painter, option, index);
}

QSize WorkspaceDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem options = option;

    initStyleOption(&options, index);

    QTextDocument textDocument;

    textDocument.setHtml(getHtml(index));

    textDocument.setTextWidth(options.rect.width());
    
    return QSize(textDocument.idealWidth(), textDocument.size().height());
}

QString WorkspaceDelegate::getHtml(const QModelIndex& index) const
{
    auto name         = index.siblingAtColumn(static_cast<int>(WorkspaceInventoryModel::Column::Name)).data(Qt::DisplayRole).toString();
    auto description  = index.siblingAtColumn(static_cast<int>(WorkspaceInventoryModel::Column::Description)).data(Qt::DisplayRole).toString();
    auto tags         = index.siblingAtColumn(static_cast<int>(WorkspaceInventoryModel::Column::Tags)).data(Qt::DisplayRole).toStringList();

    if (description.isEmpty())
        description = "No description";

    QString tagsHtml;

    if (!tags.isEmpty()) {
        for (const auto& tag : tags)
            tagsHtml += QString("<span id='tag'>%1</span>  ").arg(tag);

        tagsHtml.insert(0, "<p id='tags'>");
        tagsHtml.append("</p>");
    }

    QString style = " \
        <style> \
            body { \
                padding: 25px; \
            } \
            span#tag { \
                background-color: silver; \
            } \
            p { \
                margin: 3px; \
            } \
            p#tags { \
                margin-top: 5px; \
            } \
        </style>";

    return QString("\
        <html> \
            <head> \
            %1 \
            </head> \
            <body> \
                <p> \
                    <b>%2</b> \
                </p> \
                <p>%3</p> \
                %4 \
            </body> \
        </html> \
    ").arg(style, name, description, tagsHtml);
}
