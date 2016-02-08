#ifndef ATTRIBUTESELECTFORM_H
#define ATTRIBUTESELECTFORM_H

#include <QWidget>
#include <tango.h>
#include <QTreeWidgetItem>
#include <QShortcut>
#include <QThread>
#include <QMutex>
#include <QFuture>
#include <QFutureWatcher>
#include <QStringList>

namespace Ui {
class AttributeSelectForm;
}

class AttributeSelectForm : public QWidget
{
    Q_OBJECT

public:
    explicit AttributeSelectForm(QWidget *parent = 0, QList<QString> currentAttributeList = QList<QString>(), QString tangoDatabase = "");
    ~AttributeSelectForm();
    QList<QString> getAttributes();

private:
    Ui::AttributeSelectForm *ui;
    void populateAttributeList();    

    QString mTangoDatabaseName;
    Tango::Database* mTangoDatabase;
//    vector<string> mFullDeviceList;
//    vector<string> mFilteredDeviceList;
    QStringList mFullDeviceList;
    QStringList mFilteredDeviceList;

    QShortcut* mDeleteShortcut;

    QThread databaseThread;
    QFuture<QStringList> mFutureFullDeviceList;
    QFutureWatcher<QStringList> mFutureWatcherFullDeviceList;
    QStringList queryDeviceListDatabase(Tango::Database* tangoDatabase);

    class DatabaseReadClass
    {
    public:
        DatabaseReadClass(QMutex* mutex);
        ~DatabaseReadClass();
        void populateDeviceList(QString tangoDataBaseName);

    private:
        QMutex* mMutex;
    };

private slots:
    void populateDeviceList();
    void populateAttributeList(QTreeWidgetItem* current,QTreeWidgetItem* previous);
    void selectDeviceFromAttributeList(QModelIndex ind);
    void readDatabaseDeviceList();
    void deleteSelection();
    void filterDeviceList();
    void addAttribute();
    void closePanel();
};

#endif // ATTRIBUTESELECTFORM_H
