// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorSchemesListModel.h"

using namespace mv;
using namespace mv::util;

#ifdef _DEBUG
    #define COLOR_SCHEMES_LIST_MODEL_VERBOSE
#endif

namespace mv {

ColorSchemesListModel::ColorSchemesListModel(QObject* parent /*= nullptr*/) :
    AbstractColorSchemesModel(parent)
{
}

void ColorSchemesListModel::addColorScheme(util::ColorScheme colorScheme)
{
    appendRow(Row(colorScheme));
}

void ColorSchemesListModel::removeColorScheme(const QString& colorSchemeName)
{
    const auto matches = match(index(0, static_cast<int>(Column::Name)), Qt::DisplayRole, colorSchemeName, 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (matches.isEmpty())
        return;

    removeRow(matches.first().row());

    _colorSchemes.erase(
        std::remove_if(_colorSchemes.begin(), _colorSchemes.end(), [colorSchemeName](const ColorScheme& colorScheme) {
            return colorScheme.getName() == colorSchemeName;
		}),
        _colorSchemes.end()
    );
}

void ColorSchemesListModel::addBuiltInColorSchemes()
{
#ifdef COLOR_SCHEMES_LIST_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto addBuiltInCustomColorScheme = [this](const QString& name, const QString& description, QColor window, QColor windowText, QColor base, QColor alternateBase, QColor tooltipBase, QColor tooltipText, QColor text, QColor button, QColor buttonText, QColor brightText) -> void {
    	auto palette = QPalette();

        palette.setColor(QPalette::Window, window);
        palette.setColor(QPalette::WindowText, windowText);
        palette.setColor(QPalette::Base, base);
        palette.setColor(QPalette::AlternateBase, alternateBase);
        palette.setColor(QPalette::ToolTipBase, tooltipBase);
        palette.setColor(QPalette::ToolTipText, tooltipText);
        palette.setColor(QPalette::Text, tooltipText);
        palette.setColor(QPalette::Button, button);
        palette.setColor(QPalette::ButtonText, buttonText);
        palette.setColor(QPalette::BrightText, brightText);

        addColorScheme(ColorScheme(ColorScheme::Mode::BuiltIn, name, description, palette));
	};

    addBuiltInCustomColorScheme("Dark", "Dark color scheme", QColor(45, 45, 45), Qt::white, QColor(30, 30, 30), QColor(45, 45, 45), Qt::white, Qt::white, Qt::white, QColor(60, 60, 60), Qt::white, Qt::red);
    addBuiltInCustomColorScheme("Light", "Light color scheme", Qt::white, Qt::black, Qt::white, QColor(240, 240, 240), Qt::white, Qt::black, Qt::black, QColor(220, 220, 220), Qt::black, Qt::red);
    addBuiltInCustomColorScheme("Fusion (dark)", "Fusion dark color scheme", QColor(45, 45, 45), Qt::white, QColor(30, 30, 30), QColor(45, 45, 45), Qt::white, Qt::white, Qt::white, QColor(60, 60, 60), Qt::white, Qt::red);
    addBuiltInCustomColorScheme("Fusion (light)", "Fusion light color scheme", Qt::white, Qt::black, Qt::white, QColor(240, 240, 240), Qt::white, Qt::black, Qt::black, QColor(220, 220, 220), Qt::black, Qt::red);
    addBuiltInCustomColorScheme("Google Material Design (dark)", "Google material design dark color scheme", QColor(33, 33, 33), QColor(220, 220, 220), QColor(50, 50, 50), QColor(40, 40, 40), QColor(50, 50, 50), QColor(220, 220, 220), QColor(220, 220, 220), QColor(50, 50, 50), QColor(220, 220, 220), QColor(220, 220, 220));
    addBuiltInCustomColorScheme("Google Material Design (light)", "Google material design light color scheme", QColor(245, 245, 245), QColor(0, 0, 0), QColor(255, 255, 255), QColor(240, 240, 240), QColor(255, 255, 255), QColor(0, 0, 0), QColor(0, 0, 0), QColor(230, 230, 230), QColor(0, 0, 0), QColor(0, 0, 0));
    addBuiltInCustomColorScheme("Dracula", "Dracula color scheme", QColor(40, 42, 54), QColor(248, 248, 242), QColor(68, 71, 90), QColor(50, 50, 70), QColor(68, 71, 90), QColor(248, 248, 242), QColor(248, 248, 242), QColor(68, 71, 90), QColor(248, 248, 242), QColor(248, 248, 242));
    addBuiltInCustomColorScheme("Nord (Cold Minimalistic)", "Cold color scheme", QColor(46, 52, 64), QColor(216, 222, 233), QColor(59, 66, 82), QColor(76, 86, 106), QColor(59, 66, 82), QColor(216, 222, 233), QColor(216, 222, 233), QColor(59, 66, 82), QColor(216, 222, 233), QColor(216, 222, 233));
    addBuiltInCustomColorScheme("Solarized (dark)", "Solarized dark color scheme", QColor(0, 43, 54), QColor(131, 148, 150), QColor(7, 54, 66), QColor(0, 43, 54), QColor(7, 54, 66), QColor(131, 148, 150), QColor(147, 161, 161), QColor(88, 110, 117), QColor(147, 161, 161), QColor(147, 161, 161));
    addBuiltInCustomColorScheme("Solarized (light)", "Solarized light color scheme", QColor(253, 246, 227), QColor(101, 123, 131), QColor(238, 232, 213), QColor(253, 246, 227), QColor(238, 232, 213), QColor(101, 123, 131), QColor(88, 110, 117), QColor(147, 161, 161), QColor(88, 110, 117), QColor(88, 110, 117));
    addBuiltInCustomColorScheme("Monokai", "Monokai scheme", QColor(39, 40, 34), QColor(248, 248, 242), QColor(50, 50, 50), QColor(60, 60, 60), QColor(50, 50, 50), QColor(248, 248, 242), QColor(248, 248, 242), QColor(70, 70, 70), QColor(248, 248, 242), QColor(248, 248, 242));
    addBuiltInCustomColorScheme("High contrast", "High-contrast color scheme", Qt::black, Qt::white, Qt::black, Qt::white, Qt::black, Qt::white, Qt::white, Qt::black, Qt::white, Qt::red);
}

}
