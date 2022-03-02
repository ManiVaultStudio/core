#include "StartPageWidget.h"

#include <Application.h>

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/, const Mode& mode /*= Mode::ProjectBar*/) :
    QWidget(parent),
    _mode(mode),
    _projectBarWidget(this),
    _logoWidget(this)
{
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _layout.setMargin(0);
    _layout.setSpacing(0);

    _layout.addWidget(&_projectBarWidget);
    _layout.addWidget(&_logoWidget);

    setMode(mode);

    setLayout(&_layout);

    // Create palette and linear gradient for background
    auto palette    = QPalette();
    auto gradient   = QLinearGradient(0, 0, 0, 400);

    // Gradient gray scale at top and bottom
    const auto topGrayLevel     = 170;
    const auto bottomGrayLevel  = 200;

    // Set gradient stops
    gradient.setColorAt(0.0, QColor(topGrayLevel, topGrayLevel, topGrayLevel));
    gradient.setColorAt(1.0, QColor(bottomGrayLevel, bottomGrayLevel, bottomGrayLevel));

    // Set palette brush to linear gradient
    palette.setBrush(QPalette::Window, QBrush(gradient));

    // Apply the palette to the widget
    setPalette(palette);
}

void StartPageWidget::setMode(const Mode& mode)
{
    _mode = mode;

    switch (_mode)
    {
        case Mode::ProjectBar:
            _projectBarWidget.setVisible(true);
            _logoWidget.setVisible(false);
            break;

        case Mode::LogoOnly:
            _projectBarWidget.setVisible(false);
            _logoWidget.setVisible(true);
            break;

        default:
            break;
    }
}

StartPageWidget::Mode StartPageWidget::getMode() const
{
    return _mode;
}
