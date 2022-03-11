#pragma once

#include "PixelSelection.h"

#include <QWidget>
#include <QMap>
#include <QPen>
#include <QBrush>

class QPainter;

namespace hdps {

namespace util {

/**
 * Pixel selection tool class
 *
 * Class for pixel selection using various tools (e.g. brush, rectangle and lasso)
 * It handles mouse/keyboard events to switch between tools and perform the selection itself
 * It draws an overlay of the selection shape \p _shapePixmap and the selected area \p _areaPixmap
 *
 * @author Thomas Kroes
 */
class PixelSelectionTool : public QObject
{
    Q_OBJECT

public: // Construction/destruction

    /**
     * Constructor
     * @param targetWidget Target widget on which the pixel selection tool operates
     * @param enabled Whether the tool is enabled or not
     */
    PixelSelectionTool(QWidget* targetWidget, const bool& enabled = true);

public: // Getters/setters

    /** Get whether the tool is enabled or not */
    bool isEnabled() const;

    /**
     * Set whether the tool is enabled or not
     * @param enabled Whether the tool is enabled or not
     */
    void setEnabled(const bool& enabled);

    /** Get the current pixel selection type */
    PixelSelectionType getType() const;

    /**
     * Set the current pixel selection type
     * @param type Pixel selection type
     */
    void setType(const PixelSelectionType& type);

    /** Get the current pixel selection modifier */
    PixelSelectionModifierType getModifier() const;

    /**
     * Set the current pixel selection modifier
     * @param modifier Pixel selection modifier
     */
    void setModifier(const PixelSelectionModifierType& modifier);

    /** Get whether notifications should be fired continuously or only at the end of selection */
    bool isNotifyDuringSelection() const;

    /**
     * Set whether notifications should be fired continuously or only at the end of selection
     * @param notifyDuringSelection Whether notifications should be fired continuously or only at the end of selection
     */
    void setNotifyDuringSelection(const bool& notifyDuringSelection);

    /** Get brush radius */
    float getBrushRadius() const;

    /**
     * Set brush radius
     * @param brushRadius Brush radius
     */
    void setBrushRadius(const float& brushRadius);

    /** Get main drawing color */
    QColor getMainColor() const;

    /**
     * Set main drawing color
     * @param color Drawing color
     */
    void setMainColor(const QColor& mainColor);

    /** Trigger signals */
    void setChanged();

    /**
     * Get whether the selection process is cBurrently active
     * @return Boolean indicating whether the selection process is currently active
     */
    bool isActive() const {
        return _active;
    }

    /**
     * Get whether the selection process is aborted
     * @return Boolean indicating whether the selection process is aborted
     */
    bool isAborted() const {
        return _aborted;
    }

    /** Get the pixmap overlay that contains the selection tool visualization */
    QPixmap& getShapePixmap() {
        return _shapePixmap;
    }

    /** Get the pixmap overlay that contains the selected pixels*/
    QPixmap& getAreaPixmap() {
        return _areaPixmap;
    }

    /** Updates the pixel selection tool (wraps internal paint method) */
    void update();

public: // Event handling

    /**
     * Listens to the events of \p target
     * @param target Target object to watch for events
     * @param event Event that occurred
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Paints the selection tool pixmaps */
    void paint();

    /** Initiates the selection process */
    void startSelection();

    /** Ends the selection process */
    void endSelection();

signals:

    /** Signals that the type has changed */
    void typeChanged(const PixelSelectionType& type);

    /** Signals that the selection modifier has changed */
    void modifierChanged(const PixelSelectionModifierType& modifier);

    /** Signals that the notify during selection property has changed */
    void notifyDuringSelectionChanged(const bool& notifyDuringSelection);

    /** Signals that the brush radius has changed */
    void brushRadiusChanged(const float& brushRadius);

    /** Signals that the selection shape changed */
    void shapeChanged();

    /** Signals that the selection area changed */
    void areaChanged();

    /** Signals that the selection process has started */
    void started();

    /** Signals that the selection process has ended */
    void ended();

protected:
    bool                        _enabled;                   /** Whether the tool is enabled or not */
    PixelSelectionType          _type;                      /** Current selection type */
    PixelSelectionModifierType  _modifier;                  /** Current selection modifier */
    bool                        _active;                    /** Whether the selection process is active */
    bool                        _notifyDuringSelection;     /** Whether the selection is published continuously or at the end */
    float                       _brushRadius;               /** Brush radius */
    QPoint                      _mousePosition;             /** Current mouse position */
    QVector<QPoint>             _mousePositions;            /** Recorded mouse positions */
    int                         _mouseButtons;              /** State of the left, middle and right mouse buttons */
    QPixmap                     _shapePixmap;               /** Pixmap for the selection tool shape */
    QPixmap                     _areaPixmap;                /** Pixmap for the selection area */
    bool                        _preventContextMenu;        /** Whether to prevent a context menu */
    bool                        _aborted;                   /** Whether the selection process was aborted */

protected:
    QColor      _mainColor;                 /** Main drawing color */
    QColor      _fillColor;                 /** Selection area fill color (based on main color) */
    QBrush      _areaBrush;                 /** Selection area brush */
    QPen        _penLineForeGround;         /** Foreground pen */
    QPen        _penLineBackGround;         /** Background pen */
    QPen        _penControlPoint;           /** Control point pen */
    QPen        _penClosingPoint;           /** Closing point pen (e.g. for finishing polygon selection) */

public:
    static constexpr float BRUSH_RADIUS_MIN     = 10.0f;                    /** Minimum radius */
    static constexpr float BRUSH_RADIUS_MAX     = 500.0f;                   /** Maximum radius */
    static constexpr float BRUSH_RADIUS_DEFAULT = 50.0f;                    /** Default radius */
    static constexpr float BRUSH_RADIUS_DELTA   = 10.0f;                    /** Radius increment */
    static constexpr float CP_RADIUS_LINE       = 8.0f;                     /** Radius of line control points */
    static constexpr float CP_RADIUS_CLOSING    = 2.5f * CP_RADIUS_LINE;    /** Radius of the closing control point (e.g. for finishing polygon selection) */
};

}
}
