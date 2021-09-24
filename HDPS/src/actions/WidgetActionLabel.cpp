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
    QLabel("", parent, windowFlags),
    _widgetAction(widgetAction)
{
    setAcceptDrops(true);

    const auto update = [this, widgetAction]() -> void {
        setEnabled(widgetAction->isEnabled());
        setText(QString("%1: ").arg(widgetAction->text()));
        setToolTip(widgetAction->text());
        setVisible(widgetAction->isVisible());
    };

    connect(widgetAction, &WidgetAction::changed, this, update);

    update();

    connect(widgetAction, &WidgetAction::isDropTargetChanged, this, [this, widgetAction](const bool& isDropTarget) {
        setStyleSheet(QString("QLabel { text-decoration: %1; }").arg(isDropTarget ? "underline" : "none"));
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

    //Application::current()->getWidgetActionsManager().widgetStartedDragging(_widgetAction);

    Qt::DropAction dropAction = drag->exec();

    //Application::current()->getWidgetActionsManager().widgetStoppedDragging(_widgetAction);
}

void WidgetActionLabel::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
    auto mimeData = dragEnterEvent->mimeData();

    if (!mimeData->hasFormat("text/plain"))
        return;

    //qDebug() << mimeData->text() << typeid(*_widgetAction).name();

    if (mimeData->text() != typeid(*_widgetAction).name())
        return;

    dragEnterEvent->acceptProposedAction();
}

void WidgetActionLabel::dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent)
{
}

void WidgetActionLabel::dropEvent(QDropEvent* dropEvent)
{
    //qDebug() << dropEvent->mimeData()->text();

    dropEvent->acceptProposedAction();

    //Application::current()->getWidgetActionsManager().widgetStoppedDragging(_widgetAction);
}

}
}
