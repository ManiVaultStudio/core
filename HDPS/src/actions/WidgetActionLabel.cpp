#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTreeView>

namespace hdps {

namespace gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, Qt::WindowFlags windowFlags /*= Qt::WindowFlags()*/) :
    QWidget(parent, windowFlags),
    _widgetAction(widgetAction),
    _prefixLabel(),
    _nameLabel(),
    _publishAction(this, "Publish..."),
    _connectAction(this, "Connect..."),
    _disconnectAction(this, "Disconnect")
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _prefixLabel.setFont(fontAwesome.getFont(8));

    connect(_widgetAction, &WidgetAction::isPublishedChanged, this, &WidgetActionLabel::updatePrefixLabel);

    updatePrefixLabel();

    _publishAction.setIcon(fontAwesome.getIcon("cloud-upload-alt"));
    _connectAction.setIcon(fontAwesome.getIcon("link"));
    _disconnectAction.setIcon(fontAwesome.getIcon("unlink"));

    connect(&_publishAction, &TriggerAction::triggered, this, [this, &fontAwesome]() -> void {
        QDialog publishDialog(this);

        publishDialog.setWindowIcon(fontAwesome.getIcon("cloud-upload-alt"));
        publishDialog.setWindowTitle("Publish " + _widgetAction->text() + " parameter");

        auto mainLayout         = new QVBoxLayout();
        auto parameterLayout    = new QHBoxLayout();

        auto label      = new QLabel("Name:");
        auto lineEdit   = new QLineEdit();

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
    });

    connect(&_connectAction, &TriggerAction::triggered, this, [this, &fontAwesome]() -> void {
        QDialog connectDialog(this);

        connectDialog.setWindowIcon(fontAwesome.getIcon("link"));
        connectDialog.setWindowTitle("Connect " + _widgetAction->text() + " to public parameter");

        auto mainLayout = new QVBoxLayout();
        auto treeView   = new QTreeView();

        treeView->setModel(const_cast<ActionsManager::PublicActionsModel*>(&Application::getActionsManager().getPublicActionsModel()));
        treeView->setRootIsDecorated(false);
        treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
        treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        treeView->setSortingEnabled(false);

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

        if (connectDialog.exec() == QDialog::Accepted)
            _widgetAction->connectToPublicAction(static_cast<WidgetAction*>(treeView->selectionModel()->selectedRows().first().internalPointer()));
    });

    connect(&_disconnectAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::disconnectFromPublicAction);

    auto layout = new QHBoxLayout();

    layout->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    layout->setMargin(0);
    layout->addStretch(1);
    layout->addWidget(&_prefixLabel);
    layout->addWidget(&_nameLabel);

    setLayout(layout);

    _nameLabel.setAlignment(Qt::AlignRight);

    const auto update = [this, widgetAction]() -> void {
        _nameLabel.setEnabled(widgetAction->isEnabled());
        _nameLabel.setText(QString("%1: ").arg(widgetAction->text()));
        _nameLabel.setToolTip(widgetAction->text());
        _nameLabel.setVisible(widgetAction->isVisible());
    };

    connect(widgetAction, &WidgetAction::changed, this, update);

    update();

    _nameLabel.installEventFilter(this);
}

bool WidgetActionLabel::eventFilter(QObject* target, QEvent* event)
{
    auto contextMenu = new QMenu();

    if (_widgetAction->mayPublish()) {
        if (!_widgetAction->isPublic())
            contextMenu->addAction(&_publishAction);

        if (!contextMenu->actions().isEmpty())
            contextMenu->addSeparator();

        if (_widgetAction->isConnected())
            contextMenu->addAction(&_disconnectAction);
        else
            contextMenu->addAction(&_connectAction);
    }

    if (contextMenu->actions().isEmpty())
        return QWidget::eventFilter(target, event);

    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            auto mouseButtonPress = static_cast<QMouseEvent*>(event);

            if (mouseButtonPress->button() != Qt::LeftButton)
                break;

            contextMenu->exec(cursor().pos());

            break;
        }

        case QEvent::Enter:
        {
            setStyleSheet("QLabel { text-decoration: underline; }");

            break;
        }

        case QEvent::Leave:
        {
            setStyleSheet("QLabel { text-decoration: none; }");

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void WidgetActionLabel::updatePrefixLabel()
{
    if (_widgetAction->mayPublish()) {
        if (_widgetAction->isPublic())
            _prefixLabel.setText("<sup>" + Application::getIconFont("FontAwesome").getIconCharacter("link") + "</sup>");
        else
            _prefixLabel.setText("");
    }
}

}
}
