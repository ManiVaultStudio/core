#include "RenderModeWidget.h"
#include "ScatterplotPlugin.h"
#include "widgets/PopupWidget.h"

#include "ui_RenderModeWidget.h"

#include <QPainter>
#include <QVBoxLayout>



const QSize RenderModeWidget::BUTTON_SIZE_COMPACT = QSize(22, 22);
const QSize RenderModeWidget::BUTTON_SIZE_FULL = QSize(60, 22);

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

    /*
    const auto setButtonsState = [this](const State& state) {
        switch (state)
        {
            case State::Compact:
                _ui->scatterPlotPushButton->setFixedSize(BUTTON_SIZE_COMPACT);
                _ui->densityPlotPushButton->setFixedSize(BUTTON_SIZE_COMPACT);
                _ui->contourPlotPushButton->setFixedSize(BUTTON_SIZE_COMPACT);
                _ui->scatterPlotPushButton->setText("S");
                _ui->densityPlotPushButton->setText("D");
                _ui->contourPlotPushButton->setText("C");
                break;

            case State::Popup:
            case State::Full:
                _ui->scatterPlotPushButton->setFixedSize(BUTTON_SIZE_FULL);
                _ui->densityPlotPushButton->setFixedSize(BUTTON_SIZE_FULL);
                _ui->contourPlotPushButton->setFixedSize(BUTTON_SIZE_FULL);
                _ui->scatterPlotPushButton->setText("Scatter");
                _ui->densityPlotPushButton->setText("Density");
                _ui->contourPlotPushButton->setText("Contour");
                break;

            default:
                break;
        }
    };

    

    _widgetEventProxy.initialize(&const_cast<ScatterplotPlugin&>(plugin), [this, setButtonsState](const QSize& size) {
        const auto width = size.width();
        auto state = State::Popup;
        
        if (width >= 800 && width < 1500)
            state = RenderModeWidget::State::Compact;
        
        if (width >= 1500)
            state = RenderModeWidget::State::Full;

        switch (state)
        {
            case State::Popup:
            {
                _ui->popupPushButton->setVisible(true);
                _ui->widget->setVisible(false);
                //layout()->removeWidget(_ui->widget);
                _ui->popupPushButton->setPopupWidget(new hdps::gui::PopupWidget(this, _ui->widget));
                setButtonsState(State::Popup);
                break;
            }

            case RenderModeWidget::State::Compact:
                _ui->popupPushButton->setVisible(false);
                _ui->widget->setVisible(true);
                setButtonsState(State::Compact);
                break;

            case RenderModeWidget::State::Full:
                _ui->popupPushButton->setVisible(false);
                _ui->widget->setVisible(true);
                setButtonsState(State::Full);
                break;

            default:
                break;
        }
    });
    */
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


