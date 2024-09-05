// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageTutorialsWidget.h"

#include "LearningPageContentWidget.h"
#include "PageContentWidget.h"

#include <QApplication>
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

    updateCustomStyle();
}

bool LearningPageTutorialsWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange)
        updateCustomStyle();

    return QWidget::event(event);
}

void LearningPageTutorialsWidget::updateCustomStyle()
{
}
