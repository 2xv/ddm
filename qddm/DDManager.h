//  #======================================#
//  # Dynamic Data Manager - Version 1.0.0 #
//  #                                      #
//  # Author: Marco Vagnoni                #
//  # Email:  marco.vagnoni@yahoo.com      #
//  # Date:   April     2019 (v1.0.0)      #
//  #======================================#

#ifndef DDMANAGER_H
#define DDMANAGER_H

#include <QtGlobal>
#include <QByteArray>
#include <QDate>
#include <QDateTime>
#include <QString>
#include <QTime>
#include <QVector>

#include "DDBuffer.h"
#include "DDConst.h"
#include "DDError.h"
#include "DDList.h"
#include "DDProps.h"

class DDManager
{
public:

    DDManager ();
   ~DDManager ();

    int                    getDataNumber        ();
    int                    getPropertiesNumber  ();

    char                   getByte              (int id);
    short                  getShort             (int id);
    int                    getInt               (int id);
    qint64                 getLong              (int id);
    float                  getFloat             (int id);
    double                 getDouble            (int id);
    qint64                 getNumber            (int id);
    qint64                 getSignedNumber      (int id);
    int                    getTime              (int id);
    int                    getDate              (int id);
    qint64                 getDateTime          (int id);
    QByteArray            &getBytes             (int id);
    QString               &getString            (int id);
    DDList                &getList              (int id);

    QVector <char>        &getByteArray         (int id);
    QVector <short>       &getShortArray        (int id);
    QVector <int>         &getIntArray          (int id);
    QVector <qint64>      &getLongArray         (int id);
    QVector <float>       &getFloatArray        (int id);
    QVector <double>      &getDoubleArray       (int id);
    QVector <qint64>      &getNumberArray       (int id);
    QVector <qint64>      &getSignedNumberArray (int id);
    QVector <int>         &getTimeArray         (int id);
    QVector <int>         &getDateArray         (int id);
    QVector <qint64>      &getDateTimeArray     (int id);
    QVector <QByteArray>  &getBytesArray        (int id);
    QVector <QString>     &getStringArray       (int id);

    DDProps               *getProperties        (int id);
    void                   getProperties        (QVector <DDProps> &propereties);
    QVector <DDProps>     *getListProperties    (int id);

    bool                   isDataCheck          ();
    bool                   isDataDefault        ();
    bool                   isDataPresent        (int id);

    void                   setDataCheck         (bool dataCheck);
    void                   setDataDefault       (bool dataDefault);

    void                   setByte              (int id, char   data);
    void                   setShort             (int id, short  data);
    void                   setInt               (int id, int    data);
    void                   setLong              (int id, qint64 data);
    void                   setFloat             (int id, float  data);
    void                   setDouble            (int id, double data);
    void                   setNumber            (int id, qint64 data);
    void                   setSignedNumber      (int id, qint64 data);
    void                   setTime              (int id, int    data);
    void                   setDate              (int id, int    data);
    void                   setDateTime          (int id, qint64 data);
    void                   setBytes             (int id, const  QByteArray &data);
    void                   setString            (int id, const  QString    &data);
    void                   setList              (int id, const  DDList     &data);

    void                   setByteArray         (int id, const  QVector <char>       &data);
    void                   setShortArray        (int id, const  QVector <short>      &data);
    void                   setIntArray          (int id, const  QVector <int>        &data);
    void                   setLongArray         (int id, const  QVector <qint64>     &data);
    void                   setFloatArray        (int id, const  QVector <float>      &data);
    void                   setDoubleArray       (int id, const  QVector <double>     &data);
    void                   setNumberArray       (int id, const  QVector <qint64>     &data);
    void                   setSignedNumberArray (int id, const  QVector <qint64>     &data);
    void                   setTimeArray         (int id, const  QVector <int>        &data);
    void                   setDateArray         (int id, const  QVector <int>        &data);
    void                   setDateTimeArray     (int id, const  QVector <qint64>     &data);
    void                   setBytesArray        (int id, const  QVector <QByteArray> &data);
    void                   setStringArray       (int id, const  QVector <QString>    &data);

    void                   setProperties        (DDProps &properties);
    void                   setProperties        (QVector <DDProps> &properties);
    void                   setListProperties    (int id,  QVector <DDProps> &properties);

    void                   decode               (QByteArray &buffer);
    void                   decode               (QByteArray &buffer, int  offset);
    void                   decode               (QByteArray &buffer, bool checkCRC32);
    void                   decode               (QByteArray &buffer, bool checkProtocolInfo,   bool checkCRC32);
    void                   decode               (QByteArray &buffer, int  offset, bool checkCRC32);
    void                   decode               (QByteArray &buffer, int  offset, bool checkProtocolInfo, bool checkCRC32);
    void                   decode               (QByteArray &buffer, int  offset, int  length, bool checkProtocolInfo, bool checkCRC32);

    void                   encode               (QByteArray &buffer);
    void                   encode               (QByteArray &buffer, int  reservedSizeBefore);
    void                   encode               (QByteArray &buffer, bool addCRC32);
    void                   encode               (QByteArray &buffer, bool addProtocolInfo,    bool addCRC32);
    void                   encode               (QByteArray &buffer, int  reservedSizeBefore, bool addCRC32);
    void                   encode               (QByteArray &buffer, int  reservedSizeBefore, bool addProtocolInfo,   bool addCRC32);
    void                   encode               (QByteArray &buffer, int  reservedSizeBefore, int  reservedSizeAfter, bool addProtocolInfo, bool addCRC32);

    static int             getYear              (int    date);
    static int             getMonth             (int    date);
    static int             getDay               (int    date);
    static int             getHour              (int    time);
    static int             getMinute            (int    time);
    static int             getSecond            (int    time);
    static int             getMSec              (int    time);
    static int             getYear              (qint64 datetime);
    static int             getMonth             (qint64 datetime);
    static int             getDay               (qint64 datetime);
    static int             getHour              (qint64 datetime);
    static int             getMinute            (qint64 datetime);
    static int             getSecond            (qint64 datetime);
    static int             getMSec              (qint64 datetime);

    static int             getTime              (int hour, int minute, int second, int msec = 0);
    static int             getDate              (int year, int month,  int day);
    static qint64          getDateTime          (int year, int month,  int day,    int hour, int minute, int second, int msec = 0);
    static int             getTime              (const QTime     &time);
    static int             getDate              (const QDate     &date);
    static qint64          getDateTime          (const QDateTime &datetime);
    static QTime           getQTime             (int    time);
    static QDate           getQDate             (int    date);
    static QDateTime       getQDateTime         (qint64 datetime);

    QString                toString             ();

private:

    void                  *dataRoot       [16];
    void                  *propertiesRoot [16];
    bool                   checked        [4096];
    unsigned char          dataTypes      [4096];

    bool                   dataCheck;
    bool                   dataDefault;
    int                    dataNumber;
    int                    propertiesNumber;

    DDBuffer::ObjectNode  *newObjectList;

    void                  *getData              (int id, int type);
    void                   setData              (int id, const void *data, int type);

    void                   addNew               (void *data);

    void                   checkData            (int type, void       *data, DDProps *props);
    bool                   checkCommon          (int type, qint64      data, DDProps *props);
    void                   checkByte            (int type, char        data, DDProps *props);
    void                   checkShort           (int type, short       data, DDProps *props);
    void                   checkInt             (int type, int         data, DDProps *props);
    void                   checkLong            (int type, qint64      data, DDProps *props);
    void                   checkFloat           (int type, int         data, DDProps *props);
    void                   checkDouble          (int type, qint64      data, DDProps *props);
    void                   checkTime            (int type, int         data, DDProps *props);
    void                   checkDate            (int type, int         data, DDProps *props);
    void                   checkDateTime        (int type, qint64      data, DDProps *props);
    void                   checkBytes           (int type, QByteArray *data, DDProps *props);
    void                   checkString          (int type, QString    *data, DDProps *props);
    void                   checkArray           (int type, void       *data, DDProps *props);

    bool                   compareData          (int type, const void *data1,  void *data2);
    bool                   compareArray         (int type, const void *array1, void *array2);
    bool                   compareList          (DDList  *list1, DDList *list2);

    void                   checkRequired        ();
    const void           **linkObject           (int id, void **obj);
    void                  *seekObject           (int id, void **obj);
    void                   storeData            (int id, const  void *data, int type);
};

#endif    // DDMANAGER_H
