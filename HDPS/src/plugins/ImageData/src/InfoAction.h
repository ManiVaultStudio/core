#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"

#include "Images.h"

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

/**
 * Info action class
 *
 * Action class for displaying basic images info
 *
 * @author Thomas Kroes
 */
class InfoAction : public GroupAction
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
    Dataset<Images>         _images;                            /** Points dataset reference */
    StringAction            _typeAction;                        /** Image collection type action */
    StringAction            _numberOfImagesAction;              /** Number of images action */
    StringAction            _imageResolutionAction;             /** Image resolution action */
    StringAction            _numberOfPixelsAction;              /** Number of pixels per image action */
    StringAction            _numberComponentsPerPixelAction;    /** Number of components action */
    hdps::EventListener     _eventListener;                     /** Listen to HDPS events */
};
