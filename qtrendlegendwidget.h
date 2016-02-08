#ifndef QTRENDLEGENDWIDGET_H
#define QTRENDLEGENDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPen>
#include <QBrush>
#include <QVector>
#include <qcustomplot.h>

class QTrendLegendWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QTrendLegendWidget(QWidget *parent = 0);
    explicit QTrendLegendWidget(QString name, QColor color, QWidget *parent = 0);
    explicit QTrendLegendWidget(QString name, QPen pen, QWidget *parent = 0);

    void setName(QString name);
    QString getShortName();
    QString getLongName();
    void setRange(double minValue, double maxValue);
    QCPRange getRange();
    void setAxis(QCPAxis* axis);
    QCPAxis* getAxis();
    void setPen(QPen pen);
    QPen getPen();
    void setGraph(QCPGraph* graph);
    QCPGraph* getGraph();
    void setValue(double value);
    void setValue(int value);
    void setValue(QString value);


private:
    QString mShortName;
    QString mLongName;
    QLabel mNameLabel;
    QLabel mRangeLabel;
    QLabel mValueLabel;
    QLabel mStartLabel;
    QLabel mEndLabel;
    QPen mPen;
    QBrush mBrush;
    QCPAxis* mAxis;         // There two items are part of the QCustomplotWidget and do not need to be deleted
    QCPGraph* mGraph;       // There two items are part of the QCustomplotWidget and do not need to be deleted

    double mMinValue;
    double mMaxValue;

    void setupLayout();

signals:

public slots:
    void updateData(double t, double data);
};

#endif // QTRENDLEGENDWIDGET_H
