#include "RenderModeWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_RenderModeWidget.h"

#include <QPainter>

RenderModeWidget::RenderModeWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _ui{ std::make_unique<Ui::RenderModeWidget>() },
    _iconSize(16, 16)
{
    _ui->setupUi(this);
}

void RenderModeWidget::initialize(const ScatterplotPlugin& plugin)
{
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    /*
    _ui->scatterPlotPushButton->setIcon(getIcon(ScatterplotWidget::RenderMode::SCATTERPLOT));
    _ui->densityPlotPushButton->setIcon(getIcon(ScatterplotWidget::RenderMode::DENSITY));
    _ui->contourPlotPushButton->setIcon(getIcon(ScatterplotWidget::RenderMode::LANDSCAPE));
    */

    const auto updateToggles = [this, scatterPlotWidget]() {
        const auto renderMode = scatterPlotWidget->getRenderMode();

        _ui->scatterPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _ui->densityPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::DENSITY);
        _ui->contourPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::LANDSCAPE);
    };

    QObject::connect(_ui->scatterPlotPushButton, &QPushButton::clicked, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
        updateToggles();
    });

    QObject::connect(_ui->densityPlotPushButton, &QPushButton::clicked, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::DENSITY);
        updateToggles();
    });

    QObject::connect(_ui->contourPlotPushButton, &QPushButton::clicked, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE);
        updateToggles();
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updateToggles](const ScatterplotWidget::RenderMode& renderMode) {
        updateToggles();
    });

    updateToggles();
}

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