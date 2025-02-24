// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "InfoWidget.h"

#include <QDebug>
#include <QVBoxLayout>

#ifdef _DEBUG
    #define INFO_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

QColor InfoWidget::defaultForegroundColor = Qt::black;
QColor InfoWidget::defaultBackgroundColor = Qt::lightGray;

InfoWidget::InfoWidget(QWidget* parent) :
    QWidget(parent),
    _foregroundColor(defaultForegroundColor),
    _backgroundColor(defaultBackgroundColor),
    _foregroundColorRole(QPalette::ColorRole::Text),
    _backgroundColorRole(QPalette::ColorRole::Window)
{
    initialize();
}

InfoWidget::InfoWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description /*= ""*/) :
    InfoWidget(parent)
{
    initialize();

    set(icon, title, description);
}

void InfoWidget::set(const QIcon& icon, const QString& title, const QString& description /*= ""*/)
{
    _icon.fromQIcon(icon);

    _iconLabel.setPixmap(icon.pixmap(QSize(24, 24)));
    _titleLabel.setText(title);
    _descriptionLabel.setText(description);

    _titleLabel.setVisible(!_titleLabel.text().isEmpty());
    _descriptionLabel.setVisible(!_descriptionLabel.text().isEmpty());
}

QColor InfoWidget::getForegroundColor() const
{
    return _foregroundColor;
}

void InfoWidget::setForegroundColor(const QColor foregroundColor)
{
    if (foregroundColor == _foregroundColor)
        return;

    const auto previousForegroundColor = _foregroundColor;

    _foregroundColor = foregroundColor;

    emit foregroundColorChanged(previousForegroundColor, _foregroundColor);

    updateStyling();
}

QColor InfoWidget::getBackgroundColor() const
{
    return _backgroundColor;
}

void InfoWidget::setBackgroundColor(const QColor backgroundColor)
{
    if (backgroundColor == _backgroundColor)
        return;

    const auto previousBackgroundColor = _backgroundColor;

    _backgroundColor = backgroundColor;

    emit backgroundColorChanged(previousBackgroundColor, _backgroundColor);

    updateStyling();
}

void InfoWidget::setColors(const QColor& foregroundColor, const QColor& backgroundColor)
{
    setForegroundColor(foregroundColor);
    setBackgroundColor(backgroundColor);
}

std::int32_t InfoWidget::getForegroundColorRole() const
{
    return _foregroundColorRole;
}

void InfoWidget::setForegroundColorRole(const QPalette::ColorRole& foregroundColorRole)
{
    if (foregroundColorRole == _foregroundColorRole)
        return;

    const auto previousForegroundColorRole = _foregroundColorRole;

    _foregroundColorRole = foregroundColorRole;

    emit foregroundColorRoleChanged(static_cast<QPalette::ColorRole>(previousForegroundColorRole), static_cast<QPalette::ColorRole>(_foregroundColorRole));
}

std::int32_t InfoWidget::getBackgroundColorRole() const
{
    return _backgroundColorRole;
}

void InfoWidget::setBackgroundColorRole(const QPalette::ColorRole& backgroundColorRole)
{
    if (backgroundColorRole == _backgroundColorRole)
        return;

    const auto previousBackgroundColorRole = _backgroundColorRole;

    _backgroundColorRole = backgroundColorRole;

    emit backgroundColorRoleChanged(static_cast<QPalette::ColorRole>(previousBackgroundColorRole), static_cast<QPalette::ColorRole>(_backgroundColorRole));
}

void InfoWidget::setColorRoles(const QPalette::ColorRole& foregroundColorRole, const QPalette::ColorRole& backgroundColorRole)
{
    setForegroundColorRole(foregroundColorRole);
    setBackgroundColorRole(backgroundColorRole);
}

void InfoWidget::initialize()
{
    setObjectName("InfoWidget");

    auto layout = new QVBoxLayout();

    _iconLabel.setAlignment(Qt::AlignCenter);

    _titleLabel.setAlignment(Qt::AlignCenter);
    _titleLabel.setStyleSheet("font-weight: bold");

    _descriptionLabel.setAlignment(Qt::AlignCenter);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    layout->addStretch(1);
    layout->addWidget(&_iconLabel);
    layout->addWidget(&_titleLabel);
    layout->addWidget(&_descriptionLabel);
    layout->addStretch(1);

    setLayout(layout);

    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, &InfoWidget::updateStyling);
}

void InfoWidget::updateStyling()
{
    const auto foregroundColor = _foregroundColorRole > 0 ? qApp->palette().color(QPalette::ColorGroup::Normal, static_cast<QPalette::ColorRole>(_foregroundColorRole)) : _foregroundColor;
    const auto backgroundColor = _backgroundColorRole > 0 ? qApp->palette().color(QPalette::ColorGroup::Normal, static_cast<QPalette::ColorRole>(_backgroundColorRole)) : _backgroundColor;

    setStyleSheet(QString(
        "QWidget#InfoWidget"
        "{"
			"background-color: %1;"
        "}"
        "QWidget#InfoWidget > QLabel"
        "{"
			"color: %2;"
        "}").arg(backgroundColor.name(), foregroundColor.name()));

    _iconLabel.setPixmap(_icon.pixmap(QSize(24, 24)));
}

}
