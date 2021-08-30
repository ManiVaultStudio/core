#include "ColorMapAction.h"

#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QComboBox>
#include <QAbstractItemView>
#include <QItemSelectionModel>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapAction::ColorMapAction(QObject* parent, const QString& title /*= ""*/, const ColorMap::Type& colorMapType /*= ColorMap::Type::OneDimensional*/, const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/) :
    WidgetAction(parent),
    _currentColorMapAction(this, "Current color map"),
    _settingsAction(this),
    _filteredColorMapModel(this, colorMapType)
{
    setText(title);

    _filteredColorMapModel.setSourceModel(ColorMapModel::getGlobalInstance());
    _currentColorMapAction.setCustomModel(&_filteredColorMapModel);

    initialize(colorMap, defaultColorMap);

    connect(&_currentColorMapAction, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) -> void {
        emit colorMapImageChanged(getColorMapImage());
    });
}

void ColorMapAction::initialize(const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/)
{
    _currentColorMapAction.setCurrentText(colorMap);
    _currentColorMapAction.setDefaultText(defaultColorMap);
}

QString ColorMapAction::getColorMap() const
{
    return _currentColorMapAction.getCurrentText();
}

QImage ColorMapAction::getColorMapImage() const
{
    if (_currentColorMapAction.getModel() == nullptr)
        return QImage();

    const auto filteredModelIndex = _currentColorMapAction.getModel()->index(_currentColorMapAction.getCurrentIndex(), 0);
    return filteredModelIndex.siblingAtColumn(static_cast<std::int32_t>(ColorMapModel::Column::Image)).data(Qt::EditRole).value<QImage>();
}

void ColorMapAction::setColorMap(const QString& colorMap)
{
    Q_ASSERT(!colorMap.isEmpty());

    _currentColorMapAction.setCurrentText(colorMap);

}

QString ColorMapAction::getDefaultColorMap() const
{
    return _currentColorMapAction.getDefaultText();
}

void ColorMapAction::setDefaultColorMap(const QString& defaultColorMap)
{
    Q_ASSERT(!defaultColorMap.isEmpty());

    _currentColorMapAction.setDefaultText(defaultColorMap);
}

ColorMapAction::ComboboxWidget::ComboboxWidget(QWidget* parent, OptionAction* optionAction, ColorMapAction* colorMapAction, const WidgetActionWidget::State& state) :
    OptionAction::ComboBoxWidget(parent, optionAction),
    _colorMapAction(colorMapAction)
{
}

void ColorMapAction::ComboboxWidget::paintEvent(QPaintEvent* paintEvent)
{
    OptionAction::ComboBoxWidget::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    const auto margin               = 5;
    const auto colorMapRectangle    = rect().marginsRemoved(QMargins(margin, margin, 20, margin));

    painter.drawImage(colorMapRectangle, _colorMapAction->getColorMapImage());
    painter.setPen(QPen(QBrush(Qt::black), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(colorMapRectangle);
}

ColorMapAction::Widget::Widget(QWidget* parent, ColorMapAction* colorMapAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, colorMapAction, state),
    _comboBoxWidget(dynamic_cast<QComboBox*>(new ComboboxWidget(parent, &colorMapAction->getCurrentColorMapAction(), colorMapAction, state))),
    _settingsWidget(colorMapAction->getColorMapSettingsAction().createCollapsedWidget(this))
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    layout->addWidget(_comboBoxWidget);
    layout->addWidget(_settingsWidget);

    setLayout(layout);

    connect(_comboBoxWidget, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) -> void {
        update();
    });
}

}
}
