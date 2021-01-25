#include "SettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "RenderModeWidget.h"
#include "PlotSettingsWidget.h"
#include "PositionSettingsWidget.h"
#include "ColorSettingsWidget.h"
#include "SubsetSettingsWidget.h"
#include "SelectionSettingsWidget.h"

#include "widgets/ResponsiveToolBar.h"

#include <QPushButton>
#include <QDialog>
#include <QPropertyAnimation>

const hdps::Vector3f SettingsWidget::DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
const hdps::Vector3f SettingsWidget::DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

class StatefulWidget : private QObject {
public:
    enum class State {
        Popup,
        Compact,
        Full
    };

    StatefulWidget(QWidget* widget, const QString& name, const std::int32_t& priority = 0) :
        QObject(),
        _state(State::Popup),
        _widget(widget),
        _name(name),
        _priority(priority),
        _sizeHintWidget(new QWidget()),
        _sizeHints({QSize(), QSize(), QSize()})
    {
        _widget->setObjectName("StatefulWidget");
        //_widget->setStyleSheet("QWidget#StatefulWidget { background-color: green;}");
        _widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    }

    void initialize() {
        _widget->installEventFilter(this);

        _sizeHints[0] = getStateSizeHint(State::Popup);
        _sizeHints[1] = getStateSizeHint(State::Compact);
        _sizeHints[2] = getStateSizeHint(State::Full);

        _widget->setProperty("sizeHints", QVariant::fromValue(_sizeHints));
        _widget->setProperty("sizeHintMin", _sizeHints[0]);
        _widget->setProperty("sizeHintMax", _sizeHints[2]);

        changeState(State::Popup);
    }

protected:
    virtual QLayout* getLayout(const State& state) = 0;

private:
    bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::DynamicPropertyChange)
        {
            auto propertyChangeEvent = static_cast<QDynamicPropertyChangeEvent*>(event);

            if (propertyChangeEvent) {
                const auto propertyName = QString(propertyChangeEvent->propertyName().data());

                if (propertyName == "availableWidth")
                    availableWidthChanged();

                if (propertyName == "state")
                    changeState(getState());
            }
        }

        return QObject::eventFilter(object, event);
    }

    State getState() const
    {
        const auto stateProperty = _widget->property("state");
        return static_cast<State>(stateProperty.toInt());
    }

    void availableWidthChanged() {
        
        /*
        if (getAvailableWidth() > 0) {
            switch (_state)
            {
                case StatefulWidget::State::Popup:
                {
                    if (getAvailableWidth() >= (_sizeHints[1].width() - _sizeHints[0].width()))
                        changeState(StatefulWidget::State::Compact);

                    break;
                }

                case StatefulWidget::State::Compact:
                {
                    if (getAvailableWidth() >= (_sizeHints[2].width() - _sizeHints[1].width()))
                        changeState(StatefulWidget::State::Full);

                    break;
                }

                case StatefulWidget::State::Full:
                    break;

                default:
                    break;
            }
        }
        else {
            switch (_state)
            {
                case StatefulWidget::State::Popup:
                    break;

                case StatefulWidget::State::Compact:
                {
                    if ((_sizeHints[1].width() - _sizeHints[0].width()) > abs(getAvailableWidth()))
                        changeState(StatefulWidget::State::Popup);

                    break;
                }

                case StatefulWidget::State::Full:
                {
                    if ((_sizeHints[1].width() - _sizeHints[0].width()) > abs(getAvailableWidth()))
                        changeState(StatefulWidget::State::Popup);

                    break;
                }

                default:
                    break;
            }
        }
        */
    }

    std::int32_t getAvailableWidth() const
    {
        const auto availableWidthProperty = _widget->property("availableWidth");
        return availableWidthProperty.toInt();
    }

    void changeState(const State& state) {
        _state = state;

        

        if (_widget->layout())
            delete _widget->layout();

        auto layout = getLayout(state);

        layout->setMargin(LAYOUT_MARGIN);
        layout->setSpacing(LAYOUT_SPACING);

        _widget->setLayout(layout);
        _widget->setWindowOpacity(0);
    }

    QSize getStateSizeHint(const State& state) {
        if (_sizeHintWidget->layout())
            delete _sizeHintWidget->layout();

        _sizeHintWidget->setLayout(getLayout(state));

        return _sizeHintWidget->sizeHint();
    }

    State           _state;
    QWidget*        _widget;
    QString         _name;
    std::int32_t    _priority;
    QWidget*        _sizeHintWidget;
    QList<QSize>    _sizeHints;

public:
    static const std::int32_t LAYOUT_MARGIN;
    static const std::int32_t LAYOUT_SPACING;
};

const std::int32_t StatefulWidget::LAYOUT_MARGIN = 0;
const std::int32_t StatefulWidget::LAYOUT_SPACING = 4;

class ToolBar : public QWidget {
public:
    /*
    class ProxyWidget : public QWidget {
    public:
        ProxyWidget(QWidget* targetWidget) :
            QWidget(),
            _targetWidget(targetWidget)
        {
            Q_ASSERT(targetWidget != nullptr);

            setObjectName("TargetWidget");
            setStyleSheet("QWidget#TargetWidget { background-color: red;}");
            //setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);

            //setMinimumSize(targetWidget->property("sizeHintMin").toSize());
            //setMaximumSize(targetWidget->property("sizeHintMax").toSize());

            this->installEventFilter(this);
        }

        QSize sizeHint() const {
            return _targetWidget->property("sizeHintMax").toSize();
        };

        bool eventFilter(QObject* target, QEvent* event)
        {
            switch (event->type()) {
                case QEvent::Resize:
                    //_targetWidget->setProperty("availableWidth", width());
                    break;

                default:
                    break;
            }

            return QObject::eventFilter(target, event);
        }

        QWidget*     _targetWidget;
    };
    */

    ToolBar() :
        QWidget(),
        _mainLayout(new QHBoxLayout()),
        _proxyLayout(new QHBoxLayout()),
        _widgetLayout(new QHBoxLayout()),
        _widgets(),
        _widget(new QWidget())
    {
        setLayout(_mainLayout);
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

        _mainLayout->setMargin(StatefulWidget::LAYOUT_MARGIN);
        _mainLayout->setSpacing(StatefulWidget::LAYOUT_SPACING);

        //_mainLayout->addLayout(_proxyLayout);
        //_mainLayout->addLayout(_widgetLayout);
        _mainLayout->addWidget(_widget);
        _mainLayout->addStretch(1);

        //_widget->setParent(this);
        //_proxyLayout->setMargin(0);
        //_proxyLayout->setSpacing(1);

        _widgetLayout->setMargin(StatefulWidget::LAYOUT_MARGIN);
        _widgetLayout->setSpacing(StatefulWidget::LAYOUT_SPACING);
        _widgetLayout->setSizeConstraint(QLayout::SetFixedSize);

        _widget->setObjectName("SpacerWidget");
        _widget->setStyleSheet("QWidget#SpacerWidget { background-color: yellow;}");
        _widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

        _widget->setLayout(_widgetLayout);
        //_widget->installEventFilter(this);

        //_widgetLayout->addWidget(_spacerWidget);

        this->installEventFilter(this);
        //_spacerWidget->installEventFilter(this);
    }
    
    void addWidget(QWidget* widget, const std::int32_t& stretch = 0) {
        _widgets << widget;

        //widget->installEventFilter(this);

        //_proxyLayout->addWidget(new ProxyWidget(widget), 1);
        _widgetLayout->insertWidget(_widgetLayout->count() - 1, widget);
    }

    void addStretch(const std::int32_t& stretch = 1) {
        //_widgetLayout->addStretch(stretch);
    }

    bool eventFilter(QObject* object, QEvent* event)
    {
        auto widget = dynamic_cast<QWidget*>(object);

        switch (event->type()) {
            case QEvent::Resize:
            {
                auto availableWidth = width() - _widget->width();

                QMap<QWidget*, StatefulWidget::State> states;

                for (auto widget : _widgets)
                    states[widget] = StatefulWidget::State::Popup;

                const auto getWidgetWidth = [this](QWidget* widget, const std::int32_t& state) -> std::int32_t {
                    const auto sizeHintsProperty = widget->property("sizeHints");

                    if (sizeHintsProperty.isValid())
                        return sizeHintsProperty.value<QList<QSize>>().at(state).width();

                    return widget->width();
                };

                auto runningWidth = 0;

                //if (availableWidth > 0) {
                    for (auto widget : _widgets) {
                        const auto delta = getWidgetWidth(widget, 0);

                        runningWidth += delta;

                        states[widget] = StatefulWidget::State::Popup;
                    }

                    for (auto widget : _widgets) {
                        const auto delta = getWidgetWidth(widget, 1) - getWidgetWidth(widget, 0);

                        runningWidth += delta;

                        if (runningWidth < width())
                            states[widget] = StatefulWidget::State::Compact;
                    }
                    
                    for (auto widget : _widgets) {
                        const auto delta = getWidgetWidth(widget, 2) - getWidgetWidth(widget, 1);

                        runningWidth += delta;

                        if (runningWidth < width())
                            states[widget] = StatefulWidget::State::Full;
                    }
                //}

                for (auto widget : _widgets)
                    widget->setProperty("state", static_cast<std::int32_t>(states[widget]));

                //qDebug() << availableWidth;
                
                break;
            }

            default:
                break;
        }

        return QObject::eventFilter(object, event);
    }

    /*QSize sizeHint() const {
        return QSize(20, 20);
    };*/

    QHBoxLayout*        _mainLayout;
    QHBoxLayout*        _proxyLayout;
    QHBoxLayout*        _widgetLayout;
    QList<QWidget*>     _widgets;
    QWidget*            _widget;
};

class StatefulWidgetA : public QWidget, public StatefulWidget {
public:
    StatefulWidgetA() :
        QWidget(),
        StatefulWidget(this, "A"),
        _pushButton(new QPushButton)
    {
        //setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        initialize();
    }

    QLayout* getLayout(const State& state) override {
        auto layout = new QHBoxLayout();

        layout->addWidget(_pushButton);

        switch (state)
        {
            case State::Popup:
                _pushButton->setText("Popup");
                _pushButton->setFixedWidth(50);
                break;

            case State::Compact:
                _pushButton->setText("Compact");
                _pushButton->setFixedWidth(100);
                break;

            case State::Full:
                _pushButton->setText("Full");
                _pushButton->setFixedWidth(150);

                break;
            default:
                break;
        }

        return layout;
    }

    QPushButton*    _pushButton;
};

class StatefulWidgetB : public QWidget, public StatefulWidget {
public:
    StatefulWidgetB() :
        QWidget(),
        StatefulWidget(this, "B"),
        _pushButton(new QPushButton)
    {
        initialize();
    }

    QLayout* getLayout(const State& state) override {
        auto layout = new QHBoxLayout();

        layout->addWidget(_pushButton);

        switch (state)
        {
            case State::Popup:
                _pushButton->setText("Popup");
                _pushButton->setFixedWidth(50);
                break;

            case State::Compact:
                _pushButton->setText("Compact");
                _pushButton->setFixedWidth(400);
                break;

            case State::Full:
                _pushButton->setText("Full");
                _pushButton->setFixedWidth(500);

                break;
            default:
                break;
        }

        return layout;
    }

    QPushButton*    _pushButton;
};

#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

class StatefulWidgetC : public QWidget, public StatefulWidget {
public:
    StatefulWidgetC() :
        QWidget(),
        StatefulWidget(this, "C"),
        _label(new QLabel()),
        _doubleSpinBox(new QDoubleSpinBox()),
        _slider(new QSlider())
    {
        initialize();

        _label->setText("Label");
        _slider->setOrientation(Qt::Horizontal);
    }

    QLayout* getLayout(const State& state) override {
        auto layout = new QHBoxLayout();

        layout->addWidget(_label);
        layout->addWidget(_doubleSpinBox);
        layout->addWidget(_slider);

        switch (state)
        {
            case State::Popup:
                _label->setText("Popup");
                _doubleSpinBox->hide();
                _slider->setFixedWidth(50);
                break;

            case State::Compact:
                _label->setText("Compact label");
                _doubleSpinBox->hide();
                _slider->setFixedWidth(200);
                break;

            case State::Full:
                _label->setText("Full label");
                _doubleSpinBox->show();
                _slider->setFixedWidth(200);

                break;
            default:
                break;
        }

        return layout;
    }

    QLabel*             _label;
    QDoubleSpinBox*     _doubleSpinBox;
    QSlider*            _slider;
};

SettingsWidget::SettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(static_cast<QWidget*>(&const_cast<ScatterplotPlugin&>(plugin))),
    _responsiveToolBar(new hdps::gui::ResponsiveToolBar(this)),
    _renderModeWidget(new RenderModeWidget(this)),
    _plotSettingsWidget(new PlotSettingsWidget(this)),
    _positionSettingsWidget(new PositionSettingsWidget(this)),
    _colorSettingsWidget(new ColorSettingsWidget(this)),
    _subsetSettingsWidget(new SubsetSettingsWidget(this)),
    _selectionSettingsWidget(new SelectionSettingsWidget(this)),
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    setAutoFillBackground(true);
    auto horizontalLayout = new QHBoxLayout();

    horizontalLayout->setMargin(0);
    horizontalLayout->setSpacing(0);
    horizontalLayout->addWidget(_responsiveToolBar);

    setLayout(horizontalLayout);

    _responsiveToolBar->setListenWidget(&const_cast<ScatterplotPlugin&>(plugin));

    _renderModeWidget->setScatterPlotPlugin(plugin);
    _plotSettingsWidget->setScatterPlotPlugin(plugin);
    _positionSettingsWidget->setScatterPlotPlugin(plugin);
    _colorSettingsWidget->setScatterPlotPlugin(plugin);
    _subsetSettingsWidget->setScatterPlotPlugin(plugin);
    _selectionSettingsWidget->setScatterPlotPlugin(plugin);

    _positionSettingsWidget->hide();
    _colorSettingsWidget->hide();
    _subsetSettingsWidget->hide();
    _selectionSettingsWidget->hide();

    _responsiveToolBar->addStatefulWidget(_renderModeWidget);
    _responsiveToolBar->addStatefulWidget(_plotSettingsWidget);

    /*
    _renderModeWidget->setProperty("priority", 1000);
    _pointSettingsWidget->setProperty("priority", 200);
    _densitySettingsWidget->setProperty("priority", 200);
    _positionSettingsWidget->setProperty("priority", 1500);
    _colorSettingsWidget->setProperty("priority", 1200);
    _subsetSettingsWidget->setProperty("priority", 600);
    _selectionSettingsWidget->setProperty("priority", 800);
    
    _responsiveToolBar->addWidget(_renderModeWidget);
    _responsiveToolBar->addWidget(_pointSettingsWidget);
    _responsiveToolBar->addWidget(_densitySettingsWidget);
    _responsiveToolBar->addWidget(_positionSettingsWidget);
    _responsiveToolBar->addWidget(_colorSettingsWidget);
    _responsiveToolBar->addWidget(_subsetSettingsWidget);
    _responsiveToolBar->addWidget(_selectionSettingsWidget);
    */
    _responsiveToolBar->addStretch();

    //setEnabled(false);

    connect(&plugin, &ScatterplotPlugin::currentDatasetChanged, [this](const QString& currentDataset) {
        setEnabled(!currentDataset.isEmpty());
    });

    /*
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto updatePlotSettingsUI = [this, scatterPlotWidget]() {
        const auto renderMode = scatterPlotWidget->getRenderMode();

        _pointSettingsWidget->setVisible(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _densitySettingsWidget->setVisible(renderMode != ScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updatePlotSettingsUI](const ScatterplotWidget::RenderMode& renderMode) {
        updatePlotSettingsUI();
    });

    updatePlotSettingsUI();
    */

    /*
    auto layoutWidget = new QWidget();
    layoutWidget->show();

    
    auto findDialog = new QDialog(this);
    findDialog->setLayout(new QVBoxLayout());
    
    
    
    auto toolbar = new ToolBar();

    findDialog->layout()->addWidget(toolbar);

    toolbar->addWidget(new StatefulWidgetA());
    toolbar->addWidget(new StatefulWidgetB());
    toolbar->addWidget(new StatefulWidgetC());
    //toolbar->addWidget(new StatefulWidget("B", 100));//, 4);
    toolbar->addStretch();

    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
    */
}

int SettingsWidget::getXDimension()
{
    return _positionSettingsWidget->getDimensionX();
}

int SettingsWidget::getYDimension()
{
    return _positionSettingsWidget->getDimensionY();
}

hdps::Vector3f SettingsWidget::getBaseColor()
{
    return _baseColor;
}

hdps::Vector3f SettingsWidget::getSelectionColor()
{
    return _selectionColor;
}

void SettingsWidget::initDimOptions(const unsigned int nDim)
{
    _positionSettingsWidget->setDimensions(nDim);
}

void SettingsWidget::initDimOptions(const std::vector<QString>& dimNames)
{
    _positionSettingsWidget->setDimensions(dimNames.size(), dimNames);
}

void SettingsWidget::initScalarDimOptions(const unsigned int nDim)
{
    _colorSettingsWidget->setScalarDimensions(nDim);
}

void SettingsWidget::initScalarDimOptions(const std::vector<QString>& dimNames)
{
    _colorSettingsWidget->setScalarDimensions(dimNames.size(), dimNames);
}