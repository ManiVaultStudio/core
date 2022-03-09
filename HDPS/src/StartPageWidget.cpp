#include "StartPageWidget.h"

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/, const Mode& mode /*= Mode::ProjectBar*/) :
    QWidget(parent),
    _mode(mode),
    _projectBarWidget(this),
    _logoWidget(this)
{
    _layout.setMargin(0);
    _layout.setSpacing(0);

    _layout.addWidget(&_projectBarWidget);
    _layout.addWidget(&_logoWidget);

    setMode(mode);
    setLayout(&_layout);
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
