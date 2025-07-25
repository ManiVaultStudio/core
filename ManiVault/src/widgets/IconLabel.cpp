// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IconLabel.h"

#include <QDebug>
#include <QToolTip>

#ifdef _DEBUG
    #define ICON_LABEL_VERBOSE
#endif

namespace mv::gui
{

IconLabel::IconLabel(const QIcon& icon, QWidget* parent /*= nullptr*/) :
    QLabel(parent),
    _opacityEffect(this)
{
    setGraphicsEffect(&_opacityEffect);
    updateOpacityEffect();
    setIcon(icon);
}

void IconLabel::setIcon(const QIcon& icon)
{
    setPixmap(icon.pixmap(QSize(12, 12)));
}

void IconLabel::setTooltipCallback(const TooltipCallback& tooltipCallback)
{
    _tooltipCallback = tooltipCallback;
}

void IconLabel::enterEvent(QEnterEvent* enterEvent)
{
    QLabel::enterEvent(enterEvent);

    updateOpacityEffect();

    if (toolTip().isEmpty() && _tooltipCallback)
        setToolTip(_tooltipCallback());
}

void IconLabel::leaveEvent(QEvent* event)
{
    QLabel::leaveEvent(event);

    updateOpacityEffect();
}

bool IconLabel::event(QEvent* event)
{
	if (event->type() == QEvent::ToolTip) {
		if (auto helpEvent = dynamic_cast<QHelpEvent*>(event)) {
			QToolTip::showText(helpEvent->globalPos(), _tooltipCallback ? _tooltipCallback() : "", this);

			return true;
		}
	}

    return QLabel::event(event);
}

void IconLabel::updateOpacityEffect()
{
    _opacityEffect.setOpacity(QWidget::underMouse() ? 0.4 : 1.0);
}

}
