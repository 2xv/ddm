//  #======================================#
//  # Dynamic Data Manager - Version 1.0.0 #
//  #                                      #
//  # Author: Marco Vagnoni                #
//  # Date:   April     2019 (v1.0.0)      #
//  #======================================#

#ifndef DDMANAGER_H
#define DDMANAGER_H

#include <QtGlobal>
#include <QByteArray>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QString>
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

    int                    getDataCount         ();
    int                    getPropertiesCount   ();

    char                   getByte              (int id);
    short                  getShort             (int id);
    int                    getInt               (int id);
    qint64                 getLong              (int id);
    float                  getFloat             (int id);
    double                 getDouble            (int id);
    qint64                 getNumber            (int id);
    qint64                 getSignedNumber      (int id);
    QTime                 &getTime              (int id);
    QDate                 &getDate              (int id);
    QDateTime             &getDateTime          (int id);
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
    QVector <QTime>       &getTimeArray         (int id);
    QVector <QDate>       &getDateArray         (int id);
    QVector <QDateTime>   &getDateTimeArray     (int id);
    QVector <QByteArray>  &getBytesArray        (int id);
    QVector <QString>     &getStringArray       (int id);

    DDProps               *getProperties        (int id);

    bool                   isDataCheck          ();
    bool                   isDataDefault        ();
    bool                   isDataPresent        (int  id);

    void                   setDataCheck         (bool dataCheck);
    void                   setDataDefault       (bool dataDefault);

    void                   setByte              (int id, char        data);
    void                   setShort             (int id, short       data);
    void                   setInt               (int id, int         data);
    void                   setLong              (int id, qint64      data);
    void                   setFloat             (int id, float       data);
    void                   setDouble            (int id, double      data);
    void                   setNumber            (int id, qint64      data);
    void                   setSignedNumber      (int id, qint64      data);
    void                   setTime              (int id, QTime      &data);
    void                   setDate              (int id, QDate      &data);
    void                   setDateTime          (int id, QDateTime  &data);
    void                   setBytes             (int id, QByteArray &data);
    void                   setString            (int id, QString    &data);
    void                   setList              (int id, DDList     &data);

    void                   setByteArray         (int id, QVector <char>       &data);
    void                   setShortArray        (int id, QVector <short>      &data);
    void                   setIntArray          (int id, QVector <int>        &data);
    void                   setLongArray         (int id, QVector <qint64>     &data);
    void                   setFloatArray        (int id, QVector <float>      &data);
    void                   setDoubleArray       (int id, QVector <double>     &data);
    void                   setNumberArray       (int id, QVector <qint64>     &data);
    void                   setSignedNumberArray (int id, QVector <qint64>     &data);
    void                   setTimeArray         (int id, QVector <QTime>      &data);
    void                   setDateArray         (int id, QVector <QDate>      &data);
    void                   setDateTimeArray     (int id, QVector <QDateTime>  &data);
    void                   setBytesArray        (int id, QVector <QByteArray> &data);
    void                   setStringArray       (int id, QVector <QString>    &data);

    void                   setProperties        (DDProps &properties);
    void                   setProperties        (QVector <DDProps> &properties);

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

    QString                toString             ();

private:

    void                  *dataRoot       [16];
    void                  *propertiesRoot [16];
    bool                   checked        [4096];
    unsigned char          dataTypes      [4096];

    bool                   dataCheck;
    bool                   dataDefault;
    int                    dataCount;
    int                    propertiesCount;

    DDBuffer::ObjectNode  *newObjectList;

    void                  *getData              (int id, int   type);
    void                   setData              (int id, void *data, int type);

    void                   addNew               (void *data);
  [[noreturn]] void        unexpectedData       (int id, int expected, int requested);

    void                   checkData            (int type, void  *data,  DDProps *props);
    bool                   checkCommon          (int type, qint64 value, DDProps *props);
    void                   checkByte            (int type, char        value, DDProps *props);
    void                   checkShort           (int type, short       value, DDProps *props);
    void                   checkInt             (int type, int         value, DDProps *props);
    void                   checkLong            (int type, qint64      value, DDProps *props);
    void                   checkFloat           (int type, int         value, DDProps *props);
    void                   checkDouble          (int type, qint64      value, DDProps *props);
    void                   checkTime            (int type, QTime      *value, DDProps *props);
    void                   checkDate            (int type, QDate      *value, DDProps *props);
    void                   checkDateTime        (int type, QDateTime  *value, DDProps *props);
    void                   checkBytes           (int type, QByteArray *value, DDProps *props);
    void                   checkString          (int type, QString    *value, DDProps *props);
    void                   checkArray           (int type, void *data, DDProps *props);

    bool                   checkDataDefault     (int type, void *data1,  void *data2);
    bool                   checkArrayDefault    (int type, void *array1, void *array2);

    void                 **linkObject           (int id, void **obj);
    void                  *seekObject           (int id, void **obj);
    void                   storeData            (int id, void  *data, int type);
};

#endif    // DDMANAGER_H
