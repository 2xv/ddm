//  #===================================#
//  # Dynamic Data List - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Email:  marco.vagnoni@yahoo.com   #
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
    dataNumber  = 0;
    properties  = nullptr;

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

int                   DDList::getCurrentDataOffset () {return !currentNode ? -1 : currentNode->getOffset ();}
int                   DDList::getCurrentDataType   () {return !currentNode ? -1 : currentNode->getType   ();}
int                   DDList::getDataNumber        () {return  dataNumber;}
QVector <DDProps>    *DDList::getProperties        () {return  properties;}

char                  DDList::getByte              () {return static_cast <char>  (reinterpret_cast <qint64> (getData (DDC_BYTE)));}
short                 DDList::getShort             () {return static_cast <short> (reinterpret_cast <qint64> (getData (DDC_SHORT)));}
int                   DDList::getInt               () {return static_cast <int>   (reinterpret_cast <qint64> (getData (DDC_INT)));}
int                   DDList::getTime              () {return static_cast <int>   (reinterpret_cast <qint64> (getData (DDC_TIME)));}
int                   DDList::getDate              () {return static_cast <int>   (reinterpret_cast <qint64> (getData (DDC_DATE)));}

qint64                DDList::getLong              () {return reinterpret_cast <qint64>           (getData (DDC_LONG));}
qint64                DDList::getNumber            () {return reinterpret_cast <qint64>           (getData (DDC_NUMBER));}
qint64                DDList::getSignedNumber      () {return reinterpret_cast <qint64>           (getData (DDC_S_NUMBER));}
qint64                DDList::getDateTime          () {return reinterpret_cast <qint64>           (getData (DDC_DATETIME));}
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
QVector <int>        *DDList::getTimeArray         () {return static_cast <QVector <int>       *> (getData (DDC_A_TIME));}
QVector <int>        *DDList::getDateArray         () {return static_cast <QVector <int>       *> (getData (DDC_A_DATE));}
QVector <qint64>     *DDList::getDateTimeArray     () {return static_cast <QVector <qint64>    *> (getData (DDC_A_DATETIME));}
QVector <QByteArray> *DDList::getBytesArray        () {return static_cast <QVector <QByteArray>*> (getData (DDC_A_BYTES));}
QVector <QString>    *DDList::getStringArray       () {return static_cast <QVector <QString>   *> (getData (DDC_A_STRING));}

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

//  #====#
//  # is #
//  #====#

bool DDList::isEndOfList () {return endOfList;}

//  #=====#
//  # set #
//  #=====#

void DDList::setProperties (QVector <DDProps> &properties) {this->properties = &properties;}

//  #=====#
//  # add #
//  #=====#

void DDList::addByte              (char   data) {addData (reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_BYTE);}
void DDList::addShort             (short  data) {addData (reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_SHORT);}
void DDList::addInt               (int    data) {addData (reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_INT);}
void DDList::addLong              (qint64 data) {addData (reinterpret_cast <void *>                       (data),  DDC_LONG);}
void DDList::addSignedNumber      (qint64 data) {addData (reinterpret_cast <void *>                       (data),  DDC_S_NUMBER);}

void DDList::addBytes             (QByteArray           *data) {addData (data, DDC_BYTES);}
void DDList::addString            (QString              *data) {addData (data, DDC_STRING);}
void DDList::addList              (DDList               *data) {addData (data, DDC_LIST);}
void DDList::addByteArray         (QVector <char>       *data) {addData (data, DDC_A_BYTE);}
void DDList::addShortArray        (QVector <short>      *data) {addData (data, DDC_A_SHORT);}
void DDList::addIntArray          (QVector <int>        *data) {addData (data, DDC_A_INT);}
void DDList::addLongArray         (QVector <qint64>     *data) {addData (data, DDC_A_LONG);}
void DDList::addFloatArray        (QVector <float>      *data) {addData (data, DDC_A_FLOAT);}
void DDList::addDoubleArray       (QVector <double>     *data) {addData (data, DDC_A_DOUBLE);}
void DDList::addSignedNumberArray (QVector <qint64>     *data) {addData (data, DDC_A_S_NUMBER);}
void DDList::addBytesArray        (QVector <QByteArray> *data) {addData (data, DDC_A_BYTES);}
void DDList::addStringArray       (QVector <QString>    *data) {addData (data, DDC_A_STRING);}

void DDList::addNumber (qint64 data)
    {addData (reinterpret_cast <void *> (data), (data < 0 ? DDC_S_NUMBER : DDC_NUMBER));}

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

void DDList::addTime (int data)
{
    if (!DDBuffer::isValidTime (data))  DDException (DDE_INVALID_LIST_DATA, dataNumber);
    addData (reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_TIME);
}

void DDList::addDate (int data)
{
    if (!DDBuffer::isValidDate (data))  DDException (DDE_INVALID_LIST_DATA, dataNumber);
    addData (reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_DATE);
}

void DDList::addDateTime (qint64 data)
{
    if (!DDBuffer::isValidDateTime (data)) DDException (DDE_INVALID_LIST_DATA, dataNumber);
    addData (reinterpret_cast <void *> (data), DDC_DATETIME);
}

void DDList::addNumberArray (QVector <qint64> *data)
{
    int type = DDC_A_NUMBER;

    if (data)
        for (int i = 0; i < data->size (); i++)
             if (data->at (i) < 0) {type = DDC_A_S_NUMBER; break;}

    addData (data, type);
}

void DDList::addTimeArray (QVector <int> *data)
{
    if (data)
        for (int i = 0; i < data->size (); i++)
            if (!DDBuffer::isValidTime (data->at (i))) DDException (DDE_INVALID_LIST_DATA, dataNumber);

    addData (data, DDC_A_TIME);
}

void DDList::addDateArray (QVector <int> *data)
{
    if (data)
        for (int i = 0; i < data->size (); i++)
            if (!DDBuffer::isValidDate (data->at (i))) DDException (DDE_INVALID_LIST_DATA, dataNumber);

    addData (data, DDC_A_DATE);
}

void DDList::addDateTimeArray (QVector <qint64> *data)
{
    if (data)
        for (int i = 0; i < data->size (); i++)
            if (!DDBuffer::isValidDateTime (data->at (i))) DDException (DDE_INVALID_LIST_DATA, dataNumber);

    addData (data, DDC_A_DATETIME);
}

//  #=========#
//  # addData #
//  #=========#

void DDList::addData (void *data, int type)
{
    if (dataNumber >= DDC_MAX_ITEMS_NUMBER)
        DDException  (DDE_LIST_OVERFLOW, dataNumber, DDC_MAX_ITEMS_NUMBER);

    DDNode *newNode = new DDNode (dataNumber++, data, (data == nullptr ? DDC_NULL : type));

    if (!firstNode) firstNode = newNode;
    else lastNode->setNext (newNode);

    lastNode = newNode;
}

//  #=========#
//  # getData #
//  #=========#

void *DDList::getData (int type)
{
    if (!currentNode) DDExceptionNoArgs (DDE_END_OF_LIST);
    int currentType = currentNode->getType ();

    if (type >= DDC_NULL   && type != currentType    &&
       (type <  DDC_LONG   || type >  DDC_S_NUMBER   || currentType < DDC_LONG   || currentType > DDC_S_NUMBER) &&
       (type <  DDC_A_LONG || type >  DDC_A_S_NUMBER || currentType < DDC_A_LONG || currentType > DDC_A_S_NUMBER))
            DDBuffer::unexpectedData (currentNode->getOffset (), currentType, type);

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
    toString (&str);
    return     str;
}

void DDList::toString (QString *str)
{
    QTextStream (str) << getDataNumber () << '{';

    bool sep  = false;
    int  i, j = 0, k;

    if (properties) j = properties->size ();
    begin ();

    while (!isEndOfList ())
    {
        if (sep) QTextStream (str) << ';';
        sep = true;

        k = getCurrentDataOffset ();

        if (j) QTextStream (str) << properties->data () [k % j].name << '@' << properties->data () [k % j].id;
        QTextStream (str) << '#' << k << '=';

        i = getCurrentDataType ();

        if (i == DDC_NULL) QTextStream (str) << "null";
        else
        {
            QTextStream (str) << DDBuffer::DATA_TYPES [i & 0x0F];
            DDBuffer::stringData (str, getData (-1), i);
        }

        next ();
    }

    QTextStream (str) << '}';
}
