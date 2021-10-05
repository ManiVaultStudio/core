#pragma once

#include "ImagePreviewAction.h"

#include "actions/Actions.h"
#include "event/EventListener.h"
#include "util/DatasetRef.h"

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
class InfoAction : public GroupAction, public hdps::EventListener
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param datasetName Name of the images dataset
     */
    InfoAction(QObject* parent, const QString& datasetName);

protected:
    DatasetRef<Images>      _images;                            /** Points dataset reference */
    StringAction            _typeAction;                        /** Image collection type action */
    StringAction            _numberOfImagesAction;              /** Number of images action */
    StringAction            _imageResolutionAction;             /** Image resolution action */
    StringAction            _sourceRectangleAction;             /** Source rectangle action */
    StringAction            _targetRectangleAction;             /** Target rectangle action */
    StringAction            _numberOfPixelsAction;              /** Number of pixels per image action */
    StringAction            _numberComponentsPerPixelAction;    /** Number of components action */
    ImagePreviewAction      _imagePreviewAction;                /** Image preview action */
};
