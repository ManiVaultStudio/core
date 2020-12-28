#include "SettingsWidget.h"
#include "AccordionSection.h"

#include <QVariant>

namespace hdps
{

namespace gui
{

SettingsWidget::SettingsWidget(QWidget* parent /*= nullptr*/)
{
    auto layout = new QVBoxLayout();

    //layout->setMargin(5);

    setLayout(layout);
}

void SettingsWidget::addWidget(QWidget* widget)
{
    layout()->addWidget(widget);
}

}
}