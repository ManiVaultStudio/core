// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/StringAction.h"
#include "event/EventListener.h"

#include "Images.h"

/**
 * Info action class
 *
 * Action class for displaying basic images info
 *
 * @author Thomas Kroes
 */
class InfoAction : public mv::gui::GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param images Reference to images dataset
     */
    InfoAction(QObject* parent, Images& images);

protected:
    mv::Dataset<Images>              _images;                            /** Points dataset reference */
    mv::gui::StringAction            _typeAction;                        /** Image collection type action */
    mv::gui::StringAction            _numberOfImagesAction;              /** Number of images action */
    mv::gui::StringAction            _imageResolutionAction;             /** Image resolution action */
    mv::gui::StringAction            _numberOfPixelsAction;              /** Number of pixels per image action */
    mv::gui::StringAction            _numberComponentsPerPixelAction;    /** Number of components action */
    mv::EventListener                _eventListener;                     /** Listen to ManiniVault events */
};
