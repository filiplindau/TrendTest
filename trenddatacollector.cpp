#include "trenddatacollector.h"

TrendDataCollector::TrendDataCollector()
{
    this->mAttribute = nullptr;
    this->mAttributeName = "";
    this->mInterval = 200;
    connect(&this->mTimer, SIGNAL(timeout()), this, SLOT(updateCurve()));
//    this->mMutex = QMutex();
    this->mStopProcess = false;
    this->mPauseProcess = true;
}



TrendDataCollector::TrendDataCollector(QString attributeName)
{
    this->mAttribute = nullptr;
    this->mAttributeName = attributeName;


    this->mInterval = 200;
    connect(&this->mTimer, SIGNAL(timeout()), this, SLOT(updateCurve()));
//    this->mMutex = QMutex();
    this->mStopProcess = false;
    this->mPauseProcess = true;
}

TrendDataCollector::~TrendDataCollector()
{
    if (this->mAttribute != nullptr)
    {
        delete mAttribute;
    }
    this->mTimeData.clear();
    this->mTrendData.clear();
}

void TrendDataCollector::start()
{
    this->mTimer.start(mInterval);
}

void TrendDataCollector::stop()
{    
    QMutexLocker ml(&this->mMutex);
    this->mStopProcess = true;
    qDebug() << "Stop!";
}

void TrendDataCollector::pause()
{
    QMutexLocker ml(&this->mMutex);
    this->mPauseProcess = true;
    qDebug() << "Pause!";
}

void TrendDataCollector::unpause()
{
    QMutexLocker ml(&this->mMutex);
    this->mPauseProcess = false;
    qDebug() << "Unpause!";
}

void TrendDataCollector::process()
{
    qDebug() << "Starting processing...";
    this->setAttribute(this->mAttributeName);

    QMutexLocker ml(&this->mMutex);
    this->mStopProcess = false;
    QElapsedTimer timer;
    timer.start();
    this->updateCurve();
    while (this->mStopProcess == false)
    {
        ml.relock();
        if (this->mPauseProcess == false)
        {
            if (timer.hasExpired(this->mInterval))
            {
                ml.unlock();
                timer.restart();
                this->updateCurve();
            }
        }
        ml.unlock();
        QThread::msleep(10);

    }
    emit finished();
}

void TrendDataCollector::updateCurve()
{
    qDebug() << "Update of " << this->mAttributeName << " with ";
    Tango::DeviceAttribute attr;
    if (this->mAttribute != nullptr)
    {
        attr=this->mAttribute->read();
        Tango::TimeVal tv;
        tv = attr.get_date();
        double data, t;
        int ty = attr.get_type();
        if (ty == Tango::DEV_DOUBLE || ty == Tango::DEV_FLOAT)
        {
            attr >> data;
        }
        else if (ty == Tango::DEV_SHORT)
        {
            short d_tmp;
            attr >> d_tmp;
            data = (double)d_tmp;
        }
        else if (ty == Tango::DEV_USHORT)
        {
            unsigned short d_tmp;
            attr >> d_tmp;
            data = (double)d_tmp;
        }
        else if (ty == Tango::DEV_LONG)
        {
            int d_tmp;
            attr >> d_tmp;
            data = (double)d_tmp;
        }
        else if (ty == Tango::DEV_ULONG)
        {
            unsigned long d_tmp;
            attr >> d_tmp;
            data = (double)d_tmp;
        }
        else if (ty == Tango::DEV_UCHAR)
        {
            unsigned char d_tmp;
            attr >> d_tmp;
            data = (double)d_tmp;
        }
        else if (ty == Tango::DEV_LONG64)
        {
            long d_tmp;
            attr >> d_tmp;
            data = (double)d_tmp;
        }
        else if (ty == Tango::DEV_ULONG64)
        {
            unsigned long d_tmp;
            attr >> d_tmp;
            data = (double)d_tmp;
        }
        else
        {
            data = 0.0;
        }

        t = tv.tv_sec + 1e-6 * tv.tv_usec;
        this->mTrendData.append(data);
        this->mTimeData.append(t);
        qDebug() << " double data " << QString::number(data) << "\n";
        emit new_data(t, data);
    }

}


void TrendDataCollector::setAttribute(QString attributeName)
{
    if (this->mAttribute != nullptr)
    {
        delete this->mAttribute;
    }
    string attrName;
    attrName = attributeName.toStdString();
    try
    {
        this->mAttribute = new Tango::AttributeProxy(attrName);
        this->mAttributeName = attributeName;
    }
    catch (Tango::DevFailed &e)
    {
        Tango::Except::print_exception(e);
        this->mAttribute = nullptr;
        this->mAttributeName = QString("");
    }
}

QString TrendDataCollector::getAttributeName()
{
    return this->mAttributeName;
}
