#pragma once

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
     * @param core Pointer to the core
     * @param datasetName Name of the images dataset
     */
    InfoAction(QObject* parent, CoreInterface* core, const QString& datasetName);

protected:
    CoreInterface*          _core;                              /** Pointer to the core */
    DatasetRef<Images>      _images;                            /** Points dataset reference */
    StringAction            _typeAction;                        /** Image collection type action */
    StringAction            _numberOfImagesAction;              /** Number of images action */
    StringAction            _imageWidthAction;                  /** Image width action */
    StringAction            _imageHeightAction;                 /** Image height action */
    StringAction            _numberOfPixelsAction;              /** Number of pixels per image action */
    StringAction            _numberComponentsPerPixelAction;    /** Number of components action */
    //ImageFilePathsAction    _imageFilePathsAction;              /** Image file paths action */
    //DimensionNamesAction    _dimensionNamesAction;              /** Dimension names action */
};
