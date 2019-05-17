//  #======================================#
//  # Dynamic Data Manager - Version 1.0.0 #
//  #                                      #
//  # Author: Marco Vagnoni                #
//  # Email:  marco.vagnoni@yahoo.com      #
//  # Date:   April     2019 (v1.0.0)      #
//  #======================================#

#include <cstring>
#include <QTextStream>

#include "DDManager.h"

#define isLeapYear(year) (!(year % 400) || (!(year % 4) && (year % 100)))

//  #=============#
//  # Constructor #
//  #=============#

DDManager::DDManager ()
{
    memset (checked,        0, sizeof (checked));
    memset (dataRoot,       0, sizeof (dataRoot));
    memset (dataTypes,      0, sizeof (dataTypes));
    memset (propertiesRoot, 0, sizeof (propertiesRoot));

    dataCheck        = true;
    dataDefault      = true;
    dataNumber       = 0;
    propertiesNumber = 0;
    newObjectList    = nullptr;
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

        if (workObjectNode->type > DDC_DATETIME)
            switch (workObjectNode->type)
            {
                case 0x100:        delete [] static_cast <void               **> (workObjectNode->object); break;
                case DDC_BYTES:    delete    static_cast <QByteArray          *> (workObjectNode->object); break;
                case DDC_STRING:   delete    static_cast <QString             *> (workObjectNode->object); break;
                case DDC_LIST:     delete    static_cast <DDList              *> (workObjectNode->object); break;
                case DDC_A_BYTE:   delete    static_cast <QVector <char>      *> (workObjectNode->object); break;
                case DDC_A_SHORT:  delete    static_cast <QVector <short>     *> (workObjectNode->object); break;
                case DDC_A_FLOAT:
                case DDC_A_TIME:
                case DDC_A_DATE:
                case DDC_A_INT:    delete    static_cast <QVector <int>       *> (workObjectNode->object); break;
                case DDC_A_BYTES:  delete    static_cast <QVector <QByteArray>*> (workObjectNode->object); break;
                case DDC_A_STRING: delete    static_cast <QVector <QString>   *> (workObjectNode->object); break;
                default:           delete    static_cast <QVector <qint64>    *> (workObjectNode->object);           // Array [long, double, number, s_number, datetime]
        }

        delete workObjectNode;
    }
}

//  #=====#
//  # get #
//  #=====#

int                   DDManager::getDataNumber        ()       {return dataNumber;}
int                   DDManager::getPropertiesNumber  ()       {return propertiesNumber;}

char                  DDManager::getByte              (int id) {return static_cast <char>  (reinterpret_cast <qint64> (getData (id, DDC_BYTE)));}
short                 DDManager::getShort             (int id) {return static_cast <short> (reinterpret_cast <qint64> (getData (id, DDC_SHORT)));}
int                   DDManager::getInt               (int id) {return static_cast <int>   (reinterpret_cast <qint64> (getData (id, DDC_INT)));}
int                   DDManager::getTime              (int id) {return static_cast <int>   (reinterpret_cast <qint64> (getData (id, DDC_TIME)));}
int                   DDManager::getDate              (int id) {return static_cast <int>   (reinterpret_cast <qint64> (getData (id, DDC_DATE)));}

qint64                DDManager::getLong              (int id) {return  reinterpret_cast <qint64>           (getData (id, DDC_LONG));}
qint64                DDManager::getNumber            (int id) {return  reinterpret_cast <qint64>           (getData (id, DDC_NUMBER));}
qint64                DDManager::getSignedNumber      (int id) {return  reinterpret_cast <qint64>           (getData (id, DDC_S_NUMBER));}
qint64                DDManager::getDateTime          (int id) {return  reinterpret_cast <qint64>           (getData (id, DDC_DATETIME));}

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
QVector <int>        &DDManager::getTimeArray         (int id) {return *static_cast <QVector <int>       *> (getData (id, DDC_A_TIME));}
QVector <int>        &DDManager::getDateArray         (int id) {return *static_cast <QVector <int>       *> (getData (id, DDC_A_DATE));}
QVector <qint64>     &DDManager::getDateTimeArray     (int id) {return *static_cast <QVector <qint64>    *> (getData (id, DDC_A_DATETIME));}
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

    if (type != dataTypes [id] &&
       (type <  DDC_LONG   ||  type > DDC_S_NUMBER   || dataTypes [id] < DDC_LONG   || dataTypes [id] > DDC_S_NUMBER) &&
       (type <  DDC_A_LONG ||  type > DDC_A_S_NUMBER || dataTypes [id] < DDC_A_LONG || dataTypes [id] > DDC_A_S_NUMBER))
            DDBuffer::unexpectedData (id, dataTypes [id], type);

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

void DDManager::getProperties (QVector <DDProps> &properties)
{
    properties.resize (propertiesNumber);
    if (propertiesNumber == 0) return;

    DDProps *props;
    void    *propsTable1, *propsTable2;
    int      i, j, k, x =  0;

    for (i = 0; i < 16; i++)
    {
        if (!(propsTable1 = propertiesRoot [i])) continue;

        for (j = 0; j < 16; j++)
        {
            if (!(propsTable2 = static_cast <void **> (propsTable1) [j])) continue;

            for (k = 0; k < 16; k++)
                if (!(props = static_cast <DDProps **> (propsTable2) [k])) properties.data () [x++] = *props;
        }
    }
}

//  #===================#
//  # getListProperties #
//  #===================#

QVector <DDProps> *DDManager::getListProperties (int id)
{
    if (id < 0 || id > 4095) DDException (DDE_INVALID_ID, id);

    if (dataTypes [id] != DDC_LIST)
        DDException (DDE_INVALID_DATA_TYPE, id, DDBuffer::STRING_TYPES [DDC_LIST], DDBuffer::STRING_TYPES [dataTypes [id]]);

    return static_cast <DDList *> (seekObject (id, dataRoot))->getProperties ();
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

void DDManager::setDataCheck         (bool dataCheck)   {this->dataCheck   = dataCheck;}
void DDManager::setDataDefault       (bool dataDefault) {this->dataDefault = dataDefault;}

void DDManager::setByte              (int id, char   data) {setData (id, reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_BYTE);}
void DDManager::setShort             (int id, short  data) {setData (id, reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_SHORT);}
void DDManager::setInt               (int id, int    data) {setData (id, reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_INT);}

void DDManager::setLong              (int id, qint64 data) {setData (id, reinterpret_cast <void *> (data), DDC_LONG);}
void DDManager::setSignedNumber      (int id, qint64 data) {setData (id, reinterpret_cast <void *> (data), DDC_S_NUMBER);}

void DDManager::setBytes             (int id, const QByteArray           &data) {setData (id, &data, DDC_BYTES);}
void DDManager::setString            (int id, const QString              &data) {setData (id, &data, DDC_STRING);}
void DDManager::setList              (int id, const DDList               &data) {setData (id, &data, DDC_LIST);}

void DDManager::setByteArray         (int id, const QVector <char>       &data) {setData (id, &data, DDC_A_BYTE);}
void DDManager::setShortArray        (int id, const QVector <short>      &data) {setData (id, &data, DDC_A_SHORT);}
void DDManager::setIntArray          (int id, const QVector <int>        &data) {setData (id, &data, DDC_A_INT);}
void DDManager::setLongArray         (int id, const QVector <qint64>     &data) {setData (id, &data, DDC_A_LONG);}
void DDManager::setFloatArray        (int id, const QVector <float>      &data) {setData (id, &data, DDC_A_FLOAT);}
void DDManager::setDoubleArray       (int id, const QVector <double>     &data) {setData (id, &data, DDC_A_DOUBLE);}
void DDManager::setSignedNumberArray (int id, const QVector <qint64>     &data) {setData (id, &data, DDC_A_S_NUMBER);}
void DDManager::setBytesArray        (int id, const QVector <QByteArray> &data) {setData (id, &data, DDC_A_BYTES);}
void DDManager::setStringArray       (int id, const QVector <QString>    &data) {setData (id, &data, DDC_A_STRING);}

void DDManager::setNumber (int id, qint64 data)
    {setData (id, reinterpret_cast <void *> (data), (data < 0 ? DDC_S_NUMBER : DDC_NUMBER));}

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

void DDManager::setTime (int id, int data)
{
    if (!DDBuffer::isValidTime (data)) DDException (DDE_INVALID_DATA, id);
    setData (id, reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_TIME);
}

void DDManager::setDate (int id, int data)
{
    if (!DDBuffer::isValidDate (data)) DDException (DDE_INVALID_DATA, id);
    setData (id, reinterpret_cast <void *> (static_cast <qint64> (data)), DDC_DATE);
}

void DDManager::setDateTime (int id, qint64 data)
{
    if (!DDBuffer::isValidDateTime (data)) DDException (DDE_INVALID_DATA, id);
    setData (id, reinterpret_cast <void *> (data), DDC_DATETIME);
}

void DDManager::setNumberArray (int id, const QVector <qint64> &data)
{
    int type = DDC_A_NUMBER;

    for (int i = 0; i < data.size (); i++)
        if (data.at (i) < 0) {type = DDC_A_S_NUMBER; break;}

    setData (id, &data, type);
}

void DDManager::setTimeArray (int id, const QVector <int> &data)
{
    for (int i = 0; i < data.size (); i++)
        if (!DDBuffer::isValidTime (data.at (i))) DDException (DDE_INVALID_DATA, id);

    setData (id, &data, DDC_A_TIME);
}

void DDManager::setDateArray (int id, const QVector <int> &data)
{
    for (int i = 0; i < data.size (); i++)
        if (!DDBuffer::isValidDate (data.at (i))) DDException (DDE_INVALID_DATA, id);

    setData (id, &data, DDC_A_DATE);
}

void DDManager::setDateTimeArray (int id, const QVector <qint64> &data)
{
    for (int i = 0; i < data.size (); i++)
        if (!DDBuffer::isValidDateTime (data.at (i))) DDException (DDE_INVALID_DATA, id);

    setData (id, &data, DDC_A_DATETIME);
}

//  +---------+
//  | setData |
//  +---------+

void DDManager::setData (int id, const void *data, int type)
{
    if (id < 0 || id > 4095) DDException (DDE_INVALID_ID, id);
    storeData (id, data, type);
}

//  #===============#
//  # setProperties #
//  #===============#

void DDManager::setProperties (DDProps &properties)
{
    const void **propsTable = linkObject (properties.id, propertiesRoot);
    if (propsTable [properties.id & 0x0F]) DDException (DDE_PROPERTIES_ALREADY_PRESENT, properties.id);

    propsTable [properties.id & 0x0F] = &properties;
    propertiesNumber++;
}

void DDManager::setProperties (QVector <DDProps> &properties)
    {for (int i = 0; i < properties.size (); i++) setProperties (properties [i]);}

//  #===================#
//  # setListProperties #
//  #===================#

void DDManager::setListProperties (int id, QVector <DDProps> &properties)
{
    if (id < 0 || id > 4095) DDException (DDE_INVALID_ID, id);

    if (dataTypes [id] != DDC_LIST)
        DDException (DDE_INVALID_DATA_TYPE, id, DDBuffer::STRING_TYPES [DDC_LIST], DDBuffer::STRING_TYPES [dataTypes [id]]);

    static_cast <DDList *> (seekObject (id, dataRoot))->setProperties (properties);
}

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

//  +-----------+
//  | checkData |
//  +-----------+

void DDManager::checkData (int type, void *data, DDProps *props)    // return true if is default else false
{
    int dataType = type & 0x0F;

    switch (dataType)
    {
        default:           checkLong     (dataType, reinterpret_cast <qint64> (data), props); return;
        case DDC_DOUBLE:   checkDouble   (dataType, reinterpret_cast <qint64> (data), props); return;
        case DDC_DATETIME: checkDateTime (dataType, reinterpret_cast <qint64> (data), props); return;

        case DDC_BYTE:     checkByte     (dataType, static_cast <char>  (reinterpret_cast <qint64> (data)), props); return;
        case DDC_SHORT:    checkShort    (dataType, static_cast <short> (reinterpret_cast <qint64> (data)), props); return;
        case DDC_INT:      checkInt      (dataType, static_cast <int>   (reinterpret_cast <qint64> (data)), props); return;
        case DDC_FLOAT:    checkFloat    (dataType, static_cast <int>   (reinterpret_cast <qint64> (data)), props); return;
        case DDC_TIME:     checkTime     (dataType, static_cast <int>   (reinterpret_cast <qint64> (data)), props); return;
        case DDC_DATE:     checkDate     (dataType, static_cast <int>   (reinterpret_cast <qint64> (data)), props); return;

        case DDC_BYTES:    checkBytes    (dataType, static_cast <QByteArray *> (data), props); return;
        case DDC_STRING:   checkString   (dataType, static_cast <QString    *> (data), props); return;

        case DDC_ARRAY:    checkArray    (type, data, props); return;

        case DDC_LIST:
        {
            if ((props->operType & 0x0F) != DDC_LIST) DDBuffer::unexpectedData (props->id, props->operType & 0x0F, DDC_LIST);

            DDList *value = static_cast <DDList *> (data);
            int     i     = value->getDataNumber ();

            if ((props->operType & DDC_CHKMIN) && i < props->min) DDException (DDE_INVALID_STRUCT_SIZE, "list", props->id, i, props->min);
            if ((props->operType & DDC_CHKMAX) && i > props->max) DDException (DDE_INVALID_STRUCT_SIZE, "list", props->id, i, props->max);

            if (props->in)
            {
                for (int x = 0; x < static_cast <QVector <DDList> *> (props->in)->size (); x++)
                     if (compareList (value, &static_cast <QVector <DDList> *> (props->in)->data () [x])) return;

                DDException (DDE_CHECK_IN_STRUCT, props->id, props->name);
            }

            QVector <DDProps> *propsTable = value->getProperties ();
            if (!propsTable) DDException (DDE_LIST_PROPERTIES_NOT_FOUND, props->id);

            DDProps *itemProps;
            int      j, k = propsTable->size ();
            void    *obj;

            value->begin ();

            while (!value->isEndOfList ())
            {
                obj = value->getData (-1);
                i   = value->getCurrentDataOffset ();
                j   = value->getCurrentDataType   ();

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
                    case DDC_TIME:     checkTime     (dataType, static_cast <int>   (reinterpret_cast <qint64> (obj)), itemProps); continue;
                    case DDC_DATE:     checkDate     (dataType, static_cast <int>   (reinterpret_cast <qint64> (obj)), itemProps); continue;
                    case DDC_DOUBLE:   checkDouble   (dataType,                      reinterpret_cast <qint64> (obj),  itemProps); continue;
                    case DDC_DATETIME: checkDateTime (dataType,                      reinterpret_cast <qint64> (obj),  itemProps); continue;

                    case DDC_BYTES:    checkBytes    (dataType, static_cast <QByteArray *> (obj), itemProps); continue;
                    case DDC_STRING:   checkString   (dataType, static_cast <QString    *> (obj), itemProps); continue;

                    case DDC_ARRAY:    checkArray    (j, obj, itemProps); continue;
                    case DDC_NULL:;
                }
            }
        }
    }
}

//  +-------------+
//  | checkCommon |
//  +-------------+

bool DDManager::checkCommon (int type, qint64 data, DDProps *props)
{
    if (type >= 0 && type != (props->operType & 0x0F)) DDBuffer::unexpectedData (props->id, props->operType & 0x0F, type);
    if (props->in) return false;

    if ((props->operType & DDC_CHKMIN) && data < props->min)
         DDException (DDE_CHECK_MIN, (type == DDC_BYTES || type == DDC_STRING ? "Size" : "Value"),
                      props->id, props->name, data, props->min);

    if ((props->operType & DDC_CHKMAX) && data > props->max)
         DDException (DDE_CHECK_MAX, (type == DDC_BYTES || type == DDC_STRING ? "Size" : "Value"),
                      props->id, props->name, data, props->max);
    return true;
}

//  +-----------+
//  | checkByte |
//  +-----------+

void DDManager::checkByte (int type, char data, DDProps *props)
{
    if (checkCommon (type, data, props)) return;

    for (int i = 0; i < static_cast <QVector <char> *> (props->in)->size (); i++)
         if (data    == static_cast <QVector <char> *> (props->in)->at  (i)) return;

    DDException (DDE_CHECK_IN, props->id, props->name, static_cast <qint64> (data));
}

//  +------------+
//  | checkShort |
//  +------------+

void DDManager::checkShort (int type, short data, DDProps *props)
{
    if (checkCommon (type, data, props)) return;

    for (int i = 0; i < static_cast <QVector <short> *> (props->in)->size (); i++)
         if (data    == static_cast <QVector <short> *> (props->in)->at  (i)) return;

    DDException (DDE_CHECK_IN, props->id, props->name, static_cast <qint64> (data));
}

//  +----------+
//  | checkInt |
//  +----------+

void DDManager::checkInt (int type, int data, DDProps *props)
{
    if (checkCommon (type, data, props)) return;

    for (int i = 0; i < static_cast <QVector <int> *> (props->in)->size (); i++)
         if (data    == static_cast <QVector <int> *> (props->in)->at  (i)) return;

    DDException (DDE_CHECK_IN, props->id, props->name, static_cast <qint64> (data));
}

//  +-----------+
//  | checkLong |
//  +-----------+

void DDManager::checkLong (int type, qint64 data, DDProps *props)
{
    if (checkCommon (type, data, props)) return;

    for (int i = 0; i < static_cast <QVector <qint64> *> (props->in)->size (); i++)
         if (data    == static_cast <QVector <qint64> *> (props->in)->at  (i)) return;

    DDException (DDE_CHECK_IN, props->id, props->name, data);
}

//  +------------+
//  | checkFloat |
//  +------------+

void DDManager::checkFloat (int type, int data, DDProps *props)
{
    if (type >= 0 && type != (props->operType & 0x0F)) DDBuffer::unexpectedData (props->id, props->operType & 0x0F, type);

    union {float fv1; int iv1;};
    union {float fv2; int iv2;};

    iv1 = data;

    if (!props->in)
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

    for (int i = 0; i < static_cast <QVector <int> *> (props->in)->size (); i++)
         if (iv1     == static_cast <QVector <int> *> (props->in)->at  (i)) return;

    DDException (DDE_CHECK_IN_FLOAT, props->id, props->name, static_cast <double> (fv1), static_cast <qint64> (iv1));
}

//  +-------------+
//  | checkDouble |
//  +-------------+

void DDManager::checkDouble (int type, qint64 data, DDProps *props)
{
    if (type >= 0 && type != (props->operType & 0x0F)) DDBuffer::unexpectedData (props->id, props->operType & 0x0F, type);

    union {double dv1; qint64 lv1;};
    union {double dv2; qint64 lv2;};

    lv1 = data;

    if (!props->in)
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

    for (int i = 0; i < static_cast <QVector <qint64> *> (props->in)->size (); i++)
         if (lv1     == static_cast <QVector <qint64> *> (props->in)->at  (i)) return;

    DDException (DDE_CHECK_IN_FLOAT, props->id, props->name, dv1, lv1);
}

//  +-----------+
//  | checkTime |
//  +-----------+

void DDManager::checkTime (int type, int data, DDProps *props)
{
    if (checkCommon (type, data, props)) return;

    for (int i = 0; i < static_cast <QVector <int> *> (props->in)->size (); i++)
         if (data    == static_cast <QVector <int> *> (props->in)->at  (i)) return;

    int ms  = data % 1000;    data /= 1000;
    int sec = data % 100;     data /= 100;
    int min = data % 100;     data /= 100;

    char tmp [64];
    snprintf (tmp, sizeof (tmp), "%02d:%02d:%02d.%03d", data, min, sec, ms);

    DDException (DDE_CHECK_IN_STRING, props->id, props->name, tmp);
}

//  +-----------+
//  | checkDate |
//  +-----------+

void DDManager::checkDate (int type, int data, DDProps *props)
{
    if (checkCommon (type, data, props)) return;

    for (int i = 0; i < static_cast <QVector <int> *> (props->in)->size (); i++)
         if (data    == static_cast <QVector <int> *> (props->in)->at  (i)) return;

    int day = data % 100;    data /= 100;
    int mon = data % 100;    data /= 100;

    char tmp [64];
    snprintf (tmp, sizeof (tmp), "%04d-%02d-%02d", data, mon, day);

    DDException (DDE_CHECK_IN_STRING, props->id, props->name, tmp);
}

//  +---------------+
//  | checkDateTime |
//  +---------------+

void DDManager::checkDateTime (int type, qint64 data, DDProps *props)
{
    if (checkCommon (type, data, props)) return;

    for (int i = 0; i < static_cast <QVector <qint64> *> (props->in)->size (); i++)
         if (data    == static_cast <QVector <qint64> *> (props->in)->at  (i)) return;

    int ms   = data % 1000;    data /= 1000;
    int sec  = data % 100;     data /= 100;
    int min  = data % 100;     data /= 100;
    int hour = data % 100;     data /= 100;
    int day  = data % 100;     data /= 100;
    int mon  = data % 100;     data /= 100;

    char tmp [64];
    snprintf (tmp, sizeof (tmp), "%04lld-%02d-%02d_%02d:%02d:%02d.%03d", data, mon, day, hour, min, sec, ms);

    DDException (DDE_CHECK_IN_STRING, props->id, props->name, tmp);
}

//  +------------+
//  | checkBytes |
//  +------------+

void DDManager::checkBytes (int type, QByteArray *data, DDProps *props)
{
    if (!data->isNull ())
    {
        if (checkCommon (type, data->size (), props)) return;

        for (int i = 0; i < static_cast <QVector <QByteArray> *> (props->in)->size (); i++)
             if (*data   == static_cast <QVector <QByteArray> *> (props->in)->at  (i)) return;
    }
    else
        if (!(props->operType & DDC_CHKLIM))
        {
            if (!props->in) return;
            else for (int i = 0; i < static_cast <QVector <QByteArray> *> (props->in)->size (); i++)
                      if (static_cast <QVector <QByteArray> *> (props->in)->at (i).isNull ()) return;
        }

    DDException (DDE_CHECK_IN_STRING, props->id, props->name, (data->isNull () ? "null" : data->toHex ().data ()));
}

//  +-------------+
//  | checkString |
//  +-------------+

void DDManager::checkString (int type, QString *data, DDProps *props)
{
    if (!data->isNull ())
    {
        if (checkCommon (type, data->size (), props)) return;

        for (int i = 0; i < static_cast <QVector <QString> *> (props->in)->size (); i++)
             if (*data   == static_cast <QVector <QString> *> (props->in)->at  (i)) return;
    }
    else
        if (!(props->operType & DDC_CHKLIM))
        {
            if (!props->in) return;
            else for (int i = 0; i < static_cast <QVector <QString> *> (props->in)->size (); i++)
                      if (static_cast <QVector <QString> *> (props->in)->at (i).isNull ()) return;
        }

    DDException (DDE_CHECK_IN_STRING, props->id, props->name, (data->isNull () ? "null" : data->toUtf8 ().data ()));
}

//  +------------+
//  | checkArray |
//  +------------+

void DDManager::checkArray (int type, void *data, DDProps *props)
{
    if (type != (props->operType & 0xFF)) DDBuffer::unexpectedData (props->id, props->operType & 0xFF, type);
    int len = static_cast <QVector <int> *> (data)->size ();

    if (props->number > 0 && props->number != len)
        DDException (DDE_INVALID_STRUCT_SIZE, "array", props->id, len, static_cast <qint64> ( props->number));

    if (props->number < 0 && props->number > -len)
        DDException (DDE_INVALID_STRUCT_SIZE, "array", props->id, len, static_cast <qint64> (-props->number));

    if (props->in)
    {
        for (int x = 0; x < static_cast <QVector <QVector <int>> *> (props->in)->size (); x++)
             if (compareArray (type, data, &static_cast <QVector <QVector <int>> *> (props->in)->data () [x])) return;

        DDException (DDE_CHECK_IN_STRUCT, props->id, props->name);
    }

    int i = 0;

    switch (type >> 4)
    {
        default:           for (; i < len; i++) checkLong     (-1,  static_cast <QVector <qint64>    *> (data)->data () [i], props); break;
        case DDC_BYTE:     for (; i < len; i++) checkByte     (-1,  static_cast <QVector <char>      *> (data)->data () [i], props); break;
        case DDC_SHORT:    for (; i < len; i++) checkShort    (-1,  static_cast <QVector <short>     *> (data)->data () [i], props); break;
        case DDC_INT:      for (; i < len; i++) checkInt      (-1,  static_cast <QVector <int>       *> (data)->data () [i], props); break;
        case DDC_FLOAT:    for (; i < len; i++) checkFloat    (-1,  static_cast <QVector <int>       *> (data)->data () [i], props); break;
        case DDC_DOUBLE:   for (; i < len; i++) checkDouble   (-1,  static_cast <QVector <qint64>    *> (data)->data () [i], props); break;
        case DDC_TIME:     for (; i < len; i++) checkTime     (-1,  static_cast <QVector <int>       *> (data)->data () [i], props); break;
        case DDC_DATE:     for (; i < len; i++) checkDate     (-1,  static_cast <QVector <int>       *> (data)->data () [i], props); break;
        case DDC_DATETIME: for (; i < len; i++) checkDateTime (-1,  static_cast <QVector <qint64>    *> (data)->data () [i], props); break;
        case DDC_BYTES:    for (; i < len; i++) checkBytes    (-1, &static_cast <QVector <QByteArray>*> (data)->data () [i], props); break;
        case DDC_STRING:   for (; i < len; i++) checkString   (-1, &static_cast <QVector <QString>   *> (data)->data () [i], props);
    }
}

//  +-------------+
//  | compareData |
//  +-------------+

bool DDManager::compareData (int type, const void *data1, void *data2)
{
    switch (type & 0x0F)
    {
        default:         return  reinterpret_cast <qint64> (data1) == *static_cast <qint64 *> (data2);

        case DDC_BYTE:   return  static_cast <char>  (reinterpret_cast <qint64> (data1)) == *static_cast <char  *> (data2);
        case DDC_SHORT:  return  static_cast <short> (reinterpret_cast <qint64> (data1)) == *static_cast <short *> (data2);
        case DDC_FLOAT:
        case DDC_TIME:
        case DDC_DATE:
        case DDC_INT:    return  static_cast <int>   (reinterpret_cast <qint64> (data1)) == *static_cast <int   *> (data2);

        case DDC_BYTES:  return  static_cast <const QByteArray *> (data1)->size () ==  static_cast <QByteArray *> (data2)->size () &&
                                *static_cast <const QByteArray *> (data1)          == *static_cast <QByteArray *> (data2);

        case DDC_STRING: return  static_cast <const QString    *> (data1)->size () ==  static_cast <QString    *> (data2)->size () &&
                                *static_cast <const QString    *> (data1)          == *static_cast <QString    *> (data2);

        case DDC_ARRAY:  return  compareArray (type, data1, data2);
        case DDC_LIST:

            union {DDList *list; const void *data1c;};

            data1c = data1;
            return compareList (list, static_cast <DDList *> (data2));
    }
}

//  +--------------+
//  | compareArray |
//  +--------------+

bool DDManager::compareArray (int type, const void *array1, void *array2)
{
    int len  = static_cast <const QVector <int> *> (array1)->size ();
    if (len != static_cast       <QVector <int> *> (array2)->size ()) return false;

    switch (type >> 4)
    {
        default:        return !memcmp (static_cast <const QVector <qint64> *> (array1)->data (),
                                        static_cast       <QVector <qint64> *> (array2)->data (), static_cast <size_t> (len << 3));

        case DDC_BYTE:  return !memcmp (static_cast <const QVector <char>   *> (array1)->data (),
                                        static_cast       <QVector <char>   *> (array2)->data (), static_cast <size_t> (len));

        case DDC_SHORT: return !memcmp (static_cast <const QVector <short>  *> (array1)->data (),
                                        static_cast       <QVector <short>  *> (array2)->data (), static_cast <size_t> (len << 1));
        case DDC_FLOAT:
        case DDC_TIME:
        case DDC_DATE:
        case DDC_INT:   return !memcmp (static_cast <const QVector <int> *> (array1)->data (),
                                        static_cast       <QVector <int> *> (array2)->data (), static_cast <size_t> (len << 2));
        case DDC_BYTES:

            for (int i = 0; i < len; i++)
                if (static_cast <const QVector <QByteArray> *> (array1)->data () [i]  !=
                    static_cast       <QVector <QByteArray> *> (array2)->data () [i]) return false;

            return true;

        case DDC_STRING:

            for (int i = 0; i < len; i++)
                if (static_cast <const QVector <QString> *> (array1)->data () [i]  !=
                    static_cast       <QVector <QString> *> (array2)->data () [i]) return false;

            return true;
    }
}

//  +-------------+
//  | compareList |
//  +-------------+

bool DDManager::compareList (DDList *list1, DDList *list2)
{
    if (list1->getDataNumber () != static_cast <DDList *> (list2)->getDataNumber ()) return false;

    void *obj1, *obj2;
    int   i,     j;

    list1->begin ();
    list2->begin ();

    while (!list1->isEndOfList ())
    {
        obj1 = list1->getData (-1);
        obj2 = list2->getData (-1);
        i    = list1->getCurrentDataType ();
        j    = list2->getCurrentDataType ();

        list1->next ();
        list2->next ();

        if (i == DDC_NULL || j == DDC_NULL)
        {
            if (!obj1 && !obj2) continue;
            else return false;
        }

        if (i != j) return false;

        switch (i & 0x0F)
        {
            default:         if ( reinterpret_cast <qint64> (obj1)  ==
                                  reinterpret_cast <qint64> (obj2)) continue; else return false;    // long, double, number, s_number, datetime

            case DDC_BYTE:   if ( static_cast <char>  (reinterpret_cast <qint64> (obj1))  ==
                                  static_cast <char>  (reinterpret_cast <qint64> (obj2))) continue; else return false;

            case DDC_SHORT:  if ( static_cast <short> (reinterpret_cast <qint64> (obj1))  ==
                                  static_cast <short> (reinterpret_cast <qint64> (obj2))) continue; else return false;
            case DDC_FLOAT:
            case DDC_TIME:
            case DDC_DATE:
            case DDC_INT:    if ( static_cast <int>   (reinterpret_cast <qint64> (obj1))  ==
                                  static_cast <int>   (reinterpret_cast <qint64> (obj2))) continue; else return false;

            case DDC_BYTES:  if (*static_cast <QByteArray *> (obj1)  ==
                                 *static_cast <QByteArray *> (obj2)) continue; else return false;

            case DDC_STRING: if (*static_cast <QString    *> (obj1)  ==
                                 *static_cast <QString    *> (obj2)) continue; else return false;

            case DDC_ARRAY:  if (!compareArray (i, obj1, obj2)) return false;
        }
    }

    return true;
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

    DDBuffer ddBuffer = DDBuffer (buffer, offset, length, &newObjectList);

    if (checkProtocolInfo) ddBuffer.checkProtocolInfo ();
    if (checkCRC32)        ddBuffer.checkCRC32        ();

    int   id, type;
    void *obj;

    while (!ddBuffer.endOfBuffer ())
    {
        id   =  ddBuffer.readTypeAndId ();
        type = (id >> 12) & 0x0F;

        obj = ddBuffer.read (type);
        storeData (id & 0xFFF, obj, type);
    }
}

//  +------------+
//  | linkObject |
//  +------------+

const void **DDManager::linkObject (int id, void **obj)
{
    int idx = id >> 8;
    if (!(obj [idx])) {obj [idx] = new void * [16] (); addNew (obj [idx]);}

    obj = static_cast <void **> (obj [idx]);
    idx = (id >> 4) & 0x0F;

    if (!(obj [idx])) {obj [idx] = new void * [16] (); addNew (obj [idx]);}
    return static_cast <const void **> (obj [idx]);
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

void DDManager::storeData (int id, const void *data, int type)
{
    if (dataTypes [id] != DDC_NULL) DDException (DDE_DATA_ALREADY_PRESENT, id);

    if (dataDefault)
    {
        DDProps *props = static_cast <DDProps *> (seekObject (id, propertiesRoot));
        if (props && props->def && !(props->operType & DDC_REQ) &&
            compareData (type, data, props->def)) return;             // Default
    }

    dataTypes  [id] = static_cast <unsigned char> (type);
    linkObject (id, dataRoot) [id & 0x0F] = data;
    dataNumber++;
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
    DDBuffer   ddBuffer   (tempBuffer, 0, 4096, &newObjectList);

    if (addProtocolInfo) ddBuffer.addProtocolInfo ();

    int                 i, j, k, x;
    void               *data;
    void              **objTable1, **objTable2;
    QVector <DDProps>  *propsTable;
    DDList             *list;
    DDProps            *p;

    if (propertiesNumber)
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

                    if (dataTypes [x] != DDC_LIST)  continue;
                    list = static_cast  <DDList *> (seekObject (x, dataRoot));

                    propsTable = list->getProperties ();
                    if (!propsTable) DDException (DDE_LIST_PROPERTIES_NOT_FOUND, static_cast <DDProps *> (data)->id);

                    x = propsTable->size ();
                    list->begin ();

                    while (!list->isEndOfList ())
                    {
                        p = &propsTable->data () [list->getCurrentDataOffset () % x];

                        if ((p->operType & DDC_REQ) != 0 && list->getCurrentDataType () == DDC_NULL)
                             DDException (DDE_MISSING_DATA, p->id, p->name);

                        list->next ();
                    }

                    for (x = list->getDataNumber (); x < propsTable->size (); x++)
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

    if (offset) memcpy (buffer.data () + reservedSizeBefore, ddBuffer.getBuffer (), static_cast <size_t> (offset));
}

//  #=====================#
//  # Date Time Utilities #
//  #=====================#

int DDManager::getYear   (int date) {return  date /    10000; }
int DDManager::getMonth  (int date) {return (date /      100) %  100;}
int DDManager::getDay    (int date) {return  date             %  100;}
int DDManager::getHour   (int time) {return  time / 10000000; }
int DDManager::getMinute (int time) {return (time /   100000) %  100;}
int DDManager::getSecond (int time) {return (time /     1000) %  100;}
int DDManager::getMSec   (int time) {return  time             % 1000;}

int DDManager::getYear   (qint64 datetime) {return getYear   (static_cast <int> (datetime / 1000000000));}
int DDManager::getMonth  (qint64 datetime) {return getMonth  (static_cast <int> (datetime / 1000000000));}
int DDManager::getDay    (qint64 datetime) {return getDay    (static_cast <int> (datetime / 1000000000));}
int DDManager::getHour   (qint64 datetime) {return getHour   (static_cast <int> (datetime % 1000000000));}
int DDManager::getMinute (qint64 datetime) {return getMinute (static_cast <int> (datetime % 1000000000));}
int DDManager::getSecond (qint64 datetime) {return getSecond (static_cast <int> (datetime % 1000000000));}
int DDManager::getMSec   (qint64 datetime) {return getMSec   (static_cast <int> (datetime % 1000000000));}

int DDManager::getTime (int hour, int minute, int second, int msec)
{
    if (msec >= 0 && msec <= 1000 && second >= 0 && second < 60 && minute >= 0 && minute < 60 && hour >= 0 && hour < 24)
        return ((hour * 100 + minute) * 100 + second) * 1000 + msec;

    return -1;
}

int DDManager::getDate (int year, int month, int day)
{
    if (year && (year < 0 ? -year - 1 : year) <= 214747 && month > 0 && month <= 12 && day > 0 &&
       (day <= DDBuffer::MONTH_DAYS [month - 1] || (month == 2 && day == 29 && isLeapYear (year))))
            return (year * 100 + month) * 100 + day;

    return -1;
}

qint64 DDManager::getDateTime (int year, int month, int day, int hour, int minute, int second, int msec)
{
    if (msec >= 0 && msec <= 1000 && second >= 0 && second < 60 && minute >= 0 && minute < 60 && hour >= 0 && hour < 24 &&
        year && (year < 0 ? -year - 1 : year) <= 214747 && month > 0 && month <= 12 && day > 0 &&
       (day <= DDBuffer::MONTH_DAYS [month - 1] || (month == 2 && day == 29 && isLeapYear (year))))
            return (((((year * 100 + month) * 100 + day) * 100 + hour) * 100 + minute) * 100 + second) * 1000 + msec;

    return -1;
}

int DDManager::getTime (const QTime &time)
    {return ((time.hour () * 100 + time.minute ()) * 100 + time.second ()) * 1000 + time.msec ();}

int DDManager::getDate (const QDate &date)
    {return (date.year () * 100 + date.month ()) * 100 + date.day ();}

qint64 DDManager::getDateTime (const QDateTime &datetime)
{
    QTime t = datetime.time ();
    QDate d = datetime.date ();

    return (((((d.year ()  * 100 + d.month  ()) * 100 + d.day    ()) * 100  +
                t.hour ()) * 100 + t.minute ()) * 100 + t.second ()) * 1000 + t.msec ();
}

QTime DDManager::getQTime (int time)
{
    int ms  = time % 1000;    time /= 1000;
    int sec = time % 100;     time /= 100;
    int min = time % 100;     time /= 100;

    return QTime (time, min, sec, ms);
}

QDate DDManager::getQDate (int date)
{
    int day = date % 100;    date /= 100;
    int mon = date % 100;    date /= 100;

    return QDate (date, mon, day);
}

QDateTime DDManager::getQDateTime (qint64 datetime)
{
    int ms   = datetime % 1000;    datetime /= 1000;
    int sec  = datetime % 100;     datetime /= 100;
    int min  = datetime % 100;     datetime /= 100;
    int hour = datetime % 100;     datetime /= 100;
    int day  = datetime % 100;     datetime /= 100;
    int mon  = datetime % 100;     datetime /= 100;

    return QDateTime (QDate (static_cast <int> (datetime), mon, day), QTime (hour, min, sec, ms));
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

    QTextStream (&str) << dataNumber << '{';

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
                DDBuffer::stringData (&str, data, dataTypes [x]);
            }
        }
    }

    QTextStream (&str) << '}';
    return str;
}
