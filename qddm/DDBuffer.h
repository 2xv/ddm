//  #=====================================#
//  # Dynamic Data Buffer - Version 1.0.0 #
//  #                                     #
//  # Author: Marco Vagnoni               #
//  # Date:   April     2019 (v1.0.0)     #
//  #=====================================#

#ifndef DDBUFFER_H
#define DDBUFFER_H

#include <QtGlobal>
#include <QByteArray>
#include <QDate>
#include <QTime>
#include <QString>

#include "DDList.h"

class DDBuffer
{
public:

    struct ObjectNode
    {
        ObjectNode *next;
        void       *object;
        int         type;
    };

    static const char  DATA_TYPES   [];
    static const char  MONTH_DAYS   [];
    static const char *STRING_TYPES [];

    DDBuffer (QByteArray *buffer, int offset, int length, ObjectNode **newObjectList);

    QByteArray *getBuffer          ();
    int         getOffset          ();

    bool        endOfBuffer        ();

    void        addProtocolInfo    ();
    void        checkProtocolInfo  ();

    int         addCRC32           ();
    void        checkCRC32         ();

    void       *read               (char &type);
    int         readTypeAndId      ();

    void        write              (void *data, int type);
    void        writeTypeAndId     (int   typeAndId);

    static void stringData         (QString *str, void *data, int type, DDProps *props);

private:

    static char tempBuffer [64];

    QByteArray *buffer;
    int         length;
    int         mark;
    int         offset;
    int         start;
    ObjectNode *newObjectList;

    void        addNew             (void *data, int type);
    bool        ensureCapacity     (int bytesNumber);

    char        readByte           ();
    short       readShort          ();
    int         readInt            ();
    qint64      readLong           ();
    qint64      readNumber         (bool withSign);
    void        readTime           (QTime      &time);
    void        readDate           (QDate      &date);
    void        readDateTime       (QDateTime  &datetime);
    void        readBytes          (QByteArray &bytes);
    void        readString         (QString    &string);

    int         readArrayBitmap    ();

    void        writeByte          (char        value);
    void        writeShort         (short       value);
    void        writeInt           (int         value);
    void        writeLong          (qint64      value);
    void        writeNumber        (qint64      value, bool withSign);
    void        writeTime          (QTime      &value);
    void        writeDate          (QDate      &value);
    void        writeDateTime      (QDateTime  &value);
    void        writeBytes         (QByteArray &value);
    void        writeString        (QString    &value);
    void        writeArrayBitmap   (int pos,  int bmp,     int bm);
    void        writeArrayInfo     (int size, int numbers, int type);

    qint64      buildTime          (QTime &time, qint64 value);
    qint64      buildDate          (QDate &date);

    static void stringTime         (QString *str, QTime      *data);
    static void stringDate         (QString *str, QDate      *data);
    static void stringDateTime     (QString *str, QDateTime  *data);
    static void stringBytes        (QString *str, QByteArray *data);
    static void stringString       (QString *str, QString    *data);
};

#endif    // DDBUFFER_H
