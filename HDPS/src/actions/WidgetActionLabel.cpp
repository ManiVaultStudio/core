#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "models/ActionsFilterModel.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QTimer>

namespace hdps {

namespace gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, const std::uint32_t& flags /*= ColonAfterName*/) :
    QWidget(parent),
    _flags(flags),
    _widgetAction(widgetAction),
    _nameLabel(),
    _elide(false)
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    connect(_widgetAction, &WidgetAction::isConnectedChanged, this, &WidgetActionLabel::updateNameLabel);
    connect(_widgetAction, &WidgetAction::connectionPermissionsChanged, this, &WidgetActionLabel::updateNameLabel);

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

    _nameLabel.installEventFilter(this);
}

bool WidgetActionLabel::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            if (!_widgetAction->isEnabled())
                break;

            auto mouseButtonPress = static_cast<QMouseEvent*>(event);

            if (mouseButtonPress->button() != Qt::LeftButton)
                break;

            auto contextMenu = _widgetAction->getContextMenu(this);

            if (contextMenu->actions().isEmpty())
                return QWidget::eventFilter(target, event);

            contextMenu->exec(cursor().pos());

            break;
        }

        case QEvent::Enter:
        case QEvent::Leave:
        case QEvent::EnabledChange:
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
    return QString("%1%2").arg(_widgetAction->text(), (_flags & ColonAfterName) ? ":" : "");
}

void WidgetActionLabel::updateNameLabel()
{
    const auto connectionEnabled = _widgetAction->mayPublish(WidgetAction::Gui) || _widgetAction->mayConnect(WidgetAction::Gui) || _widgetAction->mayDisconnect(WidgetAction::Gui);

    auto font = _nameLabel.font();

    font.setUnderline(_widgetAction->isEnabled() && connectionEnabled);
    font.setItalic(_widgetAction->mayPublish(WidgetAction::Gui) && _widgetAction->isConnected());

    _nameLabel.setFont(font);
    _nameLabel.setEnabled(_widgetAction->isEnabled());
    _nameLabel.setToolTip(_widgetAction->toolTip());
    _nameLabel.setVisible(_widgetAction->isVisible());

    if (_widgetAction->isEnabled() && isEnabled()) {
        if (_widgetAction->mayPublish(WidgetAction::Gui) && _nameLabel.underMouse())
            _nameLabel.setStyleSheet("color: gray;");
        else
            _nameLabel.setStyleSheet("color: black;");
    } else {
        _nameLabel.setStyleSheet("color: gray;");
    }
}

}
}
