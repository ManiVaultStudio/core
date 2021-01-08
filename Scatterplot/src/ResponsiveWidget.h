#pragma once

#include "util/WidgetResizeEventProxy.h"

#include <QWidget>
#include <QIcon>
#include <QGroupBox>

class ResponsiveWidget : public QObject {
public:
    typedef std::function<bool(const QSize& sourceWidgetSize)> VisibilityCallback;

public:
    ResponsiveWidget(QWidget* widget, VisibilityCallback visibilityCallback);

    bool shouldBeVisible(const QSize& sourceWidgetSize) const;

protected:
    QWidget*            _widget;
    VisibilityCallback  _visibilityCallback;
};