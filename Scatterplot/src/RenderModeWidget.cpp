#include "RenderModeWidget.h"
#include "ScatterplotPlugin.h"

#include <QVBoxLayout>
#include <QPushButton>

const QSize RenderModeWidget::BUTTON_SIZE_COMPACT = QSize(22, 22);
const QSize RenderModeWidget::BUTTON_SIZE_FULL = QSize(60, 22);

RenderModeWidget::RenderModeWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    WidgetStateMixin("Render mode"),
    _scatterPlotPushButton(new QPushButton()),
    _densityPlotPushButton(new QPushButton()),
    _contourPlotPushButton(new QPushButton())
{
    _scatterPlotPushButton->setCheckable(true);
    _densityPlotPushButton->setCheckable(true);
    _contourPlotPushButton->setCheckable(true);
}

void RenderModeWidget::initialize(const ScatterplotPlugin& plugin)
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

WidgetStateMixin::State RenderModeWidget::getState(const QSize& sourceWidgetSize) const
{
    const auto width = sourceWidgetSize.width();

    auto state = WidgetStateMixin::State::Popup;

    if (width >= 1000 && width < 1500)
        state = WidgetStateMixin::State::Compact;

    if (width >= 1500)
        state = WidgetStateMixin::State::Full;

    return state;
}

void RenderModeWidget::updateState()
{
    QLayout* stateLayout = nullptr;

    switch (_state)
    {
        case State::Popup:
            stateLayout = new QVBoxLayout();
            break;

        case State::Compact:
        case State::Full:
            stateLayout = new QHBoxLayout();
            break;

        default:
            break;
    }

    if (_state == State::Popup || _state == State::Full) {
        _scatterPlotPushButton->setFixedSize(BUTTON_SIZE_FULL);
        _densityPlotPushButton->setFixedSize(BUTTON_SIZE_FULL);
        _contourPlotPushButton->setFixedSize(BUTTON_SIZE_FULL);
        _scatterPlotPushButton->setText("Scatter");
        _densityPlotPushButton->setText("Density");
        _contourPlotPushButton->setText("Contour");
    }
    else {
        _scatterPlotPushButton->setFixedSize(BUTTON_SIZE_COMPACT);
        _densityPlotPushButton->setFixedSize(BUTTON_SIZE_COMPACT);
        _contourPlotPushButton->setFixedSize(BUTTON_SIZE_COMPACT);
        _scatterPlotPushButton->setText("S");
        _densityPlotPushButton->setText("D");
        _contourPlotPushButton->setText("C");
    }

    stateLayout->setMargin(0);
    stateLayout->addWidget(_scatterPlotPushButton);
    stateLayout->addWidget(_densityPlotPushButton);
    stateLayout->addWidget(_contourPlotPushButton);

    if (layout())
        delete layout();

    setLayout(stateLayout);
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