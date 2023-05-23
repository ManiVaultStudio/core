#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "WidgetActionMimeData.h"
#include "Application.h"

#include "models/ActionsFilterModel.h"
#include "models/ActionsListModel.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QTimer>
#include <QDrag>
#include <QMimeData>

namespace hdps::gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* action, QWidget* parent /*= nullptr*/, const std::uint32_t& flags /*= ColonAfterName*/) :
    WidgetActionViewWidget(parent, action),
    _flags(flags),
    _nameLabel(),
    _elide(false),
    _drag(nullptr),
    _lastMousePressPosition()
{
    setAction(action);
    setAcceptDrops(true);

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    connect(getAction(), &WidgetAction::isConnectedChanged, this, &WidgetActionLabel::updateNameLabel);
    connect(getAction(), &WidgetAction::connectionPermissionsChanged, this, &WidgetActionLabel::updateNameLabel);

    auto layout = new QHBoxLayout();

    layout->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_nameLabel);

    setLayout(layout);

    _nameLabel.setText(getLabelText());
    _nameLabel.setAlignment(Qt::AlignRight);
    _nameLabel.setStyleSheet("color: black;");

    connect(getAction(), &WidgetAction::changed, this, &WidgetActionLabel::updateNameLabel);

    updateNameLabel();

    installEventFilter(this);

    _nameLabel.installEventFilter(this);
}

bool WidgetActionLabel::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            if (dynamic_cast<QWidget*>(target) != &_nameLabel)
                break;

            auto mouseEvent = static_cast<QMouseEvent*>(event);

            switch (mouseEvent->button())
            {
                case Qt::LeftButton:
                {
                    if (isEnabled() && (getAction()->mayPublish(WidgetAction::Gui) || getAction()->mayConnect(WidgetAction::Gui) || getAction()->mayDisconnect(WidgetAction::Gui))) {
                        auto contextMenu = getAction()->getContextMenu(this);

                        if (contextMenu->actions().isEmpty())
                            return QWidget::eventFilter(target, event);

                        contextMenu->exec(cursor().pos());
                    }

                    break;
                }

                case Qt::RightButton:
                {
                    if (isEnabled() && getAction()->mayConnect(WidgetAction::Gui)) {
                        auto drag = new QDrag(this);

                        auto mimeData = new WidgetActionMimeData(getAction());

                        drag->setMimeData(mimeData);
                        drag->setPixmap(Application::getIconFont("FontAwesome").getIcon("link").pixmap(QSize(12, 12)));

                        ActionsListModel actionsListModel(this);
                        ActionsFilterModel actionsFilterModel(this);

                        actionsFilterModel.setSourceModel(&actionsListModel);
                        actionsFilterModel.getScopeFilterAction().setSelectedOptions({ "Private" });
                        actionsFilterModel.getTypeFilterAction().setString(getAction()->getTypeString());

                        const auto numberOfRows = actionsFilterModel.rowCount();

                        for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
                            auto action = actionsFilterModel.getAction(rowIndex);

                            if (action != getAction())
                                actionsFilterModel.getAction(rowIndex)->setHighlighted(true);
                        }

                        Qt::DropAction dropAction = drag->exec();

                        for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
                            auto action = actionsFilterModel.getAction(rowIndex);

                            if (action != getAction())
                                actionsFilterModel.getAction(rowIndex)->setHighlighted(false);
                        }
                    }

                    break;
                }
            }

            break;
        }

        case QEvent::DragEnter:
        {
            if (dynamic_cast<QWidget*>(target) != this)
                break;

            auto dragEnterEvent = static_cast<QDragEnterEvent*>(event);
            auto actionMimeData = dynamic_cast<const WidgetActionMimeData*>(dragEnterEvent->mimeData());

            if (actionMimeData)
                if ((actionMimeData->getAction() != getAction()) && (actionMimeData->getAction()->getTypeString() == getAction()->getTypeString()))
                    dragEnterEvent->acceptProposedAction();

            break;
        }

        case QEvent::Drop:
        {
            if (dynamic_cast<QWidget*>(target) != this)
                break;

            auto dropEvent      = static_cast<QDropEvent*>(event);
            auto actionMimeData = dynamic_cast<const WidgetActionMimeData*>(dropEvent->mimeData());

            if (actionMimeData)
                if ((actionMimeData->getAction() != getAction()) && (actionMimeData->getAction()->getTypeString() == getAction()->getTypeString()))
                    hdps::actions().connectPrivateActions(actionMimeData->getAction(), getAction());

            break;
        }

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
    return QString("%1%2").arg(const_cast<WidgetActionLabel*>(this)->getAction()->text(), (_flags & ColonAfterName) ? ":" : "");
}

void WidgetActionLabel::updateNameLabel()
{
    const auto connectionEnabled = getAction()->mayPublish(WidgetAction::Gui) || getAction()->mayConnect(WidgetAction::Gui) || getAction()->mayDisconnect(WidgetAction::Gui);

    auto font = _nameLabel.font();

    font.setUnderline(getAction()->isEnabled() && connectionEnabled);
    font.setItalic(getAction()->mayPublish(WidgetAction::Gui) && getAction()->isConnected());

    _nameLabel.setFont(font);
    _nameLabel.setEnabled(getAction()->isEnabled());
    _nameLabel.setToolTip(getAction()->toolTip());
    _nameLabel.setVisible(getAction()->isVisible());

    if (getAction()->isEnabled() && isEnabled()) {
        if (getAction()->mayPublish(WidgetAction::Gui) && _nameLabel.underMouse())
            _nameLabel.setStyleSheet("color: gray;");
        else
            _nameLabel.setStyleSheet("color: black;");
    } else {
        _nameLabel.setStyleSheet("color: gray;");
    }
}

}
