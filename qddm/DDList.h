//  #===================================#
//  # Dynamic Data List - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

#ifndef DDLIST_H
#define DDLIST_H

#include <QByteArray>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QString>
#include <QVector>

#include "DDNode.h"
#include "DDProps.h"

class DDList
{
public:

    DDList ();
   ~DDList ();

    int                    getCurrentDataId     ();
    int                    getCurrentDataType   ();
    int                    getDataCount         ();

    void                   addData              (void *data, int type);
    void                  *getData              (int   type);

    bool                   isEndOfList          ();

    void                   begin                ();
    void                   next                 ();

    void                   addByte              (char        data);
    void                   addShort             (short       data);
    void                   addInt               (int         data);
    void                   addLong              (qint64      data);
    void                   addFloat             (float       data);
    void                   addDouble            (double      data);
    void                   addNumber            (qint64      data);
    void                   addSignedNumber      (qint64      data);
    void                   addTime              (QTime      *data);
    void                   addDate              (QDate      *data);
    void                   addDateTime          (QDateTime  *data);
    void                   addBytes             (QByteArray *data);
    void                   addString            (QString    *data);
    void                   addList              (DDList     *data);

    void                   addByteArray         (QVector <char>       *data);
    void                   addShortArray        (QVector <short>      *data);
    void                   addIntArray          (QVector <int>        *data);
    void                   addLongArray         (QVector <qint64>     *data);
    void                   addFloatArray        (QVector <float>      *data);
    void                   addDoubleArray       (QVector <double>     *data);
    void                   addNumberArray       (QVector <qint64>     *data);
    void                   addSignedNumberArray (QVector <qint64>     *data);
    void                   addTimeArray         (QVector <QTime>      *data);
    void                   addDateArray         (QVector <QDate>      *data);
    void                   addDateTimeArray     (QVector <QDateTime>  *data);
    void                   addBytesArray        (QVector <QByteArray> *data);
    void                   addStringArray       (QVector <QString>    *data);

    char                   getByte              ();
    short                  getShort             ();
    int                    getInt               ();
    qint64                 getLong              ();
    float                  getFloat             ();
    double                 getDouble            ();
    qint64                 getNumber            ();
    qint64                 getSignedNumber      ();
    QTime                 *getTime              ();
    QDate                 *getDate              ();
    QDateTime             *getDateTime          ();
    QByteArray            *getBytes             ();
    QString               *getString            ();
    DDList                *getList              ();

    QVector <char>        *getByteArray         ();
    QVector <short>       *getShortArray        ();
    QVector <int>         *getIntArray          ();
    QVector <qint64>      *getLongArray         ();
    QVector <float>       *getFloatArray        ();
    QVector <double>      *getDoubleArray       ();
    QVector <qint64>      *getNumberArray       ();
    QVector <qint64>      *getSignedNumberArray ();
    QVector <QTime>       *getTimeArray         ();
    QVector <QDate>       *getDateArray         ();
    QVector <QDateTime>   *getDateTimeArray     ();
    QVector <QByteArray>  *getBytesArray        ();
    QVector <QString>     *getStringArray       ();

    QString                toString             ();
    void                   toString             (QString *str, DDProps *props);

private:

    bool     endOfList;
    int      dataCount;

    DDNode  *currentNode;
    DDNode  *firstNode;
    DDNode  *lastNode;
};

#endif    // DDLIST_H
