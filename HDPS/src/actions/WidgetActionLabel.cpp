#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "ActionsFilterModel.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTreeView>
#include <QHeaderView>

namespace hdps {

namespace gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, const std::uint32_t& flags /*= 0*/) :
    QWidget(parent),
    _flags(flags),
    _widgetAction(widgetAction),
    _nameLabel(),
    _publishAction(this, "Publish..."),
    _disconnectAction(this, "Disconnect...")
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    connect(_widgetAction, &WidgetAction::isConnectedChanged, this, &WidgetActionLabel::updateLabel);
    connect(_widgetAction, &WidgetAction::isPublishedChanged, this, &WidgetActionLabel::updatePublishAction);

    _publishAction.setIcon(fontAwesome.getIcon("cloud-upload-alt"));
    _disconnectAction.setIcon(fontAwesome.getIcon("unlink"));

    connect(&_publishAction, &TriggerAction::triggered, this, &WidgetActionLabel::publishAction);
    connect(&_disconnectAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::disconnectFromPublicAction);

    auto layout = new QHBoxLayout();

    layout->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    layout->setMargin(0);
    layout->addStretch(1);
    layout->addWidget(&_nameLabel);

    setLayout(layout);

    _nameLabel.setAlignment(Qt::AlignRight);

    const auto update = [this, widgetAction]() -> void {
        _nameLabel.setEnabled(widgetAction->isEnabled());
        _nameLabel.setText(QString("%1%2 ").arg(widgetAction->text(), (_flags & ColonAfterName) ? ":" : ""));
        _nameLabel.setToolTip(widgetAction->text());
        _nameLabel.setVisible(widgetAction->isVisible());
    };

    connect(widgetAction, &WidgetAction::changed, this, update);

    update();

    updateLabel();
    updatePublishAction();

    // Intercept name label events so that we can highlight and provide a context menu to the label
    _nameLabel.installEventFilter(this);
}

bool WidgetActionLabel::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            auto mouseButtonPress = static_cast<QMouseEvent*>(event);

            if (mouseButtonPress->button() != Qt::LeftButton)
                break;

            auto contextMenu = QSharedPointer<QMenu>::create();

            if (_widgetAction->isEnabled() && _widgetAction->mayPublish()) {

                if (!_widgetAction->isPublic())
                    contextMenu->addAction(&_publishAction);

                if (!contextMenu->actions().isEmpty())
                    contextMenu->addSeparator();

                if (_widgetAction->isConnected())
                    contextMenu->addAction(&_disconnectAction);
                else {
                    auto connectMenu = new QMenu("Connect to:");

                    connectMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("link"));

                    auto actionsFilterModel = new ActionsFilterModel(this);

                    actionsFilterModel->setSourceModel(const_cast<ActionsModel*>(&Application::getActionsManager().getActionsModel()));
                    actionsFilterModel->setScopeFilter(ActionsFilterModel::Public);
                    actionsFilterModel->setTypeFilter(_widgetAction->getTypeString());

                    const auto numberOfRows = actionsFilterModel->rowCount();

                    for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
                        const auto publicAction = static_cast<WidgetAction*>(actionsFilterModel->mapToSource(actionsFilterModel->index(rowIndex, 0)).internalPointer());

                        auto connectAction = new QAction(publicAction->text());

                        connectAction->setIcon(connectMenu->icon());
                        connectAction->setToolTip("Connect " + _widgetAction->text() + " to " + publicAction->text());

                        connect(connectAction, &QAction::triggered, this, [this, publicAction]() -> void {
                            _widgetAction->connectToPublicAction(publicAction);
                        });

                        connectMenu->addAction(connectAction);
                    }

                    connectMenu->setEnabled(!connectMenu->actions().isEmpty());

                    contextMenu->addMenu(connectMenu);
                }
            }

            if (contextMenu->actions().isEmpty())
                return QWidget::eventFilter(target, event);

            contextMenu->exec(cursor().pos());

            break;
        }

        case QEvent::Enter:
        {
            if (_widgetAction->isEnabled() && _widgetAction->mayPublish())
                setStyleSheet("QLabel { text-decoration: underline; }");

            break;
        }

        case QEvent::Leave:
        {
            if (_widgetAction->isEnabled() && _widgetAction->mayPublish())
                setStyleSheet("QLabel { text-decoration: none; }");

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void WidgetActionLabel::updatePublishAction()
{
    _publishAction.setEnabled(!_widgetAction->isPublished());
}

void WidgetActionLabel::updateLabel()
{
    if (_widgetAction->mayPublish()) {
        auto font = _nameLabel.font();
        font.setItalic(_widgetAction->isConnected());
        _nameLabel.setFont(font);
    }
}

void WidgetActionLabel::publishAction()
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    QDialog publishDialog(this);

    publishDialog.setWindowIcon(fontAwesome.getIcon("cloud-upload-alt"));
    publishDialog.setWindowTitle("Publish " + _widgetAction->text() + " parameter");

    auto mainLayout = new QVBoxLayout();
    auto parameterLayout = new QHBoxLayout();

    auto label = new QLabel("Name:");
    auto lineEdit = new QLineEdit(_widgetAction->text());

    parameterLayout->addWidget(label);
    parameterLayout->addWidget(lineEdit);

    mainLayout->addLayout(parameterLayout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Publish");
    dialogButtonBox->button(QDialogButtonBox::Ok)->setToolTip("Publish the parameter");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setToolTip("Cancel publishing");

    connect(dialogButtonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, &publishDialog, &QDialog::accept);
    connect(dialogButtonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, &publishDialog, &QDialog::reject);

    mainLayout->addWidget(dialogButtonBox);

    publishDialog.setLayout(mainLayout);
    publishDialog.setFixedWidth(300);

    const auto updateOkButtonReadOnly = [dialogButtonBox, lineEdit]() -> void {
        dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!lineEdit->text().isEmpty());
    };

    connect(lineEdit, &QLineEdit::textChanged, this, updateOkButtonReadOnly);

    updateOkButtonReadOnly();

    if (publishDialog.exec() == QDialog::Accepted)
        _widgetAction->publish(lineEdit->text());
}

void WidgetActionLabel::connectAction()
{
    /*
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    QDialog connectDialog(this);

    connectDialog.setWindowIcon(fontAwesome.getIcon("link"));
    connectDialog.setWindowTitle("Connect " + _widgetAction->text() + " to public parameter");

    auto mainLayout = new QVBoxLayout();
    auto treeView = new QTreeView();
    auto actionsFilterModel = new ActionsFilterModel(this);

    actionsFilterModel->setSourceModel(const_cast<ActionsModel*>(&Application::getActionsManager().getActionsModel()));
    actionsFilterModel->setTypeFilter(_widgetAction->getTypeString());

    treeView->setModel(actionsFilterModel);
    treeView->setRootIsDecorated(false);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView->setSortingEnabled(false);

    treeView->header()->hideSection(ActionsModel::Column::Type);
    treeView->header()->hideSection(ActionsModel::Column::IsPublic);

    mainLayout->addWidget(treeView);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Connect");
    dialogButtonBox->button(QDialogButtonBox::Ok)->setToolTip("Connect to the selected parameter");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setToolTip("Cancel connection");

    connect(dialogButtonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, &connectDialog, &QDialog::accept);
    connect(dialogButtonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, &connectDialog, &QDialog::reject);

    mainLayout->addWidget(dialogButtonBox);

    connectDialog.setLayout(mainLayout);
    connectDialog.setMinimumSize(400, 400);

    const auto updateOkButtonReadOnly = [dialogButtonBox, treeView]() -> void {
        dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(treeView->selectionModel()->selectedRows().count() == 1);
    };

    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, updateOkButtonReadOnly);

    updateOkButtonReadOnly();

    if (connectDialog.exec() == QDialog::Accepted) {
        const auto selectedAction = actionsFilterModel->mapToSource(treeView->selectionModel()->selectedRows().first());
        _widgetAction->connectToPublicAction(static_cast<WidgetAction*>(selectedAction.internalPointer()));
    }
    */
}

}
}
