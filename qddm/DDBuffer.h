//  #=====================================#
//  # Dynamic Data Buffer - Version 1.0.0 #
//  #                                     #
//  # Author: Marco Vagnoni               #
//  # Email:  marco.vagnoni@yahoo.com     #
//  # Date:   April     2019 (v1.0.0)     #
//  #=====================================#

#ifndef DDBUFFER_H
#define DDBUFFER_H

#include <QtGlobal>
#include <QByteArray>
#include <QString>

#include "DDList.h"

class DDBuffer
{
friend class DDList;
friend class DDManager;
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

    DDBuffer (QByteArray &buffer, int offset, int length, ObjectNode **newObjectList);

    char                  *getBuffer         ();
    int                    getOffset         ();

    bool                   endOfBuffer       ();

    void                   addProtocolInfo   ();
    void                   checkProtocolInfo ();

    int                    addCRC32          ();
    void                   checkCRC32        ();

    void                  *read              (int &type);
    int                    readTypeAndId     ();

    void                   write             (void *data, int type);
    void                   writeTypeAndId    (int   typeAndId);

    static bool            isValidTime       (int    data);
    static bool            isValidDate       (int    data);
    static bool            isValidDateTime   (qint64 data);

    static void            stringData        (QString *str, void *data, int type);

private:

    static char            tempBuffer [64];

    QByteArray             byteArray;
    char                  *buffer;
    int                    length;
    int                    mark;
    int                    offset;
    int                    start;
    ObjectNode            *newObjectList;

    void                   addNew            (void *data, int type);
    bool                   ensureCapacity    (int bytesNumber);
  [[noreturn]] static void unexpectedData    (int id, int expected, int found);

    char                   readByte          ();
    short                  readShort         ();
    int                    readInt           ();
    qint64                 readLong          ();
    qint64                 readNumber        (bool withSign);
    int                    readTime          ();
    int                    readDate          ();
    qint64                 readDateTime      ();
    void                   readBytes         (QByteArray &bytes);
    void                   readString        (QString    &string);

    int                    readArrayBitmap   ();

    void                   writeByte         (char        value);
    void                   writeShort        (short       value);
    void                   writeInt          (int         value);
    void                   writeLong         (qint64      value);
    void                   writeNumber       (qint64      value, bool withSign);
    void                   writeTime         (int         value);
    void                   writeDate         (int         value);
    void                   writeDateTime     (qint64      value);
    void                   writeBytes        (QByteArray &value);
    void                   writeString       (QString    &value);
    void                   writeArrayBitmap  (int pos,  int bmp,     int bm);
    void                   writeArrayInfo    (int size, int numbers, int type);

    static void            stringTime        (QString *str, int         data);
    static void            stringDate        (QString *str, int         data);
    static void            stringDateTime    (QString *str, qint64      data);
    static void            stringBytes       (QString *str, QByteArray *data);
    static void            stringString      (QString *str, QString    *data);
};

#endif    // DDBUFFER_H
