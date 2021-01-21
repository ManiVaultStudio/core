#pragma once

#include <QWidget>

class QHBoxLayout;

namespace hdps {

namespace gui {

class ResponsiveToolBar : public QWidget
{
    Q_OBJECT

public:
    ResponsiveToolBar(QWidget* parent = nullptr);

    void setListenWidget(QWidget* listenWidget);

    bool eventFilter(QObject* target, QEvent* event);

    void addWidget(QWidget* widget);
    void addStretch(const std::int32_t& stretch = 0);

private:
    void updateLayout(QWidget* widget = nullptr);
    QList<QWidget*> getVisibleWidgets();

private:
    QWidget*            _listenWidget;      /** TODO */
    QHBoxLayout*        _layout;            /** TODO */
    QList<QWidget*>     _widgets;           /** TODO */

public:
    static const std::int32_t LAYOUT_MARGIN;
    static const std::int32_t LAYOUT_SPACING;
    static const QSize ICON_SIZE;
};

}
}