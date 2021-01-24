#include "SettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "RenderModeWidget.h"
#include "PointSettingsWidget.h"
#include "DensitySettingsWidget.h"
#include "PositionSettingsWidget.h"
#include "ColorSettingsWidget.h"
#include "SubsetSettingsWidget.h"
#include "SelectionSettingsWidget.h"

#include "widgets/ResponsiveToolBar.h"

#include <QPushButton>
#include <QDialog>

const hdps::Vector3f SettingsWidget::DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
const hdps::Vector3f SettingsWidget::DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

class StatefulWidget : public QWidget {
public:
    enum class State {
        Popup,
        Compact,
        Full
    };

    StatefulWidget(const QString& name, const std::int32_t& priority = 0) :
        QWidget(),
        _name(name),
        _priority(priority),
        _invisibleWidget(new QWidget()),
        _sizeHints({QSize(), QSize(), QSize()}),
        _pushButton(new QPushButton())
    {
        setObjectName("StatefulWidget");
        setStyleSheet("QWidget#StatefulWidget { background-color: green;}");
        //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        this->installEventFilter(this);

        _sizeHints[0] = getStateSizeHint(State::Popup);
        _sizeHints[1] = getStateSizeHint(State::Compact);
        _sizeHints[2] = getStateSizeHint(State::Full);

        setProperty("sizeHintMin", _sizeHints[0]);
        setProperty("sizeHintMax", _sizeHints[2]);

        changeState(State::Popup);
    }

    bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::DynamicPropertyChange)
        {
            auto propertyChangeEvent = static_cast<QDynamicPropertyChangeEvent*>(event);

            if (propertyChangeEvent) {
                const auto propertyName = QString(propertyChangeEvent->propertyName().data());

                if (propertyName == "availableWidth")
                    availableWidthChanged();
            }
        }

        return QObject::eventFilter(object, event);
    }

    void availableWidthChanged() {
        auto state = State::Popup;

        if (getAvailableWidth() >= _sizeHints[1].width())
            state = State::Compact;

        if (getAvailableWidth() >= _sizeHints[2].width())
            state = State::Full;
    
        changeState(state);
    }

    std::int32_t getAvailableWidth() const
    {
        const auto availableWidthProperty = property("availableWidth");
        return availableWidthProperty.toInt();
    }

    void changeState(const State& state) {
        if (layout())
            delete layout();

        setLayout(getLayout(state));
        update();
    }

    QLayout* getLayout(const State& state) {
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
                _pushButton->setFixedWidth(150);
                break;

            case State::Full:
                _pushButton->setText("Full");
                _pushButton->setFixedWidth(300);

                break;
            default:
                break;
        }

        return layout;
    }

    QSize getStateSizeHint(const State& state) {
        if (_invisibleWidget->layout())
            delete _invisibleWidget->layout();

        _invisibleWidget->setLayout(getLayout(state));

        return _invisibleWidget->sizeHint();
    }

    QString         _name;
    std::int32_t    _priority;
    QWidget*        _invisibleWidget;
    QList<QSize>    _sizeHints;
    QPushButton*    _pushButton;
};

class ToolBar : public QWidget {
public:
    class ProxyWidget : public QWidget {
    public:
        ProxyWidget(QWidget* targetWidget) :
            QWidget(),
            _targetWidget(targetWidget)
        {
            setObjectName("TargetWidget");
            setStyleSheet("QWidget#TargetWidget { background-color: red;}");
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            this->installEventFilter(this);
        }

        QSize sizeHint() const {
            return _targetWidget->property("maxSizeHint").toSize();
        };

        bool eventFilter(QObject* target, QEvent* event)
        {
            switch (event->type()) {
                case QEvent::Resize:
                {
                    //qDebug() << _targetWidget->windowTitle() << static_cast<QResizeEvent*>(event)->size();
                    _targetWidget->setProperty("availableWidth", width());
                    break;
                }

                default:
                    break;
            }

            return QObject::eventFilter(target, event);
        }

        QWidget*     _targetWidget;
    };

    ToolBar() :
        QWidget(),
        _mainLayout(new QVBoxLayout()),
        _proxyLayout(new QHBoxLayout()),
        _widgetLayout(new QHBoxLayout()),
        _statefulWidgets()
    {
        setLayout(_mainLayout);

        _mainLayout->addLayout(_proxyLayout);
        _mainLayout->addLayout(_widgetLayout);

        this->installEventFilter(this);
    }
    
    void addWidget(StatefulWidget* statefulWidget, const std::int32_t& stretch = 0) {
        _proxyLayout->addWidget(new ProxyWidget(statefulWidget), stretch);
        _widgetLayout->addWidget(statefulWidget);
    }

    void addStretch(const std::int32_t& stretch = 1) {
        _widgetLayout->addStretch(stretch);
    }

    bool eventFilter(QObject* target, QEvent* event)
    {
        switch (event->type()) {
            case QEvent::Resize:
                //qDebug() << "Toolbar:" << sizeHint();
                break;

            default:
                break;
        }

        return QObject::eventFilter(target, event);
    }

    QVBoxLayout*            _mainLayout;
    QHBoxLayout*            _proxyLayout;
    QHBoxLayout*            _widgetLayout;
    QList<StatefulWidget*>  _statefulWidgets;
};

//class StatefulWidgetA : public QWidget, public StatefulWidget

SettingsWidget::SettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(static_cast<QWidget*>(&const_cast<ScatterplotPlugin&>(plugin))),
    _responsiveToolBar(new hdps::gui::ResponsiveToolBar(this)),
    _renderModeWidget(new RenderModeWidget(this)),
    _pointSettingsWidget(new PointSettingsWidget(this)),
    _densitySettingsWidget(new DensitySettingsWidget(this)),
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
    _pointSettingsWidget->setScatterPlotPlugin(plugin);
    _densitySettingsWidget->setScatterPlotPlugin(plugin);
    _positionSettingsWidget->setScatterPlotPlugin(plugin);
    _colorSettingsWidget->setScatterPlotPlugin(plugin);
    _subsetSettingsWidget->setScatterPlotPlugin(plugin);
    _selectionSettingsWidget->setScatterPlotPlugin(plugin);

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
    _responsiveToolBar->addStretch();

    //setEnabled(false);

    connect(&plugin, &ScatterplotPlugin::currentDatasetChanged, [this](const QString& currentDataset) {
        setEnabled(!currentDataset.isEmpty());
    });

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
    /*
    auto layoutWidget = new QWidget();
    layoutWidget->show();

    */
    auto findDialog = new QDialog(this);
    findDialog->setLayout(new QVBoxLayout());
    
    
    
    auto toolbar = new ToolBar();

    findDialog->layout()->addWidget(toolbar);

    toolbar->addWidget(new StatefulWidget("A", 10));// , 1);
    toolbar->addWidget(new StatefulWidget("B", 100));//, 4);
    toolbar->addStretch();

    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
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