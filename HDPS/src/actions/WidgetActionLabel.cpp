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
#include <QTimer>

namespace hdps {

namespace gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, const std::uint32_t& flags /*= ColonAfterName*/) :
    QWidget(parent),
    _flags(flags),
    _widgetAction(widgetAction),
    _nameLabel(),
    _elide(false),
    _publishAction(this, "Publish..."),
    _disconnectAction(this, "Disconnect...")
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    connect(_widgetAction, &WidgetAction::isConnectedChanged, this, &WidgetActionLabel::updateNameLabel);
    connect(_widgetAction, &WidgetAction::isPublishedChanged, this, &WidgetActionLabel::updatePublishAction);

    _publishAction.setIcon(fontAwesome.getIcon("cloud-upload-alt"));
    _disconnectAction.setIcon(fontAwesome.getIcon("unlink"));

    connect(&_publishAction, &TriggerAction::triggered, this, &WidgetActionLabel::publishAction);
    connect(&_disconnectAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::disconnectFromPublicAction);

    auto layout = new QHBoxLayout();

    layout->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_nameLabel);

    setLayout(layout);

    _nameLabel.setText(getLabelText());
    _nameLabel.setAlignment(Qt::AlignRight);
    _nameLabel.setStyleSheet("color: black;");

    connect(widgetAction, &WidgetAction::changed, this, &WidgetActionLabel::updateNameLabel);

    updateNameLabel();

    updateNameLabel();
    updatePublishAction();

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
        case QEvent::Leave:
            updateNameLabel();
            break;

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void WidgetActionLabel::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    
    if (!_elide)
        return;

    _nameLabel.setText("");

    QTimer::singleShot(25, this, [this]() {
        elide();
     });
}

bool WidgetActionLabel::geElide() const
{
    return _elide;
}

void WidgetActionLabel::setElide(bool elide)
{
    if (elide == _elide)
        return;

    _elide = elide;
}

void WidgetActionLabel::elide()
{
    if (!_elide) {
        _nameLabel.setText(getLabelText());
    }
    else {
        QFontMetrics metrics(font());

        _nameLabel.setText(metrics.elidedText(getLabelText(), Qt::ElideMiddle, width()));
    }    
}

QString WidgetActionLabel::getLabelText() const
{
    return QString("%1%2 ").arg(_widgetAction->text(), (_flags & ColonAfterName) ? ":" : "");
}

void WidgetActionLabel::updatePublishAction()
{
    _publishAction.setEnabled(!_widgetAction->isPublished());
}

void WidgetActionLabel::updateNameLabel()
{
    auto font = _nameLabel.font();

    font.setUnderline(_widgetAction->isEnabled() && _widgetAction->mayPublish());
    font.setItalic(_widgetAction->mayPublish() && _widgetAction->isConnected());

    _nameLabel.setFont(font);
    _nameLabel.setEnabled(_widgetAction->isEnabled());
    _nameLabel.setToolTip(_widgetAction->toolTip());
    _nameLabel.setVisible(_widgetAction->isVisible());

    if (_widgetAction->isEnabled()) {
        if (_widgetAction->mayPublish() && _nameLabel.underMouse())
            _nameLabel.setStyleSheet("color: gray;");
        else
            _nameLabel.setStyleSheet("color: black;");
    } else {
        _nameLabel.setStyleSheet("color: gray;");
    }
}

void WidgetActionLabel::publishAction()
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    QDialog publishDialog(this);

    publishDialog.setWindowIcon(fontAwesome.getIcon("cloud-upload-alt"));
    publishDialog.setWindowTitle("Publish " + _widgetAction->text() + " parameter");

    auto mainLayout         = new QVBoxLayout();
    auto parameterLayout    = new QHBoxLayout();
    auto label              = new QLabel("Name:");
    auto lineEdit           = new QLineEdit(_widgetAction->text());

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

}
}
