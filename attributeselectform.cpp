#include "attributeselectform.h"
#include "ui_attributeselectform.h"
#include <QDebug>
#include <QMutexLocker>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

AttributeSelectForm::AttributeSelectForm(QWidget *parent, QList<QString> currentAttributeList, QString tangoDatabaseName) :
    QWidget(parent),
    ui(new Ui::AttributeSelectForm)
{    
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->mTangoDatabaseName = tangoDatabaseName;
    this->mTangoDatabase = new Tango::Database();
    //this->populateDeviceList();
    this->readDatabaseDeviceList();

    this->ui->listAttributes->addItems(currentAttributeList);

    for (int i=0; i < currentAttributeList.count(); i++)
    {

    }

    connect(this->ui->buttonDone, SIGNAL(clicked()), this, SLOT(close()));
    connect(this->ui->buttonDone, SIGNAL(clicked()), this->parent(), SLOT(accept()));
//    connect(this->ui->buttonDone, SIGNAL(clicked()), this, SLOT(closePanel()));
    connect(this->ui->buttonFilter, SIGNAL(clicked()), this, SLOT(filterDeviceList()));
    connect(this->ui->buttonAddAttribute, SIGNAL(clicked()), this, SLOT(addAttribute()));
    connect(this->ui->lineeditFilter, SIGNAL(editingFinished()), this, SLOT(filterDeviceList()));
    connect(this->ui->treeDevices, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(populateAttributeList(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(this->ui->treeAttributes, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(addAttribute()));
//    connect(this->ui->treeAttributes, SIGNAL(pressed(QModelIndex)), this, SLOT(addAttribute()));
    this->mDeleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this->ui->listAttributes);
    this->mDeleteShortcut->setContext(Qt::WidgetShortcut);
    connect(this->mDeleteShortcut, SIGNAL(activated()), this, SLOT(deleteSelection()));
    connect(this->ui->listAttributes, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectDeviceFromAttributeList(QModelIndex)));
}

AttributeSelectForm::~AttributeSelectForm()
{
    delete ui;
    delete this->mTangoDatabase;
}

void AttributeSelectForm::readDatabaseDeviceList()
{
    this->mFutureFullDeviceList = QtConcurrent::run(this, &AttributeSelectForm::queryDeviceListDatabase, this->mTangoDatabase);
    this->mFutureFullDeviceList.waitForFinished();
    this->mFullDeviceList = this->mFutureFullDeviceList.result();
    this->filterDeviceList();

//    moveToThread(&this->databaseThread);
//    connect(&this->databaseThread, SIGNAL(started()), this, SLOT(populateDeviceList()));
//    connect(&this->databaseThread, SIGNAL(finished()), &this->databaseThread, SLOT(deleteLater()));
//    this->databaseThread.start();
}

QStringList AttributeSelectForm::queryDeviceListDatabase(Tango::Database* tangoDatabase)
{
    QStringList deviceList;
    vector<string> fullDeviceList;
    string s = "*";
    try
    {
        Tango::DbDatum dbDatum = tangoDatabase->get_device_exported(s);
        Tango::DbDatum dbDatumDev;
        fullDeviceList = dbDatum.value_string;
    }
    catch (Tango::DevFailed df)
    {
        Tango::DevError er;
        for (int i=0; i< df.errors.length(); i++)
        {
            er = df.errors[i];
            qDebug() << er.origin;
            cout << "In " << er.origin << ": " << er.desc << "\n";

        }
        return deviceList;
    }

    for (int i = 0; i < fullDeviceList.size(); i++)
    {
        deviceList.append(QString::fromStdString(fullDeviceList[i]));
    }
    return deviceList;
}



void AttributeSelectForm::populateDeviceList()
{
    string s = "*";
    try
    {
        Tango::DbDatum dbDatum = this->mTangoDatabase->get_device_exported(s);
        Tango::DbDatum dbDatumDev;
//        this->mFullDeviceList = dbDatum.value_string;
        this->filterDeviceList();
    }
    catch (Tango::DevFailed df)
    {
        Tango::DevError er;
        for (int i=0; i< df.errors.length(); i++)
        {
            er = df.errors[i];
            qDebug() << er.origin;
            cout << "In " << er.origin << ": " << er.desc << "\n";

        }
        return;
    }


}

//void AttributeSelectForm::filterDeviceList()
//{
//    // Find entries that contain the string in lineeditFilter:
//    this->mFilteredDeviceList.clear();
//    std::string filterStr = this->ui->lineeditFilter->text().toStdString();

//    qDebug() << "Filter string: " << QString::fromStdString(filterStr) << "\n";

//    if (this->mFutureFullDeviceList.isResultReadyAt(0) == true)
//    {
//        for (int k=0; k < this->mFullDeviceList.size(); k++)
//        {
//            std::string s = this->mFullDeviceList[k];
//            if (s.find(filterStr) != string::npos)
//            {
//                qDebug() << "Found " << QString::fromStdString(s) << "\n";
//                this->mFilteredDeviceList.push_back(s);
//            }
//            else
//            {
//                qDebug() << QString::fromStdString(filterStr) << " not in " << QString::fromStdString(s) << "\n";
//            }
//        }

//        // Clear old tree and generate new:
//        this->blockSignals(true);       // Need to block signal to prevent selection changed signals to populateAttributeList
//        QList<QTreeWidgetItem*> selItems = this->ui->treeDevices->selectedItems();
//        this->ui->treeDevices->clearSelection();
//        this->ui->treeDevices->clear();
//        QString devName, pt1, pt2, pt3;
//        QString separator="/";
//        QStringList qsl;
//        QTreeWidgetItem* treeItem1;
//        QTreeWidgetItem* treeItem2;
//        QTreeWidgetItem* treeItem3;
//        QList<QTreeWidgetItem*> foundItems;
//        // Go through list of found devices and build tree widget:
//        for (int i=0; i < this->mFilteredDeviceList.size(); i++)
//        {
//            devName = QString::fromStdString(this->mFilteredDeviceList[i]);

//            qsl = devName.split(separator);
//            if (qsl.count() == 3)
//            {
//                pt1 = qsl[0];
//                pt2 = qsl[1];
//                pt3 = qsl[2];

//                foundItems = this->ui->treeDevices->findItems(pt1, Qt::MatchExactly, 0);
//                if (foundItems.length() == 0)
//                {
//                    treeItem1 = new QTreeWidgetItem(this->ui->treeDevices);
//                    treeItem1->setText(0, pt1);
//                }
//                else
//                {
//                    treeItem1 = foundItems[0];
//                }
//                foundItems.clear();

//                // Search second layer of the tree for string pt2
//                int k = 0;
//                while (k < treeItem1->childCount())
//                {
//                    if (treeItem1->child(k)->text(0) == pt2)
//                    {
//                        // Found it. Use this to insert new device
//                        treeItem2 = treeItem1->child(k);
//                        break;
//                    }
//                    k++;
//                }
//                if (k == treeItem1->childCount())
//                {
//                    // If the count variable is childCount we didn't find the string, insert new leaf
//                    treeItem2 = new QTreeWidgetItem();
//                    treeItem2->setText(0, pt2);
//                    treeItem1->addChild(treeItem2);
//                }

//                // The last part should always be new
//                treeItem3 = new QTreeWidgetItem();
//                treeItem3->setText(0, pt3);
//                treeItem2->addChild(treeItem3);
//            }

//        }

//        // Expand all items if we have fewer than 4 top levels:
//        if (this->ui->treeDevices->topLevelItemCount() < 4)
//        {
//            for (int topI = 0; topI < this->ui->treeDevices->topLevelItemCount(); topI++)
//            {
//                this->ui->treeDevices->topLevelItem(topI)->setExpanded(true);
//                for (int chI=0; chI < this->ui->treeDevices->topLevelItem(topI)->childCount(); chI++)
//                {
//                    this->ui->treeDevices->topLevelItem(topI)->child(chI)->setExpanded(true);
//                }
//            }
//        }
//        this->blockSignals(false);
//    } // \isResultReadyAt
//}

void AttributeSelectForm::filterDeviceList()
{
    // Find entries that contain the string in lineeditFilter:
    this->mFilteredDeviceList.clear();
    QString filterStr = this->ui->lineeditFilter->text();

    qDebug() << "Filter string: " << filterStr << "\n";

    if (this->mFutureFullDeviceList.isResultReadyAt(0) == true)
    {
        for (int k=0; k < this->mFullDeviceList.size(); k++)
        {
            QString s = this->mFullDeviceList[k];
            if (s.contains(filterStr) == true)
            {
                qDebug() << "Found " << s << "\n";
                this->mFilteredDeviceList.push_back(s);
            }
            else
            {
                qDebug() << filterStr << " not in " << s << "\n";
            }
        }

        // Clear old tree and generate new:
        this->blockSignals(true);       // Need to block signal to prevent selection changed signals to populateAttributeList
        QList<QTreeWidgetItem*> selItems = this->ui->treeDevices->selectedItems();
        this->ui->treeDevices->clearSelection();
        this->ui->treeDevices->clear();
        QString devName, pt1, pt2, pt3;
        QString separator="/";
        QStringList qsl;
        QTreeWidgetItem* treeItem1;
        QTreeWidgetItem* treeItem2;
        QTreeWidgetItem* treeItem3;
        QList<QTreeWidgetItem*> foundItems;
        // Go through list of found devices and build tree widget:
        for (int i=0; i < this->mFilteredDeviceList.size(); i++)
        {
            devName = this->mFilteredDeviceList[i];

            qsl = devName.split(separator);
            if (qsl.count() == 3)
            {
                pt1 = qsl[0];
                pt2 = qsl[1];
                pt3 = qsl[2];

                foundItems = this->ui->treeDevices->findItems(pt1, Qt::MatchExactly, 0);
                if (foundItems.length() == 0)
                {
                    treeItem1 = new QTreeWidgetItem(this->ui->treeDevices);
                    treeItem1->setText(0, pt1);
                }
                else
                {
                    treeItem1 = foundItems[0];
                }
                foundItems.clear();

                // Search second layer of the tree for string pt2
                int k = 0;
                while (k < treeItem1->childCount())
                {
                    if (treeItem1->child(k)->text(0) == pt2)
                    {
                        // Found it. Use this to insert new device
                        treeItem2 = treeItem1->child(k);
                        break;
                    }
                    k++;
                }
                if (k == treeItem1->childCount())
                {
                    // If the count variable is childCount we didn't find the string, insert new leaf
                    treeItem2 = new QTreeWidgetItem();
                    treeItem2->setText(0, pt2);
                    treeItem1->addChild(treeItem2);
                }

                // The last part should always be new
                treeItem3 = new QTreeWidgetItem();
                treeItem3->setText(0, pt3);
                treeItem2->addChild(treeItem3);
            }

        }

        // Expand all items if we have fewer than 4 top levels:
        if (this->ui->treeDevices->topLevelItemCount() < 4)
        {
            for (int topI = 0; topI < this->ui->treeDevices->topLevelItemCount(); topI++)
            {
                this->ui->treeDevices->topLevelItem(topI)->setExpanded(true);
                for (int chI=0; chI < this->ui->treeDevices->topLevelItem(topI)->childCount(); chI++)
                {
                    this->ui->treeDevices->topLevelItem(topI)->child(chI)->setExpanded(true);
                }
            }
        }
        this->blockSignals(false);
    } // \isResultReadyAt
}

void AttributeSelectForm::populateAttributeList(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (current != nullptr)
    {
        if (current->childCount() == 0)
        {
            // Bottom level item activated => we have a full device name
            QString deviceName = current->parent()->parent()->text(0) + "/" + current->parent()->text(0) + "/" + current->text(0);
            string s = deviceName.toStdString();
            vector<string>* attrList;
            try
            {
                Tango::DeviceProxy* dev = new Tango::DeviceProxy(s);
                 attrList = dev->get_attribute_list();
            } catch (Tango::DevFailed df)
            {
                Tango::DevError er;
                for (int i=0; i< df.errors.length(); i++)
                {
                    er = df.errors[i];
                    qDebug() << er.origin;
                    cout << "In " << er.origin << ": " << er.desc << "\n";

                }
                return;
            }

            this->ui->treeAttributes->clear();  // Remove old attribute list
            QTreeWidgetItem* treeItem;
            QTreeWidgetItem* treeItemDev;
            treeItemDev = new QTreeWidgetItem(this->ui->treeAttributes);
            treeItemDev->setText(0, deviceName);
            treeItemDev->setExpanded(true);

            for (int i=0; i<attrList->size(); i++)
            {
                treeItem = new QTreeWidgetItem();
                QString s = QString::fromStdString(attrList->at(i));
                treeItem->setText(0, s);
                treeItemDev->addChild(treeItem);
            }

            delete attrList;
        }
    }
}

void AttributeSelectForm::addAttribute()
{
    QList<QTreeWidgetItem*> qsl = this->ui->treeAttributes->selectedItems();
    if (qsl.size()>0)
    {
        QTreeWidgetItem* treeItem = qsl[0];
        QString attributeName = this->ui->treeAttributes->topLevelItem(0)->text(0) + "/" + treeItem->text(0);
        // Check for duplicates:
        QList<QListWidgetItem*> foundItems = this->ui->listAttributes->findItems(attributeName, Qt::MatchExactly);
        if (foundItems.count() == 0)
        {
            this->ui->listAttributes->addItem(attributeName);
        }
    }
}

QList<QString> AttributeSelectForm::getAttributes()
{
    QList<QString> attrList;
    for (int i=0; i < this->ui->listAttributes->count(); i++)
    {
        attrList.append(this->ui->listAttributes->item(i)->text());
    }
    return attrList;
}

void AttributeSelectForm::selectDeviceFromAttributeList(QModelIndex ind)
{
    // First find the item clicked and separate the name into the tango
    // hiearchy. Then go through the device tree to find the matching
    // device level by level. When the full device name is found the
    // device attribute tree is built and the tree traversing loop stops.
    QString attrName = this->ui->listAttributes->item(ind.row())->text();
//    qDebug() << "Double click!! Found " << attrName << "\n";
    QString separator="/";
    QStringList qsl = attrName.split(separator);
    QList<QTreeWidgetItem*> devList0;
    QTreeWidgetItem* devList1;
    QTreeWidgetItem* devList2;
    bool devFound = false;

    devList0 = this->ui->treeDevices->findItems(qsl[0], Qt::MatchExactly, 0);
    for (int i0 = 0; i0 < devList0.size(); i0++)
    {
        for (int ch0 = 0; ch0 < devList0[i0]->childCount(); ch0++)
        {
            devList1 = devList0[i0]->child(ch0);
            if (devList1->text(0) == qsl[1])
            {
                for (int ch1 = 0; ch1 < devList1->childCount(); ch1++)
                {
                    devList2 = devList1->child(ch1);
                    if (devList2->text(0) == qsl[2])
                    {
                        this->populateAttributeList(devList2, nullptr);
                        devList1->setExpanded(true);
                        devList0[i0]->setExpanded(true);
                        devList2->setSelected(true);
                        devFound = true;
                    }
                    if (devFound == true)
                    {
                        break;
                    }
                }
            }
            if (devFound == true)
            {
                break;
            }
        }
        if (devFound == true)
        {
            break;
        }
    }
}

void AttributeSelectForm::deleteSelection()
{

        QModelIndexList selIndexList = this->ui->listAttributes->selectionModel()->selectedIndexes();
        qDebug() << "Delete!! Found " << QString::number(selIndexList.count()) << " items selected \n";
        for (int i=0; i < selIndexList.count(); i++)
        {
            qDebug() << "Removing " << this->ui->listAttributes->item(selIndexList.at(i).row())->text();
            this->ui->listAttributes->takeItem(selIndexList.at(i).row());
        }
}

void AttributeSelectForm::closePanel()
{
//    this->parent()->accept();
    this->close();
}


AttributeSelectForm::DatabaseReadClass::DatabaseReadClass(QMutex* mutex )
{
    this->mMutex = mutex;
}

AttributeSelectForm::DatabaseReadClass::~DatabaseReadClass()
{

}

void AttributeSelectForm::DatabaseReadClass::populateDeviceList(QString tangoDataBaseName)
{

    Tango::Database* tangoDatabase;
    tangoDatabase = new Tango::Database();
    vector<string> fullDeviceList;

    string s = "*";
    try
    {
        Tango::DbDatum dbDatum = tangoDatabase->get_device_exported(s);
        Tango::DbDatum dbDatumDev;
        fullDeviceList = dbDatum.value_string;
//        this->filterDeviceList();
    }
    catch (Tango::DevFailed df)
    {
        Tango::DevError er;
        for (int i=0; i< df.errors.length(); i++)
        {
            er = df.errors[i];
            qDebug() << er.origin;
            cout << "In " << er.origin << ": " << er.desc << "\n";

        }
        return;
    }

}
