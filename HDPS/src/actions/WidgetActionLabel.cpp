#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "Application.h"

#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

namespace hdps {

namespace gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, Qt::WindowFlags windowFlags /*= Qt::WindowFlags()*/) :
    QLabel(widgetAction->text(), parent, windowFlags),
    _widgetAction(widgetAction)
{
    setAcceptDrops(true);

    connect(widgetAction, &WidgetAction::changed, this, [this, widgetAction]() {
        setEnabled(widgetAction->isEnabled());
        setText(widgetAction->text());
        setToolTip(widgetAction->toolTip());
    });
}

void WidgetActionLabel::mousePressEvent(QMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        _dragStartPosition = mouseEvent->pos();
    }
}

void WidgetActionLabel::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    if (!(mouseEvent->buttons() & Qt::LeftButton))
        return;

    if ((mouseEvent->pos() - _dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    auto drag = new QDrag(this);
    auto mimeData = new QMimeData;

    mimeData->setText(typeid(*_widgetAction).name());
    drag->setMimeData(mimeData);
    drag->setPixmap(Application::getIconFont("FontAwesome").getIcon("link").pixmap(16, 16));

    Qt::DropAction dropAction = drag->exec();
}

void WidgetActionLabel::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
    auto mimeData = dragEnterEvent->mimeData();

    if (!mimeData->hasFormat("text/plain"))
        return;

    qDebug() << mimeData->text() << typeid(*_widgetAction).name();

    if (mimeData->text() != typeid(*_widgetAction).name())
        return;

    dragEnterEvent->acceptProposedAction();

    setStyleSheet("QLabel { text-decoration: underline; }");
}

void WidgetActionLabel::dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent)
{
    setStyleSheet("QLabel { text-decoration: none; }");
}

void WidgetActionLabel::dropEvent(QDropEvent* dropEvent)
{
    qDebug() << dropEvent->mimeData()->text();

    dropEvent->acceptProposedAction();

    setStyleSheet("QLabel { text-decoration: none; }");
}

}
}
