// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "WidgetActionContextMenu.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QTimer>
#include <QMimeData>

namespace mv::gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* action, QWidget* parent /*= nullptr*/, const std::uint32_t& flags /*= ColonAfterName*/) :
    WidgetActionViewWidget(parent, action),
    _flags(flags),
    _elide(false),
    _drag(nullptr)
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

    _nameLabel.setObjectName("Label");
    _nameLabel.setText(getLabelText());
    _nameLabel.setAlignment(Qt::AlignRight);

    connect(getAction(), &WidgetAction::changed, this, &WidgetActionLabel::updateNameLabel);

    updateNameLabel();

    _nameLabel.installEventFilter(this);

    updateCustomStyle();
}

bool WidgetActionLabel::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            if (dynamic_cast<QWidget*>(target) != &_nameLabel)
                break;

            auto mouseEvent = dynamic_cast<QMouseEvent*>(event);

            switch (mouseEvent->button())
            {
                case Qt::LeftButton:
                {
                    if (!isEnabled())
                        break;

                    if (!getAction()->mayConnect(WidgetAction::Gui))
                        break;

					getAction()->getDrag().start();

                    break;
                }

                case Qt::RightButton:
                {
                    if (!isEnabled())
                        break;

                    if (getAction()->isConnectionPermissionFlagSet(WidgetAction::ConnectionPermissionFlag::ForceNone))
                        break;

                    if (!getAction()->mayPublish(WidgetAction::Gui) && !getAction()->mayConnect(WidgetAction::Gui) && !getAction()->mayDisconnect(WidgetAction::Gui))
                        break;

                    auto contextMenu = new WidgetActionContextMenu(this, { getAction() });

                    if (!contextMenu)
                        contextMenu = new WidgetActionContextMenu(&_nameLabel, { getAction() });

                    if (contextMenu->actions().isEmpty())
                        return QWidget::eventFilter(target, event);

                    contextMenu->exec(QCursor::pos());

                    break;
                }
            }

            break;
        }

        case QEvent::Enter:
        {
            if (dynamic_cast<QWidget*>(target) != &_nameLabel)
                break;

            if (isEnabled() && (getAction()->mayPublish(WidgetAction::Gui) || getAction()->mayConnect(WidgetAction::Gui) || getAction()->mayDisconnect(WidgetAction::Gui)))
                // changed highlight to link as it is more readable in dark theme
                _nameLabel.setStyleSheet(QString("QLabel { color: %1; }").arg(palette().link().color().name()));
            
            break;
        }

        case QEvent::Leave:
        {
            if (dynamic_cast<QWidget*>(target) != &_nameLabel)
                break;

            if (isEnabled() && (getAction()->mayPublish(WidgetAction::Gui) || getAction()->mayConnect(WidgetAction::Gui) || getAction()->mayDisconnect(WidgetAction::Gui)))
                _nameLabel.setStyleSheet(QString("QLabel { color: %1; }").arg(palette().text().color().name()));

            break;
        }

        case QEvent::EnabledChange:
        {
            updateNameLabel();
            break;
        }

        case QEvent::ApplicationPaletteChange:
        {
            updateCustomStyle();
            break;
        }

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
    font.setItalic(getAction()->isConnected());

    _nameLabel.setFont(font);
    _nameLabel.setEnabled(getAction()->isEnabled());
    _nameLabel.setToolTip(getAction()->toolTip());
    _nameLabel.setVisible(getAction()->isVisible());
    
    updateCustomStyle();
}

void WidgetActionLabel::updateCustomStyle()
{
    // update custome style settings
    if (getAction()->isEnabled() && isEnabled()) {
        if (getAction()->mayPublish(WidgetAction::Gui) && _nameLabel.underMouse())
            _nameLabel.setStyleSheet(QString("QLabel { color: %1; }").arg(qApp->palette().link().color().name()));
        else
            _nameLabel.setStyleSheet(QString("QLabel { color: %1; }").arg(qApp->palette().text().color().name()));
    } else {
        _nameLabel.setStyleSheet("color: gray;");
    }
}

}
