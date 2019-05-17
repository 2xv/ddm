//  #===================================#
//  # Dynamic Data List - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Email:  marco.vagnoni@yahoo.com   #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

#ifndef DDLIST_H
#define DDLIST_H

#include <QByteArray>
#include <QString>
#include <QVector>

#include "DDNode.h"
#include "DDProps.h"

class DDList
{
friend class DDBuffer;
friend class DDManager;
public:

    DDList ();
   ~DDList ();

    int                   getCurrentDataOffset ();
    int                   getCurrentDataType   ();
    int                   getDataNumber        ();
    QVector <DDProps>    *getProperties        ();

    bool                  isEndOfList          ();

    void                  begin                ();
    void                  next                 ();

    void                  setProperties        (QVector <DDProps> &properties);

    void                  addByte              (char        data);
    void                  addShort             (short       data);
    void                  addInt               (int         data);
    void                  addLong              (qint64      data);
    void                  addFloat             (float       data);
    void                  addDouble            (double      data);
    void                  addNumber            (qint64      data);
    void                  addSignedNumber      (qint64      data);
    void                  addTime              (int         data);
    void                  addDate              (int         data);
    void                  addDateTime          (qint64      data);
    void                  addBytes             (QByteArray *data);
    void                  addString            (QString    *data);
    void                  addList              (DDList     *data);

    void                  addByteArray         (QVector <char>       *data);
    void                  addShortArray        (QVector <short>      *data);
    void                  addIntArray          (QVector <int>        *data);
    void                  addLongArray         (QVector <qint64>     *data);
    void                  addFloatArray        (QVector <float>      *data);
    void                  addDoubleArray       (QVector <double>     *data);
    void                  addNumberArray       (QVector <qint64>     *data);
    void                  addSignedNumberArray (QVector <qint64>     *data);
    void                  addTimeArray         (QVector <int>        *data);
    void                  addDateArray         (QVector <int>        *data);
    void                  addDateTimeArray     (QVector <qint64>     *data);
    void                  addBytesArray        (QVector <QByteArray> *data);
    void                  addStringArray       (QVector <QString>    *data);

    char                  getByte              ();
    short                 getShort             ();
    int                   getInt               ();
    qint64                getLong              ();
    float                 getFloat             ();
    double                getDouble            ();
    qint64                getNumber            ();
    qint64                getSignedNumber      ();
    int                   getTime              ();
    int                   getDate              ();
    qint64                getDateTime          ();
    QByteArray           *getBytes             ();
    QString              *getString            ();
    DDList               *getList              ();

    QVector <char>       *getByteArray         ();
    QVector <short>      *getShortArray        ();
    QVector <int>        *getIntArray          ();
    QVector <qint64>     *getLongArray         ();
    QVector <float>      *getFloatArray        ();
    QVector <double>     *getDoubleArray       ();
    QVector <qint64>     *getNumberArray       ();
    QVector <qint64>     *getSignedNumberArray ();
    QVector <int>        *getTimeArray         ();
    QVector <int>        *getDateArray         ();
    QVector <qint64>     *getDateTimeArray     ();
    QVector <QByteArray> *getBytesArray        ();
    QVector <QString>    *getStringArray       ();

    QString               toString             ();

private:

    bool                  endOfList;
    int                   dataNumber;
    QVector <DDProps>    *properties;

    DDNode               *currentNode;
    DDNode               *firstNode;
    DDNode               *lastNode;

    void                  addData              (void *data, int type);
    void                 *getData              (int   type);
    void                  toString             (QString *str);
};

#endif    // DDLIST_H
