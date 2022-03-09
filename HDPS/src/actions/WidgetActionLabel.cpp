#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>

namespace hdps {

namespace gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, Qt::WindowFlags windowFlags /*= Qt::WindowFlags()*/) :
    QWidget(parent, windowFlags),
    _widgetAction(widgetAction),
    _label()
{
    auto layout = new QVBoxLayout();

    layout->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    layout->setMargin(0);
    layout->addStretch(1);
    layout->addWidget(&_label);
    layout->addStretch(1);

    setLayout(layout);

    _label.setAlignment(Qt::AlignRight);

    const auto update = [this, widgetAction]() -> void {
        _label.setEnabled(widgetAction->isEnabled());
        _label.setText(QString("%1: ").arg(widgetAction->text()));
        _label.setToolTip(widgetAction->text());
        _label.setVisible(widgetAction->isVisible());
    };

    connect(widgetAction, &WidgetAction::changed, this, update);

    update();
}

}
}
