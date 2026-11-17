// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "WidgetActionContextMenu.h"
#include "Application.h"
#include "CoreInterface.h"

#include "widgets/IconLabel.h"
#include "util/StyledIcon.h"

#include <QDebug>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QTimer>
#include <QMimeData>

namespace mv::gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* action, QWidget* parent /*= nullptr*/, const std::uint32_t& flags /*= ColonAfterName | ShowDescription*/) :
    WidgetActionViewWidget(parent, action),
    _flags(flags),
    _descriptionIconLabel(nullptr),
    _elide(false),
    _drag(nullptr)
{
    setAction(action);
    setAcceptDrops(true);

    connect(getAction(), &WidgetAction::isConnectedChanged, this, &WidgetActionLabel::updateNameLabel);
    connect(getAction(), &WidgetAction::connectionPermissionsChanged, this, &WidgetActionLabel::updateNameLabel);
    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, &WidgetActionLabel::updateCustomStyle);

    auto layout = new QHBoxLayout();

    layout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->setSpacing(4);

    layout->setContentsMargins(0, 0, 0, 0);
    _descriptionIconLabel = new IconLabel(util::StyledIcon("circle-info"), this);
    _descriptionIconLabel->setIconSize(QSize(14, 14));
    _descriptionIconLabel->setFixedSize(14, 14);
    _descriptionIconLabel->setAlignment(Qt::AlignCenter);
    _descriptionIconLabel->setIdleOpacity(0.85);
    _descriptionIconLabel->setText(QStringLiteral("•"));
    _descriptionIconLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(palette().color(QPalette::Text).name()));
    _descriptionIconLabel->setObjectName("DescriptionIcon");
    _descriptionIconLabel->setAccessibleName(QString("Description for %1").arg(action->text()));
    _descriptionIconLabel->setTooltipCallback([this]() {
        const auto actionName = getAction()->text().toHtmlEscaped();
        const auto headingColor = palette().color(QPalette::Link).name();
        return QString(
            "<qt><table width=\"440\" cellpadding=\"12\" cellspacing=\"0\" border=\"0\"><tr><td>"
            "<div style=\"font-size: 11pt; font-weight: bold; color: %1; margin-bottom: 10px;\">%2</div>"
            "<div style=\"font-size: 9pt;\">%3</div>"
            "</td></tr></table></qt>").arg(headingColor, actionName, getAction()->getDescription());
    });
    if (_flags & ShowDescription)
        layout->addWidget(_descriptionIconLabel);
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
    bool isInStudioMode = false;
    
    if (auto project = mv::projects().getCurrentProject())
        isInStudioMode = project->getStudioModeAction().isChecked();
    
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            if (dynamic_cast<QWidget*>(target) != &_nameLabel)
                break;
            
            if (auto project = mv::projects().getCurrentProject())
                if (!project->getStudioModeAction().isChecked())
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

                default:
                    break;
            }

            break;
        }

        case QEvent::Enter:
        {
            if (!isInStudioMode)
                break;
            
            if (dynamic_cast<QWidget*>(target) != &_nameLabel)
                break;

            if (isEnabled() && (getAction()->mayPublish(WidgetAction::Gui) || getAction()->mayConnect(WidgetAction::Gui) || getAction()->mayDisconnect(WidgetAction::Gui)))
                // changed highlight to link as it is more readable in dark theme
                _nameLabel.setStyleSheet(QString("QLabel { color: %1; }").arg(palette().link().color().name()));
            
            break;
        }

        case QEvent::Leave:
        {
            if (!isInStudioMode)
                break;
            
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

bool WidgetActionLabel::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange) {
        updateCustomStyle();
        updateDescriptionIndicator();
    }
    else if (event->type() == QEvent::Enter) {
        _descriptionAreaHovered = true;
        updateDescriptionIndicator();
    }
    else if (event->type() == QEvent::Leave) {
        _descriptionAreaHovered = false;
        updateDescriptionIndicator();
    }

    return WidgetActionViewWidget::event(event);
}

void WidgetActionLabel::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    updateDescriptionIndicator();
    
    if (!_elide)
        return;

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
    updateDescriptionIndicator();
}

void WidgetActionLabel::elide()
{
    if (!_elide) {
        _nameLabel.setText(getLabelText());
    }
    else {
        QFontMetrics metrics(font());
        const auto elidedLabelText = metrics.elidedText(getLabelText(), Qt::ElideMiddle, _nameLabel.width());

        if (_nameLabel.text() != elidedLabelText)
            _nameLabel.setText(elidedLabelText);
    }    

    updateDescriptionIndicator();
}

QString WidgetActionLabel::getLabelText() const
{
    return QString("%1%2").arg(const_cast<WidgetActionLabel*>(this)->getAction()->text(), (_flags & ColonAfterName) ? ":" : "");
}

void WidgetActionLabel::updateNameLabel()
{
    const auto connectionEnabled = getAction()->mayPublish(WidgetAction::Gui) || getAction()->mayConnect(WidgetAction::Gui) || getAction()->mayDisconnect(WidgetAction::Gui);
    
    bool isInStudioMode = false;
    
    if (auto project = mv::projects().getCurrentProject())
        isInStudioMode = project->getStudioModeAction().isChecked();
    
    auto font = _nameLabel.font();
    
    font.setUnderline(getAction()->isEnabled() && connectionEnabled && isInStudioMode);
    font.setItalic(getAction()->isConnected());

    _nameLabel.setFont(font);
    _nameLabel.setEnabled(getAction()->isEnabled());
    _nameLabel.setToolTip(getAction()->toolTip());
    _nameLabel.setVisible(getAction()->isVisible());
    updateDescriptionIndicator();
    _descriptionIconLabel->setEnabled(getAction()->isEnabled());
    
    updateCustomStyle();
}

void WidgetActionLabel::updateDescriptionIndicator()
{
    if (!_descriptionIconLabel)
        return;

    const auto hasDescription = getAction()->isVisible() && (_flags & ShowDescription) && !getAction()->getDescription().isEmpty();
    const auto actionEnabled = getAction()->isEnabled();

    _descriptionIconLabel->setVisible(hasDescription);

    if (!hasDescription)
        return;

    if (_descriptionAreaHovered) {
        _descriptionIconLabel->setIdleOpacity(actionEnabled ? 1.0 : 0.45);
        _descriptionIconLabel->setStyleSheet(QString());
        _descriptionIconLabel->setIcon(util::StyledIcon("circle-info"));
    } else {
        _descriptionIconLabel->setIdleOpacity(actionEnabled ? 0.85 : 0.45);
        const auto textColor = actionEnabled ? palette().color(QPalette::Text) : palette().color(QPalette::Disabled, QPalette::Text);
        _descriptionIconLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(textColor.name()));
        _descriptionIconLabel->setText(QStringLiteral("•"));
    }
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
        _nameLabel.setStyleSheet(QString("QLabel { color: %1; }").arg(qApp->palette().color(QPalette::Disabled, QPalette::Text).name()));
    }
}

}
