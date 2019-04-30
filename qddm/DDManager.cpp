//  #======================================#
//  # Dynamic Data Manager - Version 1.0.0 #
//  #                                      #
//  # Author: Marco Vagnoni                #
//  # Date:   April     2019 (v1.0.0)      #
//  #======================================#

#include <cstring>
#include <QTextStream>
#include "DDManager.h"

//  #=============#
//  # Constructor #
//  #=============#

DDManager::DDManager ()
{
    memset (checked,        0, sizeof (checked));
    memset (dataRoot,       0, sizeof (dataRoot));
    memset (dataTypes,      0, sizeof (dataTypes));
    memset (propertiesRoot, 0, sizeof (propertiesRoot));

    dataCheck       = true;
    dataDefault     = true;
    dataCount       = 0;
    propertiesCount = 0;
    newObjectList   = nullptr;
}

//  #============#
//  # Destructor #
//  #============#

DDManager::~DDManager ()
{
    DDBuffer::ObjectNode *workObjectNode;

    while (newObjectList)
    {
        workObjectNode = newObjectList;
        newObjectList  = newObjectList->next;

        if (workObjectNode->type >= DDC_TIME)
            switch (workObjectNode->type)
            {
                case 0x100:          delete [] static_cast <void               **> (workObjectNode->object); break;
                case DDC_TIME:       delete    static_cast <QTime               *> (workObjectNode->object); break;
                case DDC_DATE:       delete    static_cast <QDate               *> (workObjectNode->object); break;
                case DDC_DATETIME:   delete    static_cast <QDateTime           *> (workObjectNode->object); break;
                case DDC_BYTES:      delete    static_cast <QByteArray          *> (workObjectNode->object); break;
                case DDC_STRING:     delete    static_cast <QString             *> (workObjectNode->object); break;
                case DDC_LIST:       delete    static_cast <DDList              *> (workObjectNode->object); break;
                case DDC_A_BYTE:     delete    static_cast <QVector <char>      *> (workObjectNode->object); break;
                case DDC_A_SHORT:    delete    static_cast <QVector <short>     *> (workObjectNode->object); break;
                case DDC_A_FLOAT:
                case DDC_A_INT:      delete    static_cast <QVector <int>       *> (workObjectNode->object); break;
                case DDC_A_TIME:     delete    static_cast <QVector <QTime>     *> (workObjectNode->object); break;
                case DDC_A_DATE:     delete    static_cast <QVector <QDate>     *> (workObjectNode->object); break;
                case DDC_A_DATETIME: delete    static_cast <QVector <QDateTime> *> (workObjectNode->object); break;
                case DDC_A_BYTES:    delete    static_cast <QVector <QByteArray>*> (workObjectNode->object); break;
                case DDC_A_STRING:   delete    static_cast <QVector <QString>   *> (workObjectNode->object); break;
                default:             delete    static_cast <QVector <qint64>    *> (workObjectNode->object);           // Array [long, double, number, s_number]
        }

        delete workObjectNode;
    }
}

//  #=====#
//  # get #
//  #=====#

int DDManager::getDataCount       () {return dataCount;}
int DDManager::getPropertiesCount () {return propertiesCount;}

char                  DDManager::getByte    (int id) {return static_cast <char>  (reinterpret_cast <qint64> (getData (id, DDC_BYTE)));}
short                 DDManager::getShort   (int id) {return static_cast <short> (reinterpret_cast <qint64> (getData (id, DDC_SHORT)));}
int                   DDManager::getInt     (int id) {return static_cast <int>   (reinterpret_cast <qint64> (getData (id, DDC_INT)));}

qint64                DDManager::getLong              (int id) {return  reinterpret_cast <qint64>           (getData (id, DDC_LONG));}
qint64                DDManager::getNumber            (int id) {return  reinterpret_cast <qint64>           (getData (id, DDC_NUMBER));}
qint64                DDManager::getSignedNumber      (int id) {return  reinterpret_cast <qint64>           (getData (id, DDC_S_NUMBER));}

QTime                &DDManager::getTime              (int id) {return *static_cast <QTime     *>           (getData (id, DDC_TIME));}
QDate                &DDManager::getDate              (int id) {return *static_cast <QDate     *>           (getData (id, DDC_DATE));}
QDateTime            &DDManager::getDateTime          (int id) {return *static_cast <QDateTime *>           (getData (id, DDC_DATETIME));}
QByteArray           &DDManager::getBytes             (int id) {return *static_cast <QByteArray*>           (getData (id, DDC_BYTES));}
QString              &DDManager::getString            (int id) {return *static_cast <QString   *>           (getData (id, DDC_STRING));}
DDList               &DDManager::getList              (int id) {return *static_cast <DDList    *>           (getData (id, DDC_LIST));}

QVector <char>       &DDManager::getByteArray         (int id) {return *static_cast <QVector <char>      *> (getData (id, DDC_A_BYTE));}
QVector <short>      &DDManager::getShortArray        (int id) {return *static_cast <QVector <short>     *> (getData (id, DDC_A_SHORT));}
QVector <int>        &DDManager::getIntArray          (int id) {return *static_cast <QVector <int>       *> (getData (id, DDC_A_INT));}
QVector <qint64>     &DDManager::getLongArray         (int id) {return *static_cast <QVector <qint64>    *> (getData (id, DDC_A_LONG));}
QVector <float>      &DDManager::getFloatArray        (int id) {return *static_cast <QVector <float>     *> (getData (id, DDC_A_FLOAT));}
QVector <double>     &DDManager::getDoubleArray       (int id) {return *static_cast <QVector <double>    *> (getData (id, DDC_A_DOUBLE));}
QVector <qint64>     &DDManager::getNumberArray       (int id) {return *static_cast <QVector <qint64>    *> (getData (id, DDC_A_NUMBER));}
QVector <qint64>     &DDManager::getSignedNumberArray (int id) {return *static_cast <QVector <qint64>    *> (getData (id, DDC_A_S_NUMBER));}
QVector <QTime>      &DDManager::getTimeArray         (int id) {return *static_cast <QVector <QTime>     *> (getData (id, DDC_A_TIME));}
QVector <QDate>      &DDManager::getDateArray         (int id) {return *static_cast <QVector <QDate>     *> (getData (id, DDC_A_DATE));}
QVector <QDateTime>  &DDManager::getDateTimeArray     (int id) {return *static_cast <QVector <QDateTime> *> (getData (id, DDC_A_DATETIME));}
QVector <QByteArray> &DDManager::getBytesArray        (int id) {return *static_cast <QVector <QByteArray>*> (getData (id, DDC_A_BYTES));}
QVector <QString>    &DDManager::getStringArray       (int id) {return *static_cast <QVector <QString>   *> (getData (id, DDC_A_STRING));}

float DDManager::getFloat (int id)
{
    union {float fv; int iv;};
    iv  =  static_cast <int> (reinterpret_cast <qint64> (getData (id, DDC_FLOAT)));
    return fv;
}

double DDManager::getDouble (int id)
{
    union {double dv; qint64 lv;};
    lv  =  reinterpret_cast <qint64> (getData (id, DDC_DOUBLE));
    return dv;
}

//  +---------+
//  | getData |
//  +---------+

void *DDManager::getData (int id, int type)
{
    if (id < 0 || id > 4095) DDException (DDE_INVALID_ID, id);

    if (dataTypes [id] == DDC_NULL)
    {
        if (dataDefault)
        {
            DDProps *props = static_cast <DDProps *> (seekObject (id, propertiesRoot));
            if (props && props->def) return (type & 0x0F) >= DDC_TIME ? props->def : *static_cast <void **> (props->def);    // Default
        }

        DDException (DDE_DATA_NOT_FOUND, id);
    }

    void *data = seekObject (id, dataRoot);

    if (dataCheck && !checked [id])
    {
        DDProps *props = static_cast <DDProps *> (seekObject (id, propertiesRoot));
        if (!props) DDException (DDE_PROPERTIES_NOT_FOUND, id);

        checkData (type, data, props);
        checked [id] = true;
    }

    if (type != dataTypes [id]) unexpectedData (id, dataTypes [id], type);
    return data;
}

//  #===============#
//  # getProperties #
//  #===============#

DDProps *DDManager::getProperties (int id)
{
    if (id < 0 || id > 4095) DDException (DDE_INVALID_ID, id);
    return static_cast <DDProps*> (seekObject (id, propertiesRoot));
}

//  #====#
//  # is #
//  #====#

bool DDManager::isDataCheck   () {return dataCheck;}
bool DDManager::isDataDefault () {return dataDefault;}

//  #===============#
//  # isDataPresent #
//  #===============#

bool DDManager::isDataPresent (int id)
{
    if (id < 0 || id > 4095) DDException (DDE_INVALID_ID, id);

    DDProps *props = static_cast <DDProps *> (seekObject (id, propertiesRoot));
    return   dataTypes [id] != DDC_NULL   || (dataDefault &&  props && props->def);
}

//  #=====#
//  # set #
//  #=====#

void DDManager::setDataCheck   (bool dataCheck)   {this->dataCheck   = dataCheck;}
void DDManager::setDataDefault (bool dataDefault) {this->dataDefault = dataDefault;}

void DDManager::setByte              (int id, char   data) {setData (id, reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_BYTE);}
void DDManager::setShort             (int id, short  data) {setData (id, reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_SHORT);}
void DDManager::setInt               (int id, int    data) {setData (id, reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_INT);}

void DDManager::setLong              (int id, qint64 data) {setData (id, reinterpret_cast <void *> (data), DDC_LONG);}
void DDManager::setSignedNumber      (int id, qint64 data) {setData (id, reinterpret_cast <void *> (data), DDC_S_NUMBER);}

void DDManager::setBytes             (int id, QByteArray           &data) {setData (id, &data, DDC_BYTES);}
void DDManager::setString            (int id, QString              &data) {setData (id, &data, DDC_STRING);}
void DDManager::setList              (int id, DDList               &data) {setData (id, &data, DDC_LIST);}

void DDManager::setByteArray         (int id, QVector <char>       &data) {setData (id, &data, DDC_A_BYTE);}
void DDManager::setShortArray        (int id, QVector <short>      &data) {setData (id, &data, DDC_A_SHORT);}
void DDManager::setIntArray          (int id, QVector <int>        &data) {setData (id, &data, DDC_A_INT);}
void DDManager::setLongArray         (int id, QVector <qint64>     &data) {setData (id, &data, DDC_A_LONG);}
void DDManager::setFloatArray        (int id, QVector <float>      &data) {setData (id, &data, DDC_A_FLOAT);}
void DDManager::setDoubleArray       (int id, QVector <double>     &data) {setData (id, &data, DDC_A_DOUBLE);}
void DDManager::setSignedNumberArray (int id, QVector <qint64>     &data) {setData (id, &data, DDC_A_S_NUMBER);}
void DDManager::setBytesArray        (int id, QVector <QByteArray> &data) {setData (id, &data, DDC_A_BYTES);}
void DDManager::setStringArray       (int id, QVector <QString>    &data) {setData (id, &data, DDC_A_STRING);}

void DDManager::setFloat (int id, float data)
{
    union {float fv; int iv;}; fv = data;
    setData (id, reinterpret_cast <void *> (static_cast <qint64> (iv)), DDC_FLOAT);
}

void DDManager::setDouble (int id, double data)
{
    union {double dv; qint64 lv;}; dv = data;
    setData (id, reinterpret_cast <void *> (lv), DDC_DOUBLE);
}

void DDManager::setNumber (int id, qint64 data)
{
    if (data < 0) DDException (DDE_INVALID_DATA, id);
    setData (id, reinterpret_cast <void *> (data), DDC_NUMBER);
}

void DDManager::setTime (int id, QTime &data)
{
    if (!static_cast <QTime> (data).isValid ()) DDException (DDE_INVALID_DATA, id);
    setData (id, &data, DDC_TIME);
}

void DDManager::setDate (int id, QDate &data)
{
    if (!static_cast <QDate> (data).isValid ()) DDException (DDE_INVALID_DATA, id);
    setData (id, &data, DDC_DATE);
}

void DDManager::setDateTime (int id, QDateTime &data)
{
    if (!static_cast <QDateTime> (data).isValid ()) DDException (DDE_INVALID_DATA, id);
    setData (id, &data, DDC_DATETIME);
}

void DDManager::setNumberArray (int id, QVector <qint64> &data)
{
    for (int i = 0; i < data.size (); i++)
        if (data.at (i) < 0) DDException (DDE_INVALID_DATA, id);

    setData (id, &data, DDC_A_NUMBER);
}

void DDManager::setTimeArray (int id, QVector <QTime> &data)
{
    for (int i = 0; i < data.size (); i++)
        if (!data.at (i).isNull () && !data.at (i).isValid ()) DDException (DDE_INVALID_DATA, id);

    setData (id, &data, DDC_A_TIME);
}

void DDManager::setDateArray (int id, QVector <QDate> &data)
{
    for (int i = 0; i < data.size (); i++)
        if (!data.at (i).isNull () && !data.at (i).isValid ()) DDException (DDE_INVALID_DATA, id);

    setData (id, &data, DDC_A_DATE);
}

void DDManager::setDateTimeArray (int id, QVector <QDateTime> &data)
{
    for (int i = 0; i < data.size (); i++)
        if (!data.at (i).isNull () && !data.at (i).isValid ()) DDException (DDE_INVALID_DATA, id);

    setData (id, &data, DDC_A_DATETIME);
}

//  +---------+
//  | setData |
//  +---------+

void DDManager::setData (int id, void *data, int type)
{
    if (id < 0 || id > 4095) DDException (DDE_INVALID_ID, id);
    storeData (id, data, type);
}

//  #===============#
//  # setProperties #
//  #===============#

void DDManager::setProperties (DDProps &properties)
{
    void **propsTable = linkObject (properties.id, propertiesRoot);
    if (propsTable [properties.id & 0x0F]) DDException (DDE_PROPERTIES_ALREADY_PRESENT, properties.id);

    propsTable [properties.id & 0x0F] = &properties;
    propertiesCount++;
}

void DDManager::setProperties (QVector <DDProps> &properties)
    {for (int i = 0; i < properties.size (); i++) setProperties (properties [i]);}

//  +--------+
//  | addNew |
//  +--------+

void DDManager::addNew (void *data)
{
    DDBuffer::ObjectNode *newNode = new DDBuffer::ObjectNode ();
    newNode->object               = data;
    newNode->type                 = 0x100;
    newNode->next                 = newObjectList;
    newObjectList                 = newNode;
}

//  +----------------+
//  | unexpectedData |
//  +----------------+

[[noreturn]] void DDManager::unexpectedData (int id, int expected, int requested)
{
    char expectedType  [32];
    char requestedType [32];

    int  expectedIdH  = expected  >> 4;
    int  requestedIdH = requested >> 4;
    int  i, j;

    if (!expectedIdH) i = 0;
    else i = snprintf (expectedType,  32, "%s", DDBuffer::STRING_TYPES [expectedIdH]);

    if (!requestedIdH) j = 0;
    else j = snprintf (requestedType, 32, "%s", DDBuffer::STRING_TYPES [requestedIdH]);

    snprintf (expectedType  + i, 32 - static_cast <unsigned> (i), "%s",DDBuffer::STRING_TYPES [expected  & 0x0F]);
    snprintf (requestedType + j, 32 - static_cast <unsigned> (j), "%s",DDBuffer::STRING_TYPES [requested & 0x0F]);

    DDException (DDE_INVALID_DATA_TYPE, id, expectedType, requestedType);
}

//  +-----------+
//  | checkData |
//  +-----------+

void DDManager::checkData (int type, void *data, DDProps *props)    // return true if is default else false
{
    int dataType = type & 0x0F;

    switch (dataType)
    {
        case DDC_BYTE:     checkByte     (dataType, static_cast <char>  (reinterpret_cast <qint64> (data)), props); return;
        case DDC_SHORT:    checkShort    (dataType, static_cast <short> (reinterpret_cast <qint64> (data)), props); return;
        case DDC_INT:      checkInt      (dataType, static_cast <int>   (reinterpret_cast <qint64> (data)), props); return;
        case DDC_NUMBER:
        case DDC_S_NUMBER:
        case DDC_LONG:     checkLong     (dataType,                      reinterpret_cast <qint64> (data),  props); return;
        case DDC_FLOAT:    checkFloat    (dataType, static_cast <int>   (reinterpret_cast <qint64> (data)), props); return;
        case DDC_DOUBLE:   checkDouble   (dataType,                      reinterpret_cast <qint64> (data),  props); return;

        case DDC_TIME:     checkTime     (dataType, static_cast <QTime     *> (data), props); return;
        case DDC_DATE:     checkDate     (dataType, static_cast <QDate     *> (data), props); return;
        case DDC_DATETIME: checkDateTime (dataType, static_cast <QDateTime *> (data), props); return;
        case DDC_BYTES:    checkBytes    (dataType, static_cast <QByteArray*> (data), props); return;
        case DDC_STRING:   checkString   (dataType, static_cast <QString   *> (data), props); return;

        case DDC_ARRAY:    checkArray    (type, data, props); return;

        default:    // case DDC_LIST:
        {
            if ((props->operType & 0x0F) != DDC_LIST) unexpectedData (props->id, props->operType & 0x0F, DDC_LIST);

            DDList *value = static_cast <DDList *> (data);
            int     i     = value->getDataCount ();

            if (props->number > 0 && props->number != i) DDException (DDE_INVALID_STRUCT_SIZE, "list", props->id, i,  props->number);
            if (props->number < 0 && props->number > -i) DDException (DDE_INVALID_STRUCT_SIZE, "list", props->id, i, -props->number);

            DDProps           *itemProps;
            QVector <DDProps> *propsTable = static_cast <QVector <DDProps> *> (props->inOrProps);
            int                j, k       = propsTable->size ();
            void              *obj;

            value->begin ();

            while (!value->isEndOfList ())
            {
                obj = value->getData (-1);
                i   = value->getCurrentDataId   ();
                j   = value->getCurrentDataType ();

                itemProps = &propsTable->data () [i % k];

                dataType = j & 0x0F;
                value->next ();

                switch (dataType)
                {
                    default:           checkLong     (dataType,                      reinterpret_cast <qint64> (obj),  itemProps); continue;    // long, number, s_number
                    case DDC_BYTE:     checkByte     (dataType, static_cast <char>  (reinterpret_cast <qint64> (obj)), itemProps); continue;
                    case DDC_SHORT:    checkShort    (dataType, static_cast <short> (reinterpret_cast <qint64> (obj)), itemProps); continue;
                    case DDC_INT:      checkInt      (dataType, static_cast <int>   (reinterpret_cast <qint64> (obj)), itemProps); continue;
                    case DDC_FLOAT:    checkFloat    (dataType, static_cast <int>   (reinterpret_cast <qint64> (obj)), itemProps); continue;
                    case DDC_DOUBLE:   checkDouble   (dataType,                      reinterpret_cast <qint64> (obj),  itemProps); continue;

                    case DDC_TIME:     checkTime     (dataType, static_cast <QTime     *> (obj), itemProps); continue;
                    case DDC_DATE:     checkDate     (dataType, static_cast <QDate     *> (obj), itemProps); continue;
                    case DDC_DATETIME: checkDateTime (dataType, static_cast <QDateTime *> (obj), itemProps); continue;
                    case DDC_BYTES:    checkBytes    (dataType, static_cast <QByteArray*> (obj), itemProps); continue;
                    case DDC_STRING:   checkString   (dataType, static_cast <QString   *> (obj), itemProps); continue;

                    case DDC_ARRAY:    checkArray    (j, obj, itemProps); continue;
                    case DDC_NULL:;
                }
            }

            return;
        }
    }
}

//  +-------------+
//  | checkCommon |
//  +-------------+

bool DDManager::checkCommon (int type, qint64 value, DDProps *props)
{
    if (type >= 0 && type != (props->operType & 0x0F)) unexpectedData (props->id, props->operType & 0x0F, type);
    if (props->inOrProps) return false;

    if ((props->operType & DDC_CHKMIN) && value < props->min)
         DDException (DDE_CHECK_MIN, (type == DDC_BYTES || type == DDC_STRING ? "Size" : "Value"),
                      props->id, props->name, value, props->min);

    if ((props->operType & DDC_CHKMAX) && value > props->max)
         DDException (DDE_CHECK_MAX, (type == DDC_BYTES || type == DDC_STRING ? "Size" : "Value"),
                      props->id, props->name, value, props->max);
    return true;
}

//  +-----------+
//  | checkByte |
//  +-----------+

void DDManager::checkByte (int type, char value, DDProps *props)
{
    if (checkCommon (type, value, props)) return;

    for (int i = 0; i < static_cast <QVector <char> *> (props->inOrProps)->size (); i++)
         if (value   == static_cast <QVector <char> *> (props->inOrProps)->at  (i)) return;

    DDException (DDE_CHECK_IN, props->id, props->name, static_cast <qint64> (value));
}

//  +------------+
//  | checkShort |
//  +------------+

void DDManager::checkShort (int type, short value, DDProps *props)
{
    if (checkCommon (type, value, props)) return;

    for (int i = 0; i < static_cast <QVector <short> *> (props->inOrProps)->size (); i++)
         if (value   == static_cast <QVector <short> *> (props->inOrProps)->at  (i)) return;

    DDException (DDE_CHECK_IN, props->id, props->name, static_cast <qint64> (value));
}

//  +----------+
//  | checkInt |
//  +----------+

void DDManager::checkInt (int type, int value, DDProps *props)
{
    if (checkCommon (type, value, props)) return;

    for (int i = 0; i < static_cast <QVector <int> *> (props->inOrProps)->size (); i++)
         if (value   == static_cast <QVector <int> *> (props->inOrProps)->at  (i)) return;

    DDException (DDE_CHECK_IN, props->id, props->name, static_cast <qint64> (value));
}

//  +-----------+
//  | checkLong |
//  +-----------+

void DDManager::checkLong (int type, qint64 value, DDProps *props)
{
    if (checkCommon (type, value, props)) return;

    for (int i = 0; i < static_cast <QVector <qint64> *> (props->inOrProps)->size (); i++)
         if (value   == static_cast <QVector <qint64> *> (props->inOrProps)->at  (i)) return;

    DDException (DDE_CHECK_IN, props->id, props->name, value);
}

//  +------------+
//  | checkFloat |
//  +------------+

void DDManager::checkFloat (int type, int value, DDProps *props)
{
    if (type >= 0 && type != (props->operType & 0x0F)) unexpectedData (props->id, props->operType & 0x0F, type);

    union {float fv1; int iv1;};
    union {float fv2; int iv2;};

    iv1 = value;

    if (!props->inOrProps)
    {
        if ((props->operType & DDC_CHKMIN))
        {
            iv2 = static_cast <int> (props->min);
            if (fv1 < fv2) DDException (DDE_CHECK_MIN_FLOAT, props->id, props->name, static_cast <double> (fv1),
                                        static_cast <qint64> (iv1), static_cast <double> (fv2), static_cast <qint64> (iv2));
        }

        if ((props->operType & DDC_CHKMAX))
        {
            iv2 = static_cast <int> (props->max);
            if (fv1 > fv2) DDException (DDE_CHECK_MAX_FLOAT, props->id, props->name, static_cast <double> (fv1),
                                        static_cast <qint64> (iv1), static_cast <double> (fv2), static_cast <qint64> (iv2));
        }

        return;
    }

    for (int i = 0; i < static_cast <QVector <int> *> (props->inOrProps)->size (); i++)
         if (iv1     == static_cast <QVector <int> *> (props->inOrProps)->at  (i)) return;

    DDException (DDE_CHECK_IN_FLOAT, props->id, props->name, static_cast <double> (fv1), static_cast <qint64> (iv1));
}

//  +-------------+
//  | checkDouble |
//  +-------------+

void DDManager::checkDouble (int type, qint64 value, DDProps *props)
{
    if (type >= 0 && type != (props->operType & 0x0F)) unexpectedData (props->id, props->operType & 0x0F, type);

    union {double dv1; qint64 lv1;};
    union {double dv2; qint64 lv2;};

    lv1 = value;

    if (!props->inOrProps)
    {
        if ((props->operType & DDC_CHKMIN))
        {
            lv2 = props->min;
            if (dv1 < dv2) DDException (DDE_CHECK_MIN_FLOAT, props->id, props->name, dv1, lv1, dv2, lv2);
        }

        if ((props->operType & DDC_CHKMAX))
        {
            lv2 = props->max;
            if (dv1 > dv2) DDException (DDE_CHECK_MAX_FLOAT, props->id, props->name, dv1, lv1, dv2, lv2);
        }

        return;
    }

    for (int i = 0; i < static_cast <QVector <qint64> *> (props->inOrProps)->size (); i++)
         if (lv1     == static_cast <QVector <qint64> *> (props->inOrProps)->at  (i)) return;

    DDException (DDE_CHECK_IN_FLOAT, props->id, props->name, dv1, lv1);
}

//  +-----------+
//  | checkTime |
//  +-----------+

void DDManager::checkTime (int type, QTime *value, DDProps *props)
{
    if (!value->isNull ())
    {
        qint64 time = value->hour   () * 10000000 +
                      value->minute () *   100000 +
                      value->second () *     1000 +
                      value->msec   ();

        if (checkCommon (type, time, props)) return;

        for (int i = 0; i < static_cast <QVector <QTime> *> (props->inOrProps)->size (); i++)
             if (*value  == static_cast <QVector <QTime> *> (props->inOrProps)->at  (i)) return;
    }
    else
        if (!(props->operType & DDC_CHKLIM))
        {
            if (!props->inOrProps) return;
            else for (int i = 0; i < static_cast <QVector <QTime> *> (props->inOrProps)->size (); i++)
                      if (static_cast <QVector <QTime> *> (props->inOrProps)->at (i).isNull ()) return;
        }

    DDException (DDE_CHECK_IN_STRING, props->id, props->name,
                (value->isNull () ? "null" : value->toString ("HH:mm:ss.zzz").toUtf8 ().data ()));
}

//  +-----------+
//  | checkDate |
//  +-----------+

void DDManager::checkDate (int type, QDate *value, DDProps *props)
{
    if (!value->isNull ())
    {
        qint64 date = value->year  () * 10000 +
                      value->month () *   100 +
                      value->day   ();

        if (checkCommon (type, date, props)) return;

        for (int i = 0; i < static_cast <QVector <QDate> *> (props->inOrProps)->size (); i++)
             if (*value  == static_cast <QVector <QDate> *> (props->inOrProps)->at  (i)) return;
    }
    else
        if (!(props->operType & DDC_CHKLIM))
        {
            if (!props->inOrProps) return;
            else for (int i = 0; i < static_cast <QVector <QDate> *> (props->inOrProps)->size (); i++)
                      if (static_cast <QVector <QDate> *> (props->inOrProps)->at (i).isNull ()) return;
        }

    DDException (DDE_CHECK_IN_STRING, props->id, props->name,
                (value->isNull () ? "null" : value->toString ("yyyy-MM-dd").toUtf8 ().data ()));
}

//  +---------------+
//  | checkDateTime |
//  +---------------+

void DDManager::checkDateTime (int type, QDateTime *value, DDProps *props)
{
    if (!value->isNull ())
    {
        qint64 datetime = value->date ().year   () * 10000000000000 +
                          value->date ().month  () *   100000000000 +
                          value->date ().day    () *     1000000000 +
                          value->time ().hour   () *       10000000 +
                          value->time ().minute () *         100000 +
                          value->time ().second () *           1000 +
                          value->time ().msec   ();

        if (checkCommon (type, datetime, props)) return;

        for (int i = 0; i < static_cast <QVector <QDateTime> *> (props->inOrProps)->size (); i++)
             if (*value  == static_cast <QVector <QDateTime> *> (props->inOrProps)->at  (i)) return;
    }
    else
        if (!(props->operType & DDC_CHKLIM))
        {
            if (!props->inOrProps) return;
            else for (int i = 0; i < static_cast <QVector <QDateTime> *> (props->inOrProps)->size (); i++)
                      if (static_cast <QVector <QDateTime> *> (props->inOrProps)->at (i).isNull ()) return;
        }

    DDException (DDE_CHECK_IN_STRING, props->id, props->name,
                (value->isNull () ? "null" : value->toString ("yyyy-MM-dd_HH:mm:ss.zzz").toUtf8 ().data ()));
}

//  +------------+
//  | checkBytes |
//  +------------+

void DDManager::checkBytes (int type, QByteArray *value, DDProps *props)
{
    if (!value->isNull ())
    {
        if (checkCommon (type, value->size (), props)) return;

        for (int i = 0; i < static_cast <QVector <QByteArray> *> (props->inOrProps)->size (); i++)
             if (*value  == static_cast <QVector <QByteArray> *> (props->inOrProps)->at  (i)) return;
    }
    else
        if (!(props->operType & DDC_CHKLIM))
        {
            if (!props->inOrProps) return;
            else for (int i = 0; i < static_cast <QVector <QByteArray> *> (props->inOrProps)->size (); i++)
                      if (static_cast <QVector <QByteArray> *> (props->inOrProps)->at (i).isNull ()) return;
        }

    DDException (DDE_CHECK_IN_STRING, props->id, props->name, (value->isNull () ? "null" : value->toHex ().data ()));
}

//  +-------------+
//  | checkString |
//  +-------------+

void DDManager::checkString (int type, QString *value, DDProps *props)
{
    if (!value->isNull ())
    {
        if (checkCommon (type, value->size (), props)) return;

        for (int i = 0; i < static_cast <QVector <QString> *> (props->inOrProps)->size (); i++)
             if (*value  == static_cast <QVector <QString> *> (props->inOrProps)->at  (i)) return;
    }
    else
        if (!(props->operType & DDC_CHKLIM))
        {
            if (!props->inOrProps) return;
            else for (int i = 0; i < static_cast <QVector <QString> *> (props->inOrProps)->size (); i++)
                      if (static_cast <QVector <QString> *> (props->inOrProps)->at (i).isNull ()) return;
        }

    DDException (DDE_CHECK_IN_STRING, props->id, props->name, (value->isNull () ? "null" : value->toUtf8 ().data ()));
}

//  +------------+
//  | checkArray |
//  +------------+

void DDManager::checkArray (int type, void *data, DDProps *props)
{
    if (type != (props->operType & 0xFF)) unexpectedData (props->id, props->operType & 0xFF, type);

    int len = static_cast <QVector <int> *> (data)->size ();

    if (props->number > 0 && props->number != len) DDException (DDE_INVALID_STRUCT_SIZE, "array", props->id, len,  props->number);
    if (props->number < 0 && props->number > -len) DDException (DDE_INVALID_STRUCT_SIZE, "array", props->id, len, -props->number);

    int i = 0;

    switch (type >> 4)
    {
        case DDC_BYTE:      for (; i < len; i++) checkByte     (-1,  static_cast <QVector <char>      *> (data)->data () [i], props); break;
        case DDC_SHORT:     for (; i < len; i++) checkShort    (-1,  static_cast <QVector <short>     *> (data)->data () [i], props); break;
        case DDC_INT:       for (; i < len; i++) checkInt      (-1,  static_cast <QVector <int>       *> (data)->data () [i], props); break;
        case DDC_NUMBER:
        case DDC_S_NUMBER:
        case DDC_LONG:      for (; i < len; i++) checkLong     (-1,  static_cast <QVector <qint64>    *> (data)->data () [i], props); break;
        case DDC_FLOAT:     for (; i < len; i++) checkFloat    (-1,  static_cast <QVector <int>       *> (data)->data () [i], props); break;
        case DDC_DOUBLE:    for (; i < len; i++) checkDouble   (-1,  static_cast <QVector <qint64>    *> (data)->data () [i], props); break;
        case DDC_TIME:      for (; i < len; i++) checkTime     (-1, &static_cast <QVector <QTime>     *> (data)->data () [i], props); break;
        case DDC_DATE:      for (; i < len; i++) checkDate     (-1, &static_cast <QVector <QDate>     *> (data)->data () [i], props); break;
        case DDC_DATETIME:  for (; i < len; i++) checkDateTime (-1, &static_cast <QVector <QDateTime> *> (data)->data () [i], props); break;
        case DDC_BYTES:     for (; i < len; i++) checkBytes    (-1, &static_cast <QVector <QByteArray>*> (data)->data () [i], props); break;
        default:            for (; i < len; i++) checkString   (-1, &static_cast <QVector <QString>   *> (data)->data () [i], props);
    }
}

//  +------------------+
//  | checkDataDefault |
//  +------------------+

bool DDManager::checkDataDefault (int type, void *data1, void *data2)
{
    switch (type & 0x0F)
    {
        case DDC_BYTE:      return  static_cast <char>  (reinterpret_cast <qint64> (data1)) == *static_cast <char  *> (data2);
        case DDC_SHORT:     return  static_cast <short> (reinterpret_cast <qint64> (data1)) == *static_cast <short *> (data2);
        case DDC_FLOAT:
        case DDC_INT:       return  static_cast <int>   (reinterpret_cast <qint64> (data1)) == *static_cast <int   *> (data2);
        case DDC_DOUBLE:
        case DDC_NUMBER:
        case DDC_S_NUMBER:
        case DDC_LONG:      return  reinterpret_cast <qint64> (data1) == *static_cast <qint64    *> (data2);

        case DDC_TIME:      return *static_cast <QTime     *> (data1) == *static_cast <QTime     *> (data2);
        case DDC_DATE:      return *static_cast <QDate     *> (data1) == *static_cast <QDate     *> (data2);
        case DDC_DATETIME:  return *static_cast <QDateTime *> (data1) == *static_cast <QDateTime *> (data2);

        case DDC_BYTES:     return  static_cast <QByteArray*> (data1)->size () ==  static_cast <QByteArray *> (data2)->size () &&
                                   *static_cast <QByteArray*> (data1)          == *static_cast <QByteArray *> (data2);

        case DDC_STRING:    return  static_cast <QString   *> (data1)->size () ==  static_cast <QString    *> (data2)->size () &&
                                   *static_cast <QString   *> (data1)          == *static_cast <QString    *> (data2);

        case DDC_ARRAY:     return  checkArrayDefault (type, data1, data2);

        default:    // case DDC_LIST:
        {
            DDList *list    = static_cast <DDList *> (data1);
            DDList *listDef = static_cast <DDList *> (data2);

            if (list->getDataCount () != static_cast <DDList *> (listDef)->getDataCount ()) return false;

            void *obj, *objDef;
            int   i,    j;

            list->begin    ();
            listDef->begin ();

            while (!list->isEndOfList ())
            {
                obj    = list->getData    (-1);
                objDef = listDef->getData (-1);
                i      = list->getCurrentDataType    ();
                j      = listDef->getCurrentDataType ();

                list->next   ();
                listDef->next ();

                if (i == DDC_NULL || j == DDC_NULL)
                {
                    if (!obj && !objDef) continue;
                    else return false;
                }

                if (i != j) return false;

                switch (i & 0x0F)
                {
                    default:            if (reinterpret_cast <qint64> (obj) ==
                                            reinterpret_cast <qint64> (objDef)) continue; else return false;    // long, double, number, s_number

                    case DDC_BYTE:      if (static_cast <char>  (reinterpret_cast <qint64> (obj)) ==
                                            static_cast <char>  (reinterpret_cast <qint64> (objDef))) continue; else return false;

                    case DDC_SHORT:     if (static_cast <short> (reinterpret_cast <qint64> (obj)) ==
                                            static_cast <short> (reinterpret_cast <qint64> (objDef))) continue; else return false;
                    case DDC_FLOAT:
                    case DDC_INT:       if (static_cast <int>   (reinterpret_cast <qint64> (obj)) ==
                                            static_cast <int>   (reinterpret_cast <qint64> (objDef))) continue; else return false;

                    case DDC_TIME:      if (*static_cast <QTime     *> (obj) ==
                                            *static_cast <QTime     *> (objDef)) continue; else return false;

                    case DDC_DATE:      if (*static_cast <QDate     *> (obj) ==
                                            *static_cast <QDate     *> (objDef)) continue; else return false;

                    case DDC_DATETIME:  if (*static_cast <QDateTime *> (obj) ==
                                            *static_cast <QDateTime *> (objDef)) continue; else return false;

                    case DDC_BYTES:     if (*static_cast <QByteArray*> (obj) ==
                                            *static_cast <QByteArray*> (objDef)) continue; else return false;

                    case DDC_STRING:    if (*static_cast <QString   *> (obj) ==
                                            *static_cast <QString   *> (objDef)) continue; else return false;

                    case DDC_ARRAY:     if (!checkArrayDefault (i, obj, objDef)) return false;
                }
            }

            return true;
        }
    }
}

//  +-------------------+
//  | checkArrayDefault |
//  +-------------------+

bool DDManager::checkArrayDefault (int type, void *array1, void *array2)
{
    int len  = static_cast <QVector <int> *> (array1)->size ();
    if (len != static_cast <QVector <int> *> (array2)->size ()) return false;

    switch (type >> 4)
    {
        default:        return !memcmp (static_cast <QVector <qint64>*> (array1)->data (),
                                        static_cast <QVector <qint64>*> (array2)->data (), static_cast <size_t> (len));

        case DDC_BYTE:  return !memcmp (static_cast <QVector <char>  *> (array1)->data (),
                                        static_cast <QVector <char>  *> (array2)->data (), static_cast <size_t> (len));

        case DDC_SHORT: return !memcmp (static_cast <QVector <short> *> (array1)->data (),
                                        static_cast <QVector <short> *> (array2)->data (), static_cast <size_t> (len));
        case DDC_FLOAT:
        case DDC_INT:   return !memcmp (static_cast <QVector <int>   *> (array1)->data (),
                                        static_cast <QVector <int>   *> (array2)->data (), static_cast <size_t> (len));
        case DDC_TIME:

            for (int i = 0; i < len; i++)
                if (static_cast <QVector <QTime> *> (array1)->data () [i]  !=
                    static_cast <QVector <QTime> *> (array1)->data () [i]) return false;

            return true;

        case DDC_DATE:

            for (int i = 0; i < len; i++)
                if (static_cast <QVector <QDate> *> (array1)->data () [i]  !=
                    static_cast <QVector <QDate> *> (array2)->data () [i]) return false;

            return true;

        case DDC_DATETIME:

            for (int i = 0; i < len; i++)
                if (static_cast <QVector <QDateTime> *> (array1)->data () [i]  !=
                    static_cast <QVector <QDateTime> *> (array2)->data () [i]) return false;

            return true;

        case DDC_BYTES:

            for (int i = 0; i < len; i++)
                if (static_cast <QVector <QByteArray> *> (array1)->data () [i]  !=
                    static_cast <QVector <QByteArray> *> (array2)->data () [i]) return false;

            return true;

        case DDC_STRING:

            for (int i = 0; i < len; i++)
                if (static_cast <QVector <QString> *> (array1)->data () [i]  !=
                    static_cast <QVector <QString> *> (array2)->data () [i]) return false;

            return true;
    }
}

//  #========#
//  # decode #
//  #========#

void DDManager::decode (QByteArray &buffer)
    {decode (buffer, 0, buffer.size (), true, true);}

void DDManager::decode (QByteArray &buffer, int offset)
    {decode (buffer, offset, buffer.size (), true, true);}

void DDManager::decode (QByteArray &buffer, bool checkCRC32)
    {decode (buffer, 0, buffer.size (), true, checkCRC32);}

void DDManager::decode (QByteArray &buffer, bool checkProtocolInfo, bool checkCRC32)
    {decode (buffer, 0, buffer.size (), checkProtocolInfo, checkCRC32);}

void DDManager::decode (QByteArray &buffer, int offset, bool checkCRC32)
    {decode (buffer, offset, buffer.size (), true, checkCRC32);}

void DDManager::decode (QByteArray &buffer, int offset, bool checkProtocolInfo, bool checkCRC32)
    {decode (buffer, offset, buffer.size (), checkProtocolInfo, checkCRC32);}

void DDManager::decode (QByteArray &buffer, int offset, int length, bool checkProtocolInfo, bool checkCRC32)
{
    if (offset < 0 || length < 0 || offset + length > buffer.size ())
        DDException (DDE_INVALID_DECODE_ARGUMENT, offset, length);

    DDBuffer ddb = DDBuffer (&buffer, offset, length, &newObjectList);

    if (checkProtocolInfo) ddb.checkProtocolInfo ();
    if (checkCRC32)        ddb.checkCRC32        ();

    char type;
    int  id;

    while (!ddb.endOfBuffer ())
    {
        id   = ddb.readTypeAndId ();
        type = static_cast <char> ((id >> 12) & 0x0F);
        storeData (id & 0xFFF, ddb.read (type), type);
    }
}

//  +------------+
//  | linkObject |
//  +------------+

void **DDManager::linkObject (int id, void **obj)
{
    int idx = id >> 8;
    if (!(obj [idx])) {obj [idx] = new void * [16] (); addNew (obj [idx]);}

    obj = static_cast <void **> (obj [idx]);
    idx = (id >> 4) & 0x0F;

    if (!(obj [idx])) {obj [idx] = new void * [16] (); addNew (obj [idx]);}
    return static_cast <void **> (obj [idx]);
}

//  +------------+
//  | seekObject |
//  +------------+

void *DDManager::seekObject (int id, void **obj)
{
    if ((obj = static_cast <void **> (obj [ id >> 8])) &&
        (obj = static_cast <void **> (obj [(id >> 4) & 0x0F]))) return ((static_cast <void **> (obj)) [id & 0x0F]);

    return nullptr;
}

//  +-----------+
//  | storeData |
//  +-----------+

void DDManager::storeData (int id, void *data, int type)
{
    if (dataTypes [id] != DDC_NULL) DDException (DDE_DATA_ALREADY_PRESENT, id);

    if (dataDefault)
    {
        DDProps *props = static_cast <DDProps *> (seekObject (id, propertiesRoot));
        if (props && props->def && !(props->operType & DDC_REQ) &&
            checkDataDefault (type, data, props->def)) return;                         // Default
    }

    dataTypes  [id] = static_cast <unsigned char> (type);
    linkObject (id, dataRoot) [id & 0x0F] = data;
    dataCount++;
}

//  #========#
//  # encode #
//  #========#

void DDManager::encode (QByteArray &buffer)
    {return encode (buffer, 0, 0, true, true);}

void DDManager::encode (QByteArray &buffer, int reservedSizeBefore)
    {return encode (buffer, reservedSizeBefore, 0, true, true);}

void DDManager::encode (QByteArray &buffer, bool addCRC32)
    {return encode (buffer, 0, 0, true, addCRC32);}

void DDManager::encode (QByteArray &buffer, bool addProtocolInfo, bool addCRC32)
    {return encode (buffer, 0, 0, addProtocolInfo, addCRC32);}

void DDManager::encode (QByteArray &buffer, int reservedSizeBefore, bool addCRC32)
    {return encode (buffer, reservedSizeBefore, 0, true, addCRC32);}

void DDManager::encode (QByteArray &buffer, int reservedSizeBefore, bool addProtocolInfo, bool addCRC32)
    {return encode (buffer, reservedSizeBefore, 0, addProtocolInfo, addCRC32);}

void DDManager::encode (QByteArray &buffer, int reservedSizeBefore, int reservedSizeAfter, bool addProtocolInfo, bool addCRC32)
{
    if (reservedSizeBefore < 0 || reservedSizeAfter < 0 || (reservedSizeBefore + reservedSizeAfter > 65535))
        DDException (DDE_INVALID_ENCODE_ARGUMENT, reservedSizeBefore, reservedSizeAfter);

    QByteArray tempBuffer (4096, 0);
    DDBuffer   ddBuffer   (&tempBuffer, 0, 4096, &newObjectList);

    if (addProtocolInfo) ddBuffer.addProtocolInfo ();

    int                 i, j, k, x;
    void               *data;
    void              **objTable1, **objTable2;
    QVector <DDProps>  *propsTable;
    DDList             *list;
    DDProps             p;

    if (propertiesCount)
        for (i = 0; i < 16; i++)    // Check Required Flag
        {
            if (!(objTable1 = static_cast <void **> (propertiesRoot [i]))) continue;

            for (j = 0; j < 16; j++)
            {
                if (!(objTable2 = static_cast <void **> (objTable1 [j]))) continue;

                for (k = 0; k < 16; k++)
                {
                    if (!(data = objTable2 [k])) continue;
                    x =  (i << 8) | (j << 4) | k;

                    if ((static_cast <DDProps *> (data)->operType & DDC_REQ) > 0 && dataTypes [x] == DDC_NULL)
                         DDException (DDE_MISSING_DATA, x, static_cast <DDProps *> (data)->name);

                    if (dataTypes [x] != DDC_LIST || !(propsTable = static_cast <QVector <DDProps> *> (static_cast <DDProps *> (data)->inOrProps)))
                        continue;

                    list = static_cast <DDList *> (seekObject (x, dataRoot));
                    x    = propsTable->size ();

                    list->begin ();

                    while (!list->isEndOfList ())
                    {
                        p = propsTable->data () [list->getCurrentDataId () % x];

                        if ((p.operType & DDC_REQ) != 0 && list->getCurrentDataType () == DDC_NULL)
                             DDException (DDE_MISSING_DATA, p.id, p.name);

                        list->next ();
                    }

                    for (x = list->getDataCount (); x < propsTable->size (); x++)
                        if ((propsTable->data () [x].operType & DDC_REQ) != 0)
                             DDException (DDE_MISSING_DATA, propsTable->data () [x].id, propsTable->data () [x].name);
                }
            }
        }

    DDProps *props;
    int      y;

    for (i = 0; i < 16; i++)
    {
        if (!(objTable1 = static_cast <void **> (dataRoot [i]))) continue;

        for (j = 0; j < 16; j++)
        {
            if (!(objTable2 = static_cast <void **> (objTable1 [j]))) continue;

            for (k = 0; k < 16; k++)
            {
                x = (i << 8) | (j << 4) | k;
                y =  dataTypes [x];

                if (y == DDC_NULL) continue;
                data = objTable2 [k];

                if (dataCheck && !checked [x])
                {
                    props = static_cast <DDProps *> (seekObject (x, propertiesRoot));
                    if (!props) DDException (DDE_PROPERTIES_NOT_FOUND, x);

                    checkData (y, data, props);
                    checked [x] = true;
                }

                ddBuffer.writeTypeAndId (((y & 0x0F) << 12) | x);
                ddBuffer.write (data, y);
            }
        }
    }

    if (addCRC32) ddBuffer.addCRC32 ();

    int offset  =  ddBuffer.getOffset ();
    buffer.resize (offset + reservedSizeBefore + reservedSizeAfter);

    if (offset > 0) memcpy (buffer.data () + reservedSizeBefore, ddBuffer.getBuffer ()->data (), static_cast <size_t> (offset));
}

//  #==========#
//  # toString #
//  #==========#

QString DDManager::toString ()
{
    QString   str;
    bool      sep = false;
    void    **dataTable1, **dataTable2;
    DDProps  *props;
    void     *data;
    int       i, j, k, x;

    QTextStream (&str) << dataCount << '{';

    for (i = 0; i < 16; i++)
    {
        if (!(dataTable1 = static_cast <void **> (dataRoot [i]))) continue;

        for (j = 0, props = nullptr; j < 16; j++)
        {
            if (!(dataTable2 = static_cast <void **> (dataTable1 [j]))) continue;

            for (k = 0; k < 16; k++)
            {
                x = (i << 8) | (j << 4) | k;
                if (dataTypes [x] == DDC_NULL) continue;

                if (sep) QTextStream (&str) << ' ';
                sep = true;

                data = dataTable2 [k];
                if ((props = static_cast <DDProps *> (seekObject (x, propertiesRoot)))) QTextStream (&str) << props->name << '@';

                QTextStream (&str) << x << '=' << DDBuffer::DATA_TYPES [dataTypes [x] & 0x0F];
                DDBuffer::stringData (&str, data, dataTypes [x], props);
            }
        }
    }

    QTextStream (&str) << '}';
    return str;
}
