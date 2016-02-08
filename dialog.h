#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "trenddatacollector.h"
#include "qtrendlegendwidget.h"
#include "attributeselectform.h"
#include <QList>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;
    QList<TrendDataCollector*> trendDataCollectorList;
    QList<QThread*> threadList;
    QList<QCPGraph*> graphList;
    QList<QCPAxis*> axisList;
    QList<QTrendLegendWidget*> trendLegendList;
    QTimer updateTimer;

    QList<QColor> mLightColorList;
    QList<QColor> mDarkColorList;
    int mCurrentColorListIndex;

private slots:
    void addTrend(QString attrName);
    void removeTrend(QString attrName);
    void selectAttribute();
    void updateTrends();
    void updateTrendData();
    void stopTrends();
    void startTrends();
    void test();
};

#endif // DIALOG_H
