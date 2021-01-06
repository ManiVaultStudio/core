#include "PopupPushButton.h"
#include "PopupWidget.h"

hdps::gui::PopupPushButton::PopupPushButton(QWidget* parent) :
    QPushButton(parent),
    _popupWidget(nullptr),
    _alignment(Qt::AlignLeft)
{
    QObject::connect(this, &QPushButton::clicked, [this]() {
        if (_popupWidget == nullptr)
            return;

        /*
        switch (_alignment)
        {
            case Qt::AlignLeft:
                break;
            case Qt::AlignRight:
                break;
            case Qt::AlignHCenter:
                break;
            case Qt::AlignJustify:
                break;
            case Qt::AlignAbsolute:
                break;
            case Qt::AlignHorizontal_Mask:
                break;
            case Qt::AlignTop:
                break;
            case Qt::AlignBottom:
                break;
            case Qt::AlignVCenter:
                break;
            case Qt::AlignBaseline:
                break;
            case Qt::AlignVertical_Mask:
                break;
            case Qt::AlignCenter:
                break;
            default:
                break;
        }
        */

        _popupWidget->move(mapToGlobal(rect().bottomLeft()) - QPoint(width(), 0));
    });
}

QWidget* hdps::gui::PopupPushButton::getPopupWidget()
{
    return _popupWidget;
}

void hdps::gui::PopupPushButton::setPopupWidget(QWidget* popupWidget)
{
    Q_ASSERT(popupWidget != nullptr);

    if (popupWidget == _popupWidget)
        return;

    _popupWidget = popupWidget;
}

Qt::AlignmentFlag hdps::gui::PopupPushButton::getPopupWidgetAlignment() const
{
    return _alignment;
}

void hdps::gui::PopupPushButton::setPopupWidgetAlignment(const Qt::AlignmentFlag& alignment)
{
    if (alignment == _alignment)
        return;

    _alignment = alignment;
}