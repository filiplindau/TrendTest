#ifndef TRENDCURVE_H
#define TRENDCURVE_H

#include <QObject>
#include <tango.h>
#include <QTimer>
#include <QVector>
#include <qcustomplot.h>
#include <QMutex>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QThread>

class TrendDataCollector : public QObject
{
    Q_OBJECT
public:
    TrendDataCollector();
    TrendDataCollector(QString attributeName);
    ~TrendDataCollector();
    void start();    
    void setAttribute(QString attributeName);
    QString getAttributeName();

private:
    // Better to use pointers? How will the garbage collection work?
    Tango::AttributeProxy *  mAttribute;
    QString mAttributeName;
    double  mInterval;
    QTimer  mTimer;
    QVector<double> mTrendData;
    QVector<double> mTimeData;
    QMutex mMutex;
    bool mStopProcess;
    bool mPauseProcess;


private slots:
    void updateCurve();

public slots:
    void stop();
    void pause();
    void unpause();
    void process();

signals:
    void new_data(double, double);
    void finished();

};

#endif // TRENDCURVE_H
