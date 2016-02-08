#include "qtrendlegendwidget.h"
#include <QHBoxLayout>

QTrendLegendWidget::QTrendLegendWidget(QWidget *parent) :
    QWidget(parent)
{
    this->mNameLabel.setText("");
    this->mRangeLabel.setText("(0.0, 1.0)");

    this->mGraph = nullptr;
    this->mAxis = nullptr;

    this->mMinValue = 0.0;
    this->mMaxValue = 1.0;

    this->mPen = QPen(QColor(200,0,0));
//    this->mPen.setWidthF(2.0);

    this->setupLayout();
}

QTrendLegendWidget::QTrendLegendWidget(QString name, QColor color, QWidget *parent) :
    QWidget(parent)
{
    this->setName(name);
    this->mRangeLabel.setText("(0.0, 1.0)");

    this->mGraph = nullptr;
    this->mAxis = nullptr;

    this->mMinValue = 0.0;
    this->mMaxValue = 1.0;

    this->mPen = QPen(color);
//    this->mPen.setWidthF(2.0);

    this->setupLayout();
}

QTrendLegendWidget::QTrendLegendWidget(QString name, QPen pen, QWidget *parent) :
    QWidget(parent)
{
    this->setName(name);
    this->mRangeLabel.setText("(0.0, 1.0)");

    this->mGraph = nullptr;
    this->mAxis = nullptr;

    this->mMinValue = 0.0;
    this->mMaxValue = 1.0;

    this->mPen = pen;
//    this->mPen.setWidthF(2.0);

    this->setupLayout();
}

void QTrendLegendWidget::setupLayout()
{
    this->mStartLabel.setText("");
    this->mStartLabel.setMinimumWidth(15);
    this->mStartLabel.setMaximumWidth(15);
    this->mStartLabel.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->mStartLabel.setAutoFillBackground(true);
    QString s = "QLabel { background-color: " + this->mPen.color().name() + "; }";
    this->mStartLabel.setStyleSheet(s);

    QHBoxLayout* layout = new QHBoxLayout;
    this->setLayout(layout);
    QVBoxLayout* layout2 = new QVBoxLayout;
    QHBoxLayout* layout3 = new QHBoxLayout;
    layout->addWidget(&this->mStartLabel);
    layout->addLayout(layout2);
    layout2->addWidget(&this->mNameLabel);
    layout2->addLayout(layout3);
    layout3->addWidget(&this->mValueLabel);
    QSpacerItem* spacer = new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout3->addSpacerItem(spacer);
    layout3->addWidget(&this->mRangeLabel);

}

void QTrendLegendWidget::setName(QString name)
{
    QString separator="/";
    QStringList qsl = name.split(separator);
    this->mShortName = qsl[qsl.length()-1];
    this->mLongName = name;
    this->mNameLabel.setText(this->mShortName);
    this->setToolTip(this->mLongName);
}

QString QTrendLegendWidget::getShortName()
{
    return this->mNameLabel.text();
}

QString QTrendLegendWidget::getLongName()
{
    return this->mLongName;
}

void QTrendLegendWidget::setRange(double minValue, double maxValue)
{
    this->mMinValue = minValue;
    this->mMaxValue = maxValue;

    QString s = "(" + QString::number(minValue) + ", " + QString::number(maxValue) + ")";
    this->mRangeLabel.setText(s);

    if (this->mAxis != nullptr)
    {
        QCPRange range(minValue, maxValue);
        this->mAxis->setRange(range);
    }
}

QCPRange QTrendLegendWidget::getRange()
{
    QCPRange range(this->mMinValue, this->mMaxValue);
    return range;
}

void QTrendLegendWidget::setAxis(QCPAxis *axis)
{
//    if (this->mAxis != nullptr)
//    {
//        delete this->mAxis;
//    }
    this->mAxis = axis;
    QCPRange range(this->mMinValue, this->mMaxValue);
    this->mAxis->setRange(range);
}

QCPAxis* QTrendLegendWidget::getAxis()
{
    return this->mAxis;
}

void QTrendLegendWidget::setGraph(QCPGraph *graph)
{
//    if (this->mGraph != nullptr)
//    {
//        delete this->mGraph;
//    }
    this->mGraph = graph;
    this->mGraph->setPen(this->mPen);
}

QCPGraph* QTrendLegendWidget::getGraph()
{
    return this->mGraph;
}

void QTrendLegendWidget::setPen(QPen pen)
{
    this->mPen = pen;
}

QPen QTrendLegendWidget::getPen()
{
    return this->mPen;
}

void QTrendLegendWidget::setValue(double value)
{
    this->mValueLabel.setText(QString::number(value));
}

void QTrendLegendWidget::setValue(int value)
{
    this->mValueLabel.setText(QString::number(value));
}

void QTrendLegendWidget::setValue(QString value)
{
    this->mValueLabel.setText(value);
}

void QTrendLegendWidget::updateData(double t, double data)
{
    qDebug() << "Updating " << this->mShortName;
    this->mGraph->addData(t, data);
    this->setValue(data);
}
