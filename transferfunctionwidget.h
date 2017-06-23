#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include "inc/painting/arthurwidgets.h"
#include <QPointer>

class HoverPoints;

/**
 * @brief The ShadeWidget class
 */
class ShadeWidget : public QWidget
{
    Q_OBJECT

public:
    enum ShadeType {
        RedShade,
        GreenShade,
        BlueShade,
        ARGBShade
    };

    ShadeWidget(ShadeType type, QWidget *parent);

    void setGradientStops(const QGradientStops &stops);

    void paintEvent(QPaintEvent *e) override;

    QSize sizeHint() const override { return QSize(150, 40); }
    QPolygonF points() const;

    HoverPoints *hoverPoints() const { return _pHoverPoints; }

    uint colorAt(int x);

signals:
    void colorsChanged();

private:
    void generateShade();

    ShadeType _pShadeType;
    QImage _pShade;
    HoverPoints *_pHoverPoints;
    QLinearGradient _pAlphaGradient;
};


/**
 * @brief The TransferFunctionEditor class
 */
class TransferFunctionEditor : public QWidget
{
    Q_OBJECT
public:
    explicit TransferFunctionEditor(QWidget *parent = 0);

    void setGradientStops(const QGradientStops &stops);

    const QGradientStops getGradientStops() const;

    void resetPoints();

    void setInterpolation(QString method);

public slots:
    void pointsUpdated();

signals:
    void gradientStopsChanged(const QGradientStops &stops);

private:
    ShadeWidget *_pRedShade;
    ShadeWidget *_pGreenShade;
    ShadeWidget *_pBlueShade;
    ShadeWidget *_pAlphaShade;

    QVector<ShadeWidget *> _shades;
    QGradientStops _stops;
};


/**
 * @brief The TransferFunctionWidget class
 */
class TransferFunctionWidget : public QWidget
{
    Q_OBJECT

public:
    TransferFunctionWidget(QWidget *parent);
    ~TransferFunctionWidget(){}

    QPointer<TransferFunctionEditor> getEditor();

public slots:
    void resetTransferFunction();

    void setInterpolation(QString method);

private:
    QPointer<TransferFunctionEditor> _pEditor;
};

#endif // TRANSFERFUNCTIONWIDGET_H