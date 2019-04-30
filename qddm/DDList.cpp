//  #===================================#
//  # Dynamic Data List - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

#include <QTextStream>

#include "DDBuffer.h"
#include "DDConst.h"
#include "DDError.h"
#include "DDList.h"

//  #=============#
//  # Constructor #
//  #=============#

DDList::DDList ()
{
    endOfList   = true;
    dataCount   = 0;

    currentNode = nullptr;
    firstNode   = nullptr;
    lastNode    = nullptr;
}

//  #============#
//  # Destructor #
//  #============#

DDList::~DDList ()
{
    while (firstNode)
    {
        currentNode = firstNode;
        firstNode   = firstNode->getNext ();
        delete currentNode;
    }
}

//  #=====#
//  # get #
//  #=====#

int DDList::getCurrentDataId   () {return !currentNode ? -1 : currentNode->getId   ();}
int DDList::getCurrentDataType () {return !currentNode ? -1 : currentNode->getType ();}
int DDList::getDataCount       () {return  dataCount;}

float DDList::getFloat ()
{
    union {float fv; int iv;};
    iv  =  static_cast <int> (reinterpret_cast <qint64> (getData (DDC_FLOAT)));
    return fv;
}

double DDList::getDouble ()
{
    union {double dv; qint64 lv;};
    lv  =  reinterpret_cast <qint64> (getData (DDC_DOUBLE));
    return dv;
}

char                  DDList::getByte   () {return static_cast <char>  (reinterpret_cast <qint64> (getData (DDC_BYTE)));}
short                 DDList::getShort  () {return static_cast <short> (reinterpret_cast <qint64> (getData (DDC_SHORT)));}
int                   DDList::getInt    () {return static_cast <int>   (reinterpret_cast <qint64> (getData (DDC_INT)));}

qint64                DDList::getLong              () {return reinterpret_cast <qint64>           (getData (DDC_LONG));}
qint64                DDList::getNumber            () {return reinterpret_cast <qint64>           (getData (DDC_NUMBER));}
qint64                DDList::getSignedNumber      () {return reinterpret_cast <qint64>           (getData (DDC_S_NUMBER));}

QTime                *DDList::getTime              () {return static_cast <QTime     *>           (getData (DDC_TIME));}
QDate                *DDList::getDate              () {return static_cast <QDate     *>           (getData (DDC_DATE));}
QDateTime            *DDList::getDateTime          () {return static_cast <QDateTime *>           (getData (DDC_DATETIME));}
QByteArray           *DDList::getBytes             () {return static_cast <QByteArray*>           (getData (DDC_BYTES));}
QString              *DDList::getString            () {return static_cast <QString   *>           (getData (DDC_STRING));}
DDList               *DDList::getList              () {return static_cast <DDList    *>           (getData (DDC_LIST));}

QVector <char>       *DDList::getByteArray         () {return static_cast <QVector <char>      *> (getData (DDC_A_BYTE));}
QVector <short>      *DDList::getShortArray        () {return static_cast <QVector <short>     *> (getData (DDC_A_SHORT));}
QVector <int>        *DDList::getIntArray          () {return static_cast <QVector <int>       *> (getData (DDC_A_INT));}
QVector <qint64>     *DDList::getLongArray         () {return static_cast <QVector <qint64>    *> (getData (DDC_A_LONG));}
QVector <float>      *DDList::getFloatArray        () {return static_cast <QVector <float>     *> (getData (DDC_A_FLOAT));}
QVector <double>     *DDList::getDoubleArray       () {return static_cast <QVector <double>    *> (getData (DDC_A_DOUBLE));}
QVector <qint64>     *DDList::getNumberArray       () {return static_cast <QVector <qint64>    *> (getData (DDC_A_NUMBER));}
QVector <qint64>     *DDList::getSignedNumberArray () {return static_cast <QVector <qint64>    *> (getData (DDC_A_S_NUMBER));}
QVector <QTime>      *DDList::getTimeArray         () {return static_cast <QVector <QTime>     *> (getData (DDC_A_TIME));}
QVector <QDate>      *DDList::getDateArray         () {return static_cast <QVector <QDate>     *> (getData (DDC_A_DATE));}
QVector <QDateTime>  *DDList::getDateTimeArray     () {return static_cast <QVector <QDateTime> *> (getData (DDC_A_DATETIME));}
QVector <QByteArray> *DDList::getBytesArray        () {return static_cast <QVector <QByteArray>*> (getData (DDC_A_BYTES));}
QVector <QString>    *DDList::getStringArray       () {return static_cast <QVector <QString>   *> (getData (DDC_A_STRING));}

//  #====#
//  # is #
//  #====#

bool DDList::isEndOfList () {return endOfList;}

//  #=====#
//  # add #
//  #=====#

void DDList::addByte              (char   data) {addData (reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_BYTE);}
void DDList::addShort             (short  data) {addData (reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_SHORT);}
void DDList::addInt               (int    data) {addData (reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_INT);}

void DDList::addLong              (qint64 data) {addData (reinterpret_cast <void *> (data), DDC_LONG);}
void DDList::addNumber            (qint64 data) {addData (reinterpret_cast <void *> (data), DDC_NUMBER);}
void DDList::addSignedNumber      (qint64 data) {addData (reinterpret_cast <void *> (data), DDC_S_NUMBER);}

void DDList::addTime              (QTime                *data) {addData (data, DDC_TIME);}
void DDList::addDate              (QDate                *data) {addData (data, DDC_DATE);}
void DDList::addDateTime          (QDateTime            *data) {addData (data, DDC_DATETIME);}

void DDList::addBytes             (QByteArray           *data) {addData (data, DDC_BYTES);}
void DDList::addString            (QString              *data) {addData (data, DDC_STRING);}
void DDList::addList              (DDList               *data) {addData (data, DDC_LIST);}

void DDList::addByteArray         (QVector <char>       *data) {addData (data, DDC_A_BYTE);}
void DDList::addShortArray        (QVector <short>      *data) {addData (data, DDC_A_SHORT);}
void DDList::addIntArray          (QVector <int>        *data) {addData (data, DDC_A_INT);}
void DDList::addLongArray         (QVector <qint64>     *data) {addData (data, DDC_A_LONG);}
void DDList::addFloatArray        (QVector <float>      *data) {addData (data, DDC_A_FLOAT);}
void DDList::addDoubleArray       (QVector <double>     *data) {addData (data, DDC_A_DOUBLE);}
void DDList::addNumberArray       (QVector <qint64>     *data) {addData (data, DDC_A_NUMBER);}
void DDList::addSignedNumberArray (QVector <qint64>     *data) {addData (data, DDC_A_S_NUMBER);}
void DDList::addTimeArray         (QVector <QTime>      *data) {addData (data, DDC_A_TIME);}
void DDList::addDateArray         (QVector <QDate>      *data) {addData (data, DDC_A_DATE);}
void DDList::addDateTimeArray     (QVector <QDateTime>  *data) {addData (data, DDC_A_DATETIME);}
void DDList::addBytesArray        (QVector <QByteArray> *data) {addData (data, DDC_A_BYTES);}
void DDList::addStringArray       (QVector <QString>    *data) {addData (data, DDC_A_STRING);}

void DDList::addFloat (float data)
{
    union {float fv; int iv;}; fv = data;
    addData (reinterpret_cast <void *> (static_cast <qint64> (iv)), DDC_FLOAT);
}

void DDList::addDouble (double data)
{
    union {double dv; qint64 lv;}; dv = data;
    addData (reinterpret_cast <void *> (lv), DDC_DOUBLE);
}

//  #=========#
//  # addData #
//  #=========#

void DDList::addData (void *data, int type)
{
    if (type < DDC_NULL || type > DDC_A_STRING || (type > DDC_ARRAY && type < DDC_A_BYTE))
        DDException (DDE_INVALID_DATA, dataCount);

    bool valid = true;

    if (type >= DDC_TIME)
    {
        if (!data) type = DDC_NULL;
        else
            switch (type)
            {
                case DDC_TIME:     if (!static_cast <QTime     *> (data)->isValid ()) valid = false; break;
                case DDC_DATE:     if (!static_cast <QDate     *> (data)->isValid ()) valid = false; break;
                case DDC_DATETIME: if (!static_cast <QDateTime *> (data)->isValid ()) valid = false; break;

                case DDC_A_NUMBER:

                    for (int i = 0; i < static_cast <QVector <qint64> *> (data)->size (); i++)
                         if (static_cast <QVector <qint64> *> (data)->at (i) < 0) {valid = false; break;}

                    break;

                case DDC_A_TIME:

                    for (int i = 0; i < static_cast <QVector <QTime> *> (data)->size (); i++)
                        if (!static_cast <QVector <QTime> *> (data)->at (i).isNull   ()  &&
                            !static_cast <QVector <QTime> *> (data)->at (i).isValid  ()) {valid = false; break;}
                    break;

                case DDC_A_DATE:

                    for (int i = 0; i < static_cast <QVector <QDate> *> (data)->size (); i++)
                        if (!static_cast <QVector <QDate> *> (data)->at (i).isNull   ()  &&
                            !static_cast <QVector <QDate> *> (data)->at (i).isValid  ()) {valid = false; break;}
                    break;

                case DDC_A_DATETIME:

                    for (int i = 0; i < static_cast <QVector <QDateTime> *> (data)->size (); i++)
                        if (!static_cast <QVector <QDateTime> *> (data)->at (i).isNull   ()  &&
                            !static_cast <QVector <QDateTime> *> (data)->at (i).isValid  ()) {valid = false; break;}
                    break;

                default:;
            }
    }
    else
        if (type == DDC_NUMBER && reinterpret_cast <qint64> (data) < 0) valid = false;

    if (!valid) DDException (DDE_INVALID_DATA, dataCount);

    if (dataCount >= DDC_MAX_ITEMS_NUMBER)
        DDException (DDE_LIST_OVERFLOW, dataCount, DDC_MAX_ITEMS_NUMBER);

    DDNode *newNode = new DDNode (dataCount++, data, type);

    if (!firstNode) firstNode = newNode;
    else lastNode->setNext (newNode);

    lastNode = newNode;
}

//  #=========#
//  # getData #
//  #=========#

void *DDList::getData (int type)
{
    if (!currentNode) return nullptr;

    if (type > DDC_NULL && currentNode->getType () > DDC_NULL && type != currentNode->getType ())
    {
        char expectedType [32];
        char requiredType [32];

        int  storedIdH   = currentNode->getType () >> 4;
        int  requiredIdH = type >> 4;
        int  i, j;

        if (!storedIdH) i = 0;
        else i = snprintf (expectedType, 32, "%s", DDBuffer::STRING_TYPES [storedIdH]);

        if (!requiredIdH) j = 0;
        else j = snprintf (requiredType, 32, "%s", DDBuffer::STRING_TYPES [requiredIdH]);

        snprintf (expectedType + i, 32 - static_cast <unsigned> (i), "%s",DDBuffer::STRING_TYPES [currentNode->getType () & 0x0F]);
        snprintf (requiredType + j, 32 - static_cast <unsigned> (j), "%s",DDBuffer::STRING_TYPES [type & 0x0F]);

        DDException (DDE_INVALID_DATA_TYPE, currentNode->getId (), expectedType, requiredType);
    }

    return currentNode->getObject ();
}

//  #=======#
//  # begin #
//  #=======#

void DDList::begin ()
{
    currentNode = firstNode;
    endOfList   = firstNode == nullptr;
}

//  #======#
//  # next #
//  #======#

void DDList::next ()
{
    if (currentNode)
    {
        currentNode = currentNode->getNext ();
        if (!currentNode) endOfList = true;
    }
}

//  #==========#
//  # toString #
//  #==========#

QString DDList::toString ()
{
    QString    str;
    toString (&str, nullptr);
    return     str;
}

void DDList::toString (QString *str, DDProps *props)
{
    QTextStream (str) << getDataCount () << '{';

    void              *obj;
    int                i, j = 0;
    bool               sep  = false;
    QVector <DDProps> *p    = props == nullptr ? nullptr : static_cast <QVector <DDProps> *> (props->inOrProps);

    if (p) j = p->size ();
    begin ();

    while (!isEndOfList ())
    {
        if (sep) QTextStream (str) << ';';
        sep = true;

        if (j) QTextStream (str) << p->data () [getCurrentDataId () % j].name << '@';

        obj = getData (-1);
        i   = getCurrentDataType ();

        if (i != DDC_NULL)
        {
            QTextStream (str) << DDBuffer::DATA_TYPES [i & 0x0F];
            DDBuffer::stringData (str, obj, i, nullptr);
        }

        next ();
    }

    QTextStream (str) << '}';
}
