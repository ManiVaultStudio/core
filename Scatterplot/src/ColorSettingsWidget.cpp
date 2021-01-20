#include "ColorSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

using namespace hdps::gui;

ColorSettingsWidget::ColorDimensionWidget::ColorDimensionWidget() :
    QStackedWidget(),
    _colorDimensionComboBox(new QComboBox()),
    _widget(new QWidget()),
    _layout(new QHBoxLayout()),
    _colorDataLineEdit(new QLineEdit()),
    _removeColorDataPushButton(new QPushButton())
{
    addWidget(_colorDimensionComboBox);
    addWidget(_widget);

    _widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _widget->setLayout(_layout);

    _layout->setMargin(ResponsiveToolBar::Widget::LAYOUT_MARGIN);
    _layout->setSpacing(ResponsiveToolBar::Widget::LAYOUT_SPACING);

    _colorDataLineEdit->setEnabled(false);

    _removeColorDataPushButton->setIconSize(ResponsiveToolBar::ICON_SIZE);
    _removeColorDataPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));

    _layout->addWidget(_colorDataLineEdit);
    _layout->addWidget(_removeColorDataPushButton);
}

QComboBox* ColorSettingsWidget::ColorDimensionWidget::getColorDimensionComboBox()
{
    return _colorDimensionComboBox;
}

ColorSettingsWidget::ColorSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::Widget("Color"),
    _colorByLabel(new QLabel()),
    _colorByComboBox(new QComboBox()),
    _colorDimensionWidget(new ColorDimensionWidget())
{
    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("palette"));

    _colorByLabel->setToolTip("Color by");
    _colorByLabel->setText("Color by:");

    _colorByComboBox->setToolTip("Color by");
    _colorByComboBox->setFixedWidth(75);
    _colorByComboBox->addItem("Dimension");
    _colorByComboBox->addItem("Data");

    computeStateSizes();
}

void ColorSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    connect(_colorByComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        _colorDimensionWidget->setCurrentIndex(index);
    });

    connect(_colorDimensionWidget->getColorDimensionComboBox(), qOverload<int>(&QComboBox::currentIndexChanged), [this, &plugin](int index) {
        const_cast<ScatterplotPlugin&>(plugin).cDimPicked(index);
    });

    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto renderModeChanged = [this, scatterPlotWidget]() {
        setEnabled(scatterPlotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, renderModeChanged](const ScatterplotWidget::RenderMode& renderMode) {
        renderModeChanged();
    });

    renderModeChanged();
}

void ColorSettingsWidget::updateState()
{
    /*
    auto layout = new QHBoxLayout();

    setWidgetLayout(layout);

    layout->addWidget(_colorByLabel);
    layout->addWidget(_colorByComboBox);
    layout->addWidget(_colorDimensionWidget);

    layout->invalidate();
    layout->activate();

    switch (_state)
    {
        case State::Popup:
            setCurrentIndex(0);
            break;

        case State::Compact:
            _colorDimensionWidget->setFixedWidth(80);
            setCurrentIndex(1);
            break;

        case State::Full:
            _colorDimensionWidget->setFixedWidth(120);
            setCurrentIndex(1);
            break;

        default:
            break;
    }
    */
}

void ColorSettingsWidget::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    QSignalBlocker signalBlocker(_colorDimensionWidget->getColorDimensionComboBox());

    _colorDimensionWidget->getColorDimensionComboBox()->setModel(&stringListModel);
}