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

ColorMapAction::ColorMapAction(QObject* parent, const QString& title /*= ""*/, const ColorMap::Type& colorMapType /*= ColorMap::Type::OneDimensional*/, const QString& colorMap /*= "RdYlBu"*/, const QString& defaultColorMap /*= "RdYlBu"*/) :
    WidgetAction(parent),
    _currentColorMapAction(this, "Current color map"),
    _settingsAction(this),
    _filteredColorMapModel(this, colorMapType)
{
    setText(title);

    _filteredColorMapModel.setSourceModel(ColorMapModel::getGlobalInstance());
    _currentColorMapAction.setCustomModel(&_filteredColorMapModel);

    initialize(colorMap, defaultColorMap);

    const auto notifyColorMapImageChanged = [this]() -> void {
        emit imageChanged(getColorMapImage());
    };

    connect(&_currentColorMapAction, &OptionAction::currentIndexChanged, this, [this, notifyColorMapImageChanged](const std::int32_t& currentIndex) -> void {
        notifyColorMapImageChanged();
    });

    connect(&_settingsAction.getInvertAction(), &ToggleAction::toggled, this, [this, notifyColorMapImageChanged](bool toggled) -> void {
        notifyColorMapImageChanged();
    });
}

void ColorMapAction::initialize(const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/)
{
    if (_filteredColorMapModel.rowCount() >= 1) {
        if (!colorMap.isEmpty())
            _currentColorMapAction.setCurrentText(colorMap);
        else
            _currentColorMapAction.setCurrentIndex(0);
    }

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

    // Get the model index of the selected color map image
    const auto filteredModelIndex = _currentColorMapAction.getModel()->index(_currentColorMapAction.getCurrentIndex(), 0);

    // Get the selected color map image
    auto colorMapImage = filteredModelIndex.siblingAtColumn(static_cast<std::int32_t>(ColorMapModel::Column::Image)).data(Qt::EditRole).value<QImage>();

    // Mirror if required
    if (const_cast<ColorMapAction*>(this)->getSettingsAction().getInvertAction().isChecked())
        colorMapImage = colorMapImage.mirrored(true, false);

    return colorMapImage;
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
    connect(colorMapAction, &ColorMapAction::imageChanged, this, [this](const QImage& colorMapImage) {
        update();
    });
}

void ColorMapAction::ComboboxWidget::paintEvent(QPaintEvent* paintEvent)
{
    OptionAction::ComboBoxWidget::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    QStyleOption styleOption;

    styleOption.init(this);
    
    const auto margin               = 5;
    const auto colorMapRectangle    = rect().marginsRemoved(QMargins(margin, margin, 20, margin));

    // Get color map image from the model
    auto colorMapeImage = _colorMapAction->getColorMapImage();

    // Convert to gray scale if disabled
    if (!isEnabled())
        colorMapeImage = colorMapeImage.convertToFormat(QImage::Format_Grayscale8);

    painter.drawImage(colorMapRectangle, colorMapeImage);
    painter.setPen(QPen(QBrush(isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::ButtonText) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText)), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(colorMapRectangle);
}

ColorMapAction::Widget::Widget(QWidget* parent, ColorMapAction* colorMapAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, colorMapAction, state),
    _comboBoxWidget(dynamic_cast<QComboBox*>(new ComboboxWidget(parent, &colorMapAction->getCurrentColorMapAction(), colorMapAction, state))),
    _settingsWidget(colorMapAction->getSettingsAction().createCollapsedWidget(this))
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
