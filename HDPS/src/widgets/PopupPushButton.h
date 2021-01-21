#pragma once

#include <QPushButton>

class QWidget;
class QVBoxLayout;
class QGroupBox;

namespace hdps
{

namespace gui
{

class PopupPushButton : public QPushButton {
    Q_OBJECT

public:
    enum class PopupAlignmentFlag {

        // Docking side
        DockLeft        = 0x0001,
        DockRight       = 0x0002,
        DockTop         = 0x0004,
        DockBottom      = 0x0008,

        // Anchor location
        AnchorLeft      = 0x0010,
        AnchorRight     = 0x0020,
        AnchorTop       = 0x0040,
        AnchorBottom    = 0x0080,
        AnchorCenter    = 0x0100,

        LeftTop         = DockLeft | AnchorTop,
        LeftCenter      = DockLeft | AnchorCenter,
        LeftBottom      = DockLeft | AnchorBottom,

        TopRight        = DockTop | AnchorRight,
        TopCenter       = DockTop | AnchorCenter,
        TopLeft         = DockTop | AnchorLeft,

        RightTop        = DockRight | AnchorTop,
        RightCenter     = DockRight | AnchorCenter,
        RightBottom     = DockRight | AnchorBottom,

        BottomRight     = DockBottom | AnchorRight,
        BottomCenter    = DockBottom | AnchorCenter,
        BottomLeft      = DockBottom | AnchorLeft
    };

    Q_DECLARE_FLAGS(PopupAlignment, PopupAlignmentFlag)

public:
    PopupPushButton();

    void setWidget(QWidget* widget);

    bool eventFilter(QObject* object, QEvent* event);

    void paintEvent(QPaintEvent* paintEvent);

    PopupAlignment getPopupAlignment() const;
    void setPopupAlignment(const PopupAlignment& alignment);

signals:
    void popupClosed();

protected:
    QWidget*            _widget;
    PopupAlignment      _alignment;
    QWidget*            _popupWidget;
    QVBoxLayout*        _popupLayout;
    QGroupBox*          _groupBox;
    QVBoxLayout*        _groupBoxLayout;
};

}
}