#include "dialog.h"
#include "ui_dialog.h"

#ifdef QT_NO_DEBUG_OUTPUT
#undef QT_NO_DEBUG_OUTPUT
#endif

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
//    connect(ui->buttonAdd, SIGNAL(clicked()), this, SLOT(addTrend()));
    connect(ui->buttonAdd, SIGNAL(clicked()), this, SLOT(selectAttribute()));
    connect(ui->buttonStop, SIGNAL(clicked()), this, SLOT(stopTrends()));
    connect(ui->buttonStart, SIGNAL(clicked()), this, SLOT(startTrends()));
    connect(ui->buttonTest, SIGNAL(clicked()), this, SLOT(test()));

    connect(&this->updateTimer, SIGNAL(timeout()), this, SLOT(updateTrends()));

    this->mLightColorList.append(QColor(240,0,0));
    this->mLightColorList.append(QColor(0,0,200));
    this->mLightColorList.append(QColor(20,120,20));
    this->mLightColorList.append(QColor(150,120,30));
    this->mLightColorList.append(QColor(130,10,140));
    this->mLightColorList.append(QColor(30,90,120));
    this->mLightColorList.append(QColor(0,0,0));
    this->mLightColorList.append(QColor(135,70,20));
    this->mLightColorList.append(QColor(80,80,80));
    this->mLightColorList.append(QColor(25,90,60));

    this->mCurrentColorListIndex = 0;

}

Dialog::~Dialog()
{
    QMutableListIterator<TrendDataCollector*> i(this->trendDataCollectorList);
    TrendDataCollector* trendDataCollector;
    while (i.hasNext())
    {
        trendDataCollector = i.next();
        trendDataCollector->stop();
        //delete curve;     // The object will be deleted by the deletelater method
    }
    delete ui;
}

void Dialog::selectAttribute()
{
    QList<QString> currentAttributeList;
    int i;
    for (i=0; i < this->trendDataCollectorList.count(); i++)
    {
        currentAttributeList.append(this->trendDataCollectorList[i]->getAttributeName());
    }
    QDialog selDlg;
    AttributeSelectForm* selWidget = new AttributeSelectForm(&selDlg, currentAttributeList, "");
    selDlg.setWindowModality(Qt::WindowModal);
    QBoxLayout dlgLayout(QBoxLayout::LeftToRight);
    dlgLayout.addWidget(selWidget);
    dlgLayout.setMargin(0);    
    selDlg.setLayout(&dlgLayout);
    if (selDlg.exec() == QDialog::Accepted)
    {
        QList<QString> attrList = selWidget->getAttributes();
        for (i=0; i < attrList.count(); i++)
        {
            qDebug() << attrList[i] << "\n";
            if (!currentAttributeList.contains(attrList[i]))
            {
                this->addTrend(attrList[i]);
            }
        }
        for (i=0; i < currentAttributeList.count(); i++)
        {
            if (!attrList.contains(currentAttributeList[i]))
            {
                this->removeTrend(currentAttributeList[i]);
            }
        }
    }
    else
    {
        qDebug() << "Not accepted\n";
    }
}

void Dialog::addTrend(QString attrName)
{

    QString s;
    s = QString("Adding ");
//    attrName = QString("gunlaser/devices/redpitayaevents2/measurementdata2");
    s.append(attrName);
    this->ui->infoLabel->setText(s);

    cout << "In Dialog::addTrend...\n";
    QCPAxis* ax = this->ui->plotWidget->axisRect()->addAxis(QCPAxis::AxisType::atLeft);
    ui->plotWidget->yAxis->setVisible(false);
    ax->setVisible(false);

    QCPGraph* graph = this->ui->plotWidget->addGraph(0, ax);

    QColor attrColor = this->mLightColorList[this->mCurrentColorListIndex];
    this->mCurrentColorListIndex++;
    if (this->mCurrentColorListIndex > this->mLightColorList.count())
    {
        this->mCurrentColorListIndex=0;
    }

    QTrendLegendWidget* leg = new QTrendLegendWidget(attrName, attrColor, this);
//    leg->setName(attrName);
    leg->setAxis(ax);
    leg->setGraph(graph);
    this->ui->verticalLayout_2->insertWidget(this->ui->verticalLayout_2->count()-1, leg);
//    this->ui->verticalLayout_2->addWidget(leg);
    this->trendLegendList.append(leg);

//    this->graphList.append(graph);
//    this->axisList.append(ax);
//    TrendCurve newCurve(attrName, graph, ax);

//    TrendCurve newCurve();
//    TrendCurve* newCurve = new TrendCurve(attrName, this->ui->plotWidget);
//    delete newCurve;
    try
    {
        TrendDataCollector* newCurve = new TrendDataCollector(attrName);
        QThread* thread = new QThread;
        newCurve->moveToThread(thread);
        connect(newCurve, SIGNAL(new_data(double,double)), leg, SLOT(updateData(double, double)));
        connect(thread, SIGNAL(started()), newCurve, SLOT(process()));

        connect(newCurve, SIGNAL(finished()), thread, SLOT(quit()));
        connect(newCurve, SIGNAL(finished()), newCurve, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        this->trendDataCollectorList.append(newCurve);
        this->threadList.append(thread);
        thread->start();
    }
    catch (...)
    {
        cout << "Error creating trend curve";
    };
}

void Dialog::removeTrend(QString attrName)
{
    QList<QString> currentAttributeList;
    int index;
    QTrendLegendWidget* tlw;
    qDebug() << "Removing trend " << attrName << "\n";
    for (int i=0; i < this->trendDataCollectorList.count(); i++)
    {
        if (this->trendDataCollectorList[i]->getAttributeName() == attrName)
        {
            qDebug() << "Stopping trendDataCollector\n";
            this->trendDataCollectorList[i]->stop();
            this->trendDataCollectorList.removeAt(i);
        }
    }
    for (int i=0; i < this->trendLegendList.count(); i++)
    {
        if (this->trendLegendList[i]->getLongName() == attrName)
        {
            qDebug() << "Removing widget\n";
            index = this->ui->verticalLayout_2->indexOf(this->trendLegendList.at(i));
            this->ui->verticalLayout_2->takeAt(index);
            tlw = this->trendLegendList.takeAt(i);
            delete tlw;
        }
    }

}

void Dialog::updateTrends()
{
    qDebug() << "updating\n";
    this->ui->plotWidget->rescaleAxes();
    this->ui->plotWidget->replot();
}

void Dialog::stopTrends()
{
    this->updateTimer.stop();
    qDebug() << "Stopping threads...\n";
    QMutableListIterator<TrendDataCollector*> i(this->trendDataCollectorList);
    TrendDataCollector* curve;
    while (i.hasNext())
    {
        curve = i.next();
        qDebug() << "Found " << QString::number((uintptr_t)curve) << "\n";
        curve->pause();
    }

}

void Dialog::startTrends()
{
    qDebug() << "Starting threads...\n";
    QMutableListIterator<TrendDataCollector*> i(this->trendDataCollectorList);
    while (i.hasNext())
    {
        i.next()->unpause();
    }
    this->updateTimer.start(500);
}

void Dialog::updateTrendData()
{
//    qDebug() << "New data:" << QString::number(data) << "\n";
//    graph->addData(t, data);
//    ui->plotWidget->graph(0)->addData(t,data);
//    ui->plotWidget->rescaleAxes();
    ui->plotWidget->replot();
}

void Dialog::test()
{
    qDebug() << "Test!!\n";
}
