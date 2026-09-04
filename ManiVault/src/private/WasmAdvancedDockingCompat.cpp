// SPDX-License-Identifier: LGPL-3.0-or-later

#include <FloatingDockContainer.h>

#include <QMoveEvent>
#include <QResizeEvent>

#ifdef Q_OS_WASM

namespace ads {

bool CFloatingDockContainer::event(QEvent* event)
{
    return QWidget::event(event);
}

void CFloatingDockContainer::moveEvent(QMoveEvent* event)
{
    QWidget::moveEvent(event);
}

void CFloatingDockContainer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

} // namespace ads

#endif
