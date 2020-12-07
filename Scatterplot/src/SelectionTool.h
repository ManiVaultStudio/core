#pragma once

#include <memory>

#include <QWidget>
#include <QMap>

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
        None = -1,		/** Not set */
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
        Remove,         /** Remove from selection */
        All,            /** Select all */
        None,           /** Deselect all points */
        Invert          /** Invert point selection */
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

    SelectionTool(QObject* parent);

public: // Getters/setters

    Type getType() const;
    void setType(const Type& type);

    Modifier getModifier() const;
    void setModifier(const Modifier& modifier);

    float getRadius() const;
    void setRadius(const float& radius);

public: // Event handling

    bool eventFilter(QObject* target, QEvent* event) override;

signals:

    void typeChanged(const Type& type);

    void modifierChanged(const Modifier& modifier);

    void radiusChanged(const float& radius);

protected:
    Type                _type;              /** Current selection type */
    Modifier            _modifier;          /** Selection modifier */
    float               _radius;            /** Brush/circle radius */
    QVector<QPoint>     _mousePositions;    /** Recorded mouse positions */
    int                 _mouseButtons;      /** State of the left, middle and right mouse buttons */

    static constexpr float MIN_RADIUS       = 1.0f;         /** Minimum brush radius */
    static constexpr float MAX_RADIUS       = 1000.0f;      /** Maximum brush radius */
    static constexpr float DEFAULT_RADIUS   = 10.0f;        /** Default brush radius */

};