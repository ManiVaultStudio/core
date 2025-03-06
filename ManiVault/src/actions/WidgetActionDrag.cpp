// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionDrag.h"
#include "WidgetActionMimeData.h"

#include <QDebug>
#include <QDrag>

using namespace mv::util;

namespace mv::gui {

WidgetActionDrag::WidgetActionDrag(WidgetAction* dragAction) :
    QObject(dragAction),
    _dragAction(dragAction),
    _isDragging(false)
{
    Q_ASSERT(_dragAction);
}

bool WidgetActionDrag::isDragging() const
{
    return _isDragging;
}

void WidgetActionDrag::start()
{
    Q_ASSERT(_dragAction);

    if (!_dragAction)
        return;

    setIsDragging(true);
    {
        auto drag       = new QDrag(this);
        auto mimeData   = new WidgetActionMimeData(_dragAction);

        drag->setMimeData(mimeData);
        drag->setPixmap(QIcon(StyledIcon("link")).pixmap(QSize(12, 12)));

        drag->exec();
    }
    QTimer::singleShot(100, [this]() { setIsDragging(false); });
}

void WidgetActionDrag::setIsDragging(bool dragging)
{
    if (dragging == _isDragging)
        return;

    _isDragging = dragging;

    emit isDraggingChanged(_isDragging);
}

}
