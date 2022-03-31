#include "ActionsViewerWidget.h"

#include <Application.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QGraphicsOpacityEffect>
#include <QHeaderView>

#include <stdexcept>

namespace hdps
{

namespace gui
{

ActionsViewerWidget::ActionsViewerWidget(QWidget* parent) :
    QWidget(parent),
    _treeView(this),
    _overlayWidget(this),
    _filterModel(this),
    _nameFilterAction(this, "Name filter"),
    _typeFilterAction(this, "Type filter")
{
    _filterModel.setSourceModel(const_cast<ActionsModel*>(&Application::getActionsManager().getActionsModel()));

    connect(&_filterModel, &QAbstractItemModel::rowsInserted, this, &ActionsViewerWidget::numberOfRowsChanged);
    connect(&_filterModel, &QAbstractItemModel::rowsRemoved, this, &ActionsViewerWidget::numberOfRowsChanged);

    _treeView.setModel(&_filterModel);

    _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView.setDragEnabled(true);
    _treeView.setDragDropMode(QAbstractItemView::DragOnly);
    _treeView.setSelectionMode(QAbstractItemView::SingleSelection);
    _treeView.setRootIsDecorated(false);
    _treeView.setItemsExpandable(true);
    _treeView.setSortingEnabled(true);

    _treeView.header()->setStretchLastSection(false);
    _treeView.header()->setMinimumSectionSize(18);
    _treeView.header()->hideSection(ActionsModel::Column::IsPublic);

    _treeView.header()->resizeSection(ActionsModel::Column::Name, 200);
    _treeView.header()->resizeSection(ActionsModel::Column::Type, 80);
    _treeView.header()->resizeSection(ActionsModel::Column::IsPublic, 50);
    _treeView.header()->resizeSection(ActionsModel::Column::NumberOfConnections, 50);

    _treeView.header()->setSectionResizeMode(ActionsModel::Column::Name, QHeaderView::Interactive);
    _treeView.header()->setSectionResizeMode(ActionsModel::Column::Type, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(ActionsModel::Column::IsPublic, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(ActionsModel::Column::NumberOfConnections, QHeaderView::Stretch);

    _nameFilterAction.setPlaceHolderString("Filter by name...");
    _nameFilterAction.setSearchMode(true);

    connect(&_nameFilterAction, &StringAction::stringChanged, &_filterModel, &ActionsFilterModel::setNameFilter);

    auto layout = new QVBoxLayout();

    layout->setMargin(6);
    layout->setSpacing(3);

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->setContentsMargins(0, 3, 0, 3);
    toolbarLayout->setSpacing(4);
    toolbarLayout->addWidget(_nameFilterAction.createWidget(this), 1);
    toolbarLayout->addWidget(_typeFilterAction.createCollapsedWidget(this));

    layout->addLayout(toolbarLayout);
    layout->addWidget(&_treeView);

    setLayout(layout);

    numberOfRowsChanged();
}

void ActionsViewerWidget::numberOfRowsChanged()
{
    const auto hasParameters            = Application::getActionsManager().getActionsModel().rowCount() >= 1;
    const auto hasFilteredParameters    = _filterModel.rowCount() >= 1;

    _treeView.setHeaderHidden(!hasFilteredParameters);
    _nameFilterAction.setEnabled(hasParameters);
    
    _overlayWidget.setVisible(!hasParameters || !hasFilteredParameters);

    if (!hasParameters) {
        _overlayWidget.setIconCharacter("link");
        _overlayWidget.setTitle("No shared parameters...");
        _overlayWidget.setDescription("Click on a parameter label to share it");
    }

    if (hasParameters && !hasFilteredParameters) {
        _overlayWidget.setIconCharacter("search");
        _overlayWidget.setTitle("No shared parameters found");
        _overlayWidget.setDescription("");
    }
}

ActionsViewerWidget::OverlayWidget::OverlayWidget(QWidget* parent) :
    QWidget(parent),
    _opacityEffect(this),
    _iconLabel(),
    _titleLabel(),
    _descriptionLabel()
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setGraphicsEffect(&_opacityEffect);

    _opacityEffect.setOpacity(0.35);

    auto layout = new QVBoxLayout();

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _iconLabel.setAlignment(Qt::AlignCenter);
    _iconLabel.setFont(fontAwesome.getFont(14));
    _iconLabel.setStyleSheet("QLabel { padding: 10px; }");

    _titleLabel.setAlignment(Qt::AlignCenter);
    _titleLabel.setStyleSheet("font-weight: bold");

    _descriptionLabel.setAlignment(Qt::AlignCenter);

    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    layout->addStretch(1);
    layout->addWidget(&_iconLabel);
    layout->addWidget(&_titleLabel);
    layout->addWidget(&_descriptionLabel);
    layout->addStretch(1);

    setLayout(layout);

    parent->installEventFilter(this);

    setObjectName("NoSharedParametersWidget");
    setStyleSheet("QWidget#NoSharedParametersWidget > QLabel { color: gray; }");
}

bool ActionsViewerWidget::OverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            setFixedSize(static_cast<QResizeEvent*>(event)->size());

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void ActionsViewerWidget::OverlayWidget::setIconCharacter(const QString& iconCharacter)
{
    _iconLabel.setText(Application::getIconFont("FontAwesome").getIconCharacter(iconCharacter));
}

void ActionsViewerWidget::OverlayWidget::setTitle(const QString& title)
{
    _titleLabel.setText(title);
}

void ActionsViewerWidget::OverlayWidget::setDescription(const QString& description)
{
    _descriptionLabel.setText(description);
}

}
}
