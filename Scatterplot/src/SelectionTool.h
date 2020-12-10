#pragma once

#include <memory>

#include <QWidget>
#include <QMap>
#include <QPen>

class ScatterplotPlugin;

class QPainter;

/**
 * Selection tool class
 *
 * @author Thomas Kroes
 */
class SelectionTool : public QObject
{
    Q_OBJECT

public:

    /** Types */
    enum class Type
    {
        Rectangle,		/** Select points within a rectangle */
        Circle,		    /** Select points within a circle */
        Brush,			/** A brush is used the paint the selection */
        Lasso,			/** A lasso tool is used to enclose points */
        Polygon		    /** Select points in the interior of a polygon */
    };

    /** Maps type name to type enum and vice versa */
    static QMap<QString, Type> const types;

    /** Get string representation of type enum */
    static QString getTypeName(const Type& type) {
        return types.key(type);
    }

    /** Get enum representation from type name */
    static Type getTypeEnum(const QString& typeName) {
        return types[typeName];
    }

    /** Modifiers */
    enum class Modifier
    {
        Replace,        /** Replace selection */
        Add,            /** Add to selection */
        Remove          /** Remove from selection */
    };

    /** Maps modifier name to modifier enum and vice versa */
    static QMap<QString, Modifier> const modifiers;

    /** Get string representation of modifier enum */
    static QString getModifierName(const Modifier& modifier) {
        return modifiers.key(modifier);
    }

    /** Get enum representation from modifier name */
    static Modifier getModifierEnum(const QString& modifierName) {
        return modifiers[modifierName];
    }

public: // Construction/destruction

    SelectionTool(ScatterplotPlugin* scatterplotPlugin);

public: // Getters/setters

    Type getType() const;
    void setType(const Type& type);

    Modifier getModifier() const;
    void setModifier(const Modifier& modifier);

    float getRadius() const;
    void setRadius(const float& radius);

    bool canSelect() const;

    void finish();

    void setChanged();

    
    void selectAll();
    void clearSelection();
    void invertSelection();

    bool canSelectAll() const;
    bool canClearSelection() const;
    bool canInvertSelection() const;

public: // Event handling

    bool eventFilter(QObject* target, QEvent* event) override;

    void paint(QPainter* painter);

signals:

    void typeChanged(const Type& type);

    void modifierChanged(const Modifier& modifier);

    void radiusChanged(const float& radius);

protected:
    ScatterplotPlugin*  _scatterplotPlugin;     /** Scatter plot plugin */
    Type                _type;                  /** Current selection type */
    Modifier            _modifier;              /** Selection modifier */
    float               _radius;                /** Brush/circle radius */
    QVector<QPoint>     _mousePositions;        /** Recorded mouse positions */
    int                 _mouseButtons;          /** State of the left, middle and right mouse buttons */

public:

    // Brush/circle radius 
    static constexpr float RADIUS_MIN       = 10.0f;        /** Minimum radius */
    static constexpr float RADIUS_MAX       = 1000.0f;      /** Maximum radius */
    static constexpr float RADIUS_DEFAULT   = 10.0f;        /** Default radius */
    static constexpr float RADIUS_DELTA     = 10.0f;        /** Radius increment */

    // Drawing constants
    static const QColor COLOR_MAIN;
    static const QColor COLOR_FILL;
    static const QPen PEN_FOREGROUND;
    static const QPen PEN_BACKGROUND;
    static const QPen PEN_CONTROL_POINT;
};