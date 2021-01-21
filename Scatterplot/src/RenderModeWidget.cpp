#include "RenderModeWidget.h"
#include "ScatterplotPlugin.h"

#include "widgets/ResponsiveToolBar.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

using namespace hdps::gui;

RenderModeWidget::RenderModeWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _widgetState(this),
    _popupPushButton(new PopupPushButton()),
    _widget(new QWidget()),
    _scatterPlotPushButton(new QPushButton()),
    _densityPlotPushButton(new QPushButton()),
    _contourPlotPushButton(new QPushButton())
{
    initializeUI();

    connect(&_widgetState, &WidgetState::updateState, [this](const WidgetState::State& state) {
        const auto setWidgetLayout = [this](QLayout* layout) -> void {
            if (_widget->layout())
                delete _widget->layout();

            layout->setMargin(ResponsiveToolBar::LAYOUT_MARGIN);
            layout->setSpacing(ResponsiveToolBar::LAYOUT_SPACING);

            _widget->setLayout(layout);
        };

        auto& fontAwesome = Application::getIconFont("FontAwesome");

        switch (state)
        {
            case WidgetState::State::Popup:
            {
                /*
                auto layout = new QHBoxLayout();

                setWidgetLayout(layout);

                layout->addWidget(_scatterPlotPushButton);
                layout->addWidget(_densityPlotPushButton);
                layout->addWidget(_contourPlotPushButton);

                _scatterPlotPushButton->setText("Scatter Plot");
                _densityPlotPushButton->setText("Density Plot");
                _contourPlotPushButton->setText("Contour Plot");
                */
                setCurrentWidget(_popupPushButton);
                break;
            }

            case WidgetState::State::Compact:
            case WidgetState::State::Full:
            {
                auto layout = new QHBoxLayout();

                setWidgetLayout(layout);

                layout->addWidget(_scatterPlotPushButton);
                layout->addWidget(_densityPlotPushButton);
                layout->addWidget(_contourPlotPushButton);

                const auto isCompact = state == WidgetState::State::Compact;

                _scatterPlotPushButton->setText(isCompact ? "" : "Scatter Plot");
                _densityPlotPushButton->setText(isCompact ? "" : "Density Plot");
                _contourPlotPushButton->setText(isCompact ? "" : "Contour Plot");

                _scatterPlotPushButton->setIcon(isCompact ? fontAwesome.getIcon("braille") : QIcon());
                _densityPlotPushButton->setIcon(isCompact ? fontAwesome.getIcon("cloud") : QIcon());
                _contourPlotPushButton->setIcon(isCompact ? fontAwesome.getIcon("mountain") : QIcon());

                setCurrentWidget(_widget);
                break;
            }

            default:
                break;
        }
    });
    
    _widgetState.initialize();
}

void RenderModeWidget::initializeUI()
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("toggle-on"));

    _scatterPlotPushButton->setIconSize(ResponsiveToolBar::ICON_SIZE);
    _densityPlotPushButton->setIconSize(ResponsiveToolBar::ICON_SIZE);
    _contourPlotPushButton->setIconSize(ResponsiveToolBar::ICON_SIZE);

    _scatterPlotPushButton->setCheckable(true);
    _densityPlotPushButton->setCheckable(true);
    _contourPlotPushButton->setCheckable(true);

    addWidget(_popupPushButton);
    addWidget(_widget);
}

void RenderModeWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();
    
    const auto updateToggles = [this, scatterPlotWidget]() {
        const auto renderMode = scatterPlotWidget->getRenderMode();

        _scatterPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _densityPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::DENSITY);
        _contourPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::LANDSCAPE);
    };

    QObject::connect(_scatterPlotPushButton, &QPushButton::clicked, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
        updateToggles();
    });

    QObject::connect(_densityPlotPushButton, &QPushButton::clicked, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::DENSITY);
        updateToggles();
    });

    QObject::connect(_contourPlotPushButton, &QPushButton::clicked, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE);
        updateToggles();
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updateToggles](const ScatterplotWidget::RenderMode& renderMode) {
        updateToggles();
    });

    updateToggles();
}

/*
QIcon RenderModeWidget::getIcon(const ScatterplotWidget::RenderMode& renderMode) const
{
    
    QPixmap pixmap(_iconSize);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::Antialiasing);
    
    const auto iconRectangle    = QRect(0, 0, _iconSize.width(), _iconSize.height());
    const auto iconCenter       = QPointF(_iconSize.width() / 2.0f, _iconSize.height() / 2.0f);
    const auto iconRadius       = _iconSize.width() / 2.0f;

    switch (renderMode)
    {
        case ScatterplotWidget::SCATTERPLOT:
        {
            painter.setPen(QPen(QBrush(Qt::black), 3, Qt::SolidLine, Qt::RoundCap));
            painter.drawPoint(iconCenter + QPointF(5.0f, -5.0f));
            painter.drawPoint(iconCenter + QPointF(-4.0f, -4.0f));
            painter.drawPoint(iconCenter + QPointF(1.0f, 1.0f));
            painter.drawPoint(iconCenter + QPointF(-4.0f, 3.0f));
            painter.drawPoint(iconCenter + QPointF(7.0f, -2.0f));
            painter.drawPoint(iconCenter + QPointF(-8.0f, -1.0f));
            break;
        }

        case ScatterplotWidget::DENSITY:
        {
            QRadialGradient radialGradient(iconCenter, iconRadius);
            
            radialGradient.setColorAt(0, QColor(0, 0, 0, 255));
            radialGradient.setColorAt(1, QColor(0, 0, 0, 0));

            painter.setPen(QPen(QBrush(radialGradient), _iconSize.width(), Qt::SolidLine, Qt::RoundCap));
            painter.drawPoint(iconCenter);
            break;
        }

        case ScatterplotWidget::LANDSCAPE:
        {
            painter.setPen(QPen(QBrush(Qt::black), 1.52, Qt::SolidLine));

            auto radius = 4.0f;

            painter.drawEllipse(iconCenter, radius, radius);

            radius = 7.0f;

            painter.setBrush(QBrush(QColor(0, 0, 0, 60)));
            painter.drawEllipse(iconCenter, radius, radius);

            break;
        }

        default:
            break;
    }
    

    return QIcon(pixmap);
}
*/