#include "WidgetActionWidget.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>

namespace hdps::gui {

WidgetActionWidget::WidgetActionWidget(QWidget* parent, WidgetAction* action, const std::int32_t& widgetFlags /*= 0*/) :
    WidgetActionViewWidget(parent, action),
    _widgetFlags(widgetFlags)
{
    setAction(action);
}

QSize WidgetActionWidget::sizeHint() const
{
    if (_widgetFlags & WidgetFlag::PopupLayout) {
        auto popupSizeHint = const_cast<WidgetActionWidget*>(this)->getAction()->getPopupSizeHint();

        if (!popupSizeHint.isNull())
            return popupSizeHint;
    }

    return QWidget::sizeHint();
}

void WidgetActionWidget::setLayout(QLayout* layout)
{
    if (isPopup())
        setPopupLayout(layout);
    else
        WidgetActionViewWidget::setLayout(layout);
}

void WidgetActionWidget::setPopupLayout(QLayout* popupLayout)
{
    auto mainLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(4, 4, 4, 4);

    WidgetActionViewWidget::setLayout(mainLayout);

    auto groupBox = new QGroupBox(getAction()->text());

    groupBox->setLayout(popupLayout);
    groupBox->setCheckable(getAction()->isCheckable());

    mainLayout->addWidget(groupBox);

    const auto update = [this, groupBox]() -> void {
        QSignalBlocker blocker(getAction());

        groupBox->setTitle(getAction()->text());
        groupBox->setToolTip(getAction()->text());
        groupBox->setChecked(getAction()->isChecked());
    };

    connect(groupBox, &QGroupBox::toggled, this, [this](bool toggled) {
        getAction()->setChecked(toggled);
    });

    connect(getAction(), &WidgetAction::changed, this, [update]() {
        update();
    });

    update();
}

bool WidgetActionWidget::isPopup() const
{
    return _widgetFlags & PopupLayout;
}

}
