// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageTutorialsWidget.h"
#include "LearningPageContentWidget.h"

#include <QDebug>
#include <QStyledItemDelegate>

using namespace mv::gui;

LearningPageTutorialsWidget::LearningPageTutorialsWidget(LearningPageContentWidget* learningPageContentWidget) :
    QWidget(learningPageContentWidget),
    _learningPageContentWidget(learningPageContentWidget),
    _mainLayout()
{
    _mainLayout.setSpacing(20);
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Tutorials", "Tutorials"));

    setLayout(&_mainLayout);

    connect(qApp, &QApplication::paletteChanged, this, &LearningPageTutorialsWidget::updateCustomStyle);

    updateCustomStyle();
}

void LearningPageTutorialsWidget::updateCustomStyle()
{
}
