//  #=====================================#
//  # Dynamic Data Buffer - Version 1.0.0 #
//  #                                     #
//  # Author: Marco Vagnoni               #
//  # Email:  marco.vagnoni@yahoo.com     #
//  # Date:   April     2019 (v1.0.0)     #
//  #=====================================#

//  Type            TypeId

//  Byte            4b =1     12b Id     8b Data
//  Short           4b =2     12b Id    16b Data
//  Int             4b =3     12b Id    32b Data
//  Long            4b =4     12b Id    64b Data

//  Number          4b =5     12b Id     ?B Data
//  SignedNumber    4b =6     12b Id     ?B Data

//  Float           4b =7     12b Id    32b Data
//  Double          4b =8     12b Id    64b Data

//  Time            4b =9     12b Id     ?B(Data+1bMs) Data=( 5b Hour, 6b Min,   6b Sec, [10b Millis])
//  Date            4b =10    12b Id     ?B Data       Data=(27b Year, 4b Month, 5b Day)
//  DateTime        4b =11    12b Id     ?B(Data+1bMs) Data=(27b Year, 4b Month, 5b Day, 5b Hour, 6b Min, 6b Sec, [10b Millis])

//  Bytes           4b =12    12b Id     ?B Size       SizeB Data
//  String          4b =13    12b Id     ?B Size       SizeB Data

//  Array           4b =14    12b Id     ?B(Size+1bItemRem+4bItemType)   [?B ItemsRem]    1B Bitmap    ET Data     ...

//  List            4b =15    12b Id     ?B Size       1B Type1 + 2     Data1    Data2    ...

//  ?B = DynamicNumber -> 1b Next (0=No,1=Yes) 7b Data ... (max 10 Bytes -> 7 * 10 = 70 (Used 64))

#include <cstring>

#include <QChar>
#include <QTextStream>
#include <QVector>

#include "DDBuffer.h"
#include "DDConst.h"
#include "DDError.h"

#include "crc32.h"

#define isLeapYear(year) (!(year % 400) || (!(year % 4) && (year % 100)))

                                      //  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
const char  DDBuffer::DATA_TYPES   [] = {'z','b','s','i','I','n','N','f','F','t','d','D','B','S','A','L'};
const char  DDBuffer::MONTH_DAYS   [] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const char *DDBuffer::STRING_TYPES [] = {"Null","Byte","Short","Integer","Long","Number","SignedNumber","Float",
                                         "Double","Time","Date","DateTime","Bytes","String","Array","List"};
char DDBuffer::tempBuffer [];

//  #=============#
//  # Constructor #
//  #=============#

DDBuffer::DDBuffer (QByteArray &byteArray, int offset, int length, ObjectNode **newObjectList)
{
    this->byteArray     =  byteArray;
    this->buffer        =  byteArray.data ();
    this->length        =  length;
    this->offset        =  offset;
    this->start         =  offset;
    this->newObjectList = *newObjectList;
}

//  #=====#
//  # get #
//  #=====#

char *DDBuffer::getBuffer () {return buffer;}
int   DDBuffer::getOffset () {return offset;}

//  #=============#
//  # endOfBuffer #
//  #=============#

bool DDBuffer::endOfBuffer () {return offset >= length;}

//  +--------+
//  | addNew |
//  +--------+

void DDBuffer::addNew (void *data, int type)
{
    ObjectNode *newNode = new ObjectNode ();
    newNode->object     = data;
    newNode->type       = type;
    newNode->next       = newObjectList;
    newObjectList       = newNode;
}

//  +----------------+
//  | ensureCapacity |
//  +----------------+

bool DDBuffer::ensureCapacity (int bytesNumber)
{
    if ((offset + bytesNumber) <= length) return true;
    if  (byteArray.size ()     >= 0xFFFF) return false;

    int newSize = byteArray.size () << 1;
    if (newSize > 0xFFFF) newSize = 0xFFFF;

    byteArray.resize (newSize);

    length += newSize - byteArray.size ();
    buffer  = byteArray.data ();

    return (offset + bytesNumber) <= length;
}

//  +----------------+
//  | unexpectedData |
//  +----------------+

[[noreturn]] void DDBuffer::unexpectedData (int id, int expected, int found)
{
    char expectedType [32];
    char foundType    [32];

    int  expectedIdH = expected >> 4;
    int  foundIdH    = found    >> 4;
    int  i, j;

    if (!expectedIdH) i = 0;
    else i = snprintf (expectedType, 32, "%s", STRING_TYPES [expectedIdH]);

    if (!foundIdH) j = 0;
    else j = snprintf (foundType,    32, "%s", STRING_TYPES [foundIdH]);

    snprintf (expectedType + i, 32 - static_cast <unsigned> (i), "%s", STRING_TYPES [expected & 0x0F]);
    snprintf (foundType    + j, 32 - static_cast <unsigned> (j), "%s", STRING_TYPES [found    & 0x0F]);

    DDException (DDE_INVALID_DATA_TYPE, id, expectedType, foundType);
}

//  #=================#
//  # addProtocolInfo #
//  #=================#

void DDBuffer::addProtocolInfo ()
{
    if (!ensureCapacity (4)) DDException (DDE_BUFFER_OVERFLOW, 4, offset, length);

    buffer [offset++] = 'D';
    buffer [offset++] = 'D';
    buffer [offset++] = 'P';
    buffer [offset++] =  0x10;
}

//  #===================#
//  # checkProtocolInfo #
//  #===================#

void DDBuffer::checkProtocolInfo ()
{
    if (offset > length - 4) DDException (DDE_BUFFER_OVERFLOW, 4, offset, length);

    int id = (buffer [offset + 3] >> 4) & 0x0F;    // id = Version 0001-0000 -> valid 0001-????

    if (buffer [offset] != 'D' || buffer [offset + 1] != 'D' || buffer [offset + 2] != 'P' || id != 1)
        DDException (DDE_INVALID_PROTOCOL_INFO, (buffer [offset]     << 20) | (buffer [offset + 1] << 12) |
                                                (buffer [offset + 2] <<  4) |  id);
    offset += 4;
}

//  #==========#
//  # addCRC32 #
//  #==========#

int DDBuffer::addCRC32 ()
{
    if (!ensureCapacity (4)) DDException (DDE_BUFFER_OVERFLOW, 4, offset, length);
    crc32::write (crc32::update (0, offset - start, buffer + start), buffer + offset);

    offset += 4;
    return offset;
}

//  #=============#
//  # checkCRC32  #
//  #=============#

void DDBuffer::checkCRC32 ()
{
    int len = length - start - 4;
    if (len < 0) DDException (DDE_BUFFER_OVERFLOW, len, start, length - 4);

    int crcFound = ((buffer [length - 4] & 0xFF) << 24) | ((buffer [length - 3] & 0xFF) << 16) |
                   ((buffer [length - 2] & 0xFF) <<  8) |  (buffer [length - 1] & 0xFF);

    int crcExpected = static_cast <int> (crc32::update (0, len, buffer + start));
    if (crcFound   != crcExpected) DDException (DDE_INVALID_CRC, crcExpected, crcFound);
    length -= 4;
}

//  #======#
//  # read #
//  #======#

#define READ_ARRAY_1(vectorType,dataType,method) \
{ \
    obj  =  new  QVector <vectorType> (itemNumbers); \
    addNew (obj, dataType); \
\
    for (; i < size; i++, bm <<= 1) \
    { \
        if (!(i & 7)) bm = readArrayBitmap (); \
        if ((bm & 0x80))   static_cast <QVector <vectorType> *> (obj)->data () [i] = method; \
    } \
}

#define READ_ARRAY_2(vectorType,dataType,method) \
{ \
    obj  =  new  QVector <vectorType> (itemNumbers); \
    addNew (obj, dataType); \
\
    for (; i < size; i++, bm <<= 1) \
    { \
        if (!(i & 7)) bm = readArrayBitmap (); \
        if ((bm & 0x80))   method (static_cast <QVector <vectorType> *> (obj)->data () [i]); \
    } \
}

void *DDBuffer::read (int &type)
{
    switch (type & 0x0F)
    {
        case DDC_BYTE:      return reinterpret_cast <void *> (static_cast <qint64> (readByte  ()));
        case DDC_SHORT:     return reinterpret_cast <void *> (static_cast <qint64> (readShort ()));
        case DDC_FLOAT:
        case DDC_INT:       return reinterpret_cast <void *> (static_cast <qint64> (readInt   ()));
        case DDC_TIME:      return reinterpret_cast <void *> (static_cast <qint64> (readTime  ()));
        case DDC_DATE:      return reinterpret_cast <void *> (static_cast <qint64> (readDate  ()));
        case DDC_DOUBLE:
        case DDC_LONG:      return reinterpret_cast <void *> (readLong     ());
        case DDC_NUMBER:    return reinterpret_cast <void *> (readNumber   (false));
        case DDC_S_NUMBER:  return reinterpret_cast <void *> (readNumber   (true));
        case DDC_DATETIME:  return reinterpret_cast <void *> (readDateTime ());

        case DDC_BYTES:    {QByteArray *v = new QByteArray (); addNew (v, DDC_BYTES);  readBytes  (*v); return v;}
        case DDC_STRING:   {QString    *v = new QString    (); addNew (v, DDC_STRING); readString (*v); return v;}

        case DDC_ARRAY:
        {
            qint64 v                = readNumber (false);
            int    itemRemovedFlag  = v & 0x10;                 //  Item Removed Flag
            int    itemType         = v & 0x0F;                 //  Item Type
            v                     >>= 5;                        //  Item Numbers
            int    size             = static_cast <int> (v);

            if (itemRemovedFlag) v += readNumber (false);       // +Item Removed Numbers

            if (itemType <=  DDC_NULL || itemType >= DDC_ARRAY || v > DDC_MAX_ITEMS_NUMBER)
                DDException (DDE_INVALID_ARRAY_ITEM, itemType,    v,  mark, length);

            int   itemNumbers = static_cast <int> (v);
            int   i = 0,  bm  = 0;
            void *obj;

            switch (itemType)
            {
                case DDC_BYTE:      READ_ARRAY_1 (char,       DDC_A_BYTE,     readByte     ());      break;
                case DDC_SHORT:     READ_ARRAY_1 (short,      DDC_A_SHORT,    readShort    ());      break;
                case DDC_FLOAT:
                case DDC_INT:       READ_ARRAY_1 (int,        DDC_A_INT,      readInt      ());      break;
                case DDC_TIME:      READ_ARRAY_1 (int,        DDC_A_TIME,     readTime     ());      break;
                case DDC_DATE:      READ_ARRAY_1 (int,        DDC_A_DATE,     readDate     ());      break;
                case DDC_DOUBLE:
                case DDC_LONG:      READ_ARRAY_1 (qint64,     DDC_A_LONG,     readLong     ());      break;
                case DDC_NUMBER:    READ_ARRAY_1 (qint64,     DDC_A_NUMBER,   readNumber   (false)); break;
                case DDC_S_NUMBER:  READ_ARRAY_1 (qint64,     DDC_A_S_NUMBER, readNumber   (true));  break;
                case DDC_DATETIME:  READ_ARRAY_1 (qint64,     DDC_A_DATETIME, readDateTime ());      break;
                case DDC_BYTES:     READ_ARRAY_2 (QByteArray, DDC_A_BYTES,    readBytes);            break;
                case DDC_STRING:    READ_ARRAY_2 (QString,    DDC_A_STRING,   readString);           break;

                default: snprintf (tempBuffer, sizeof (tempBuffer), "%s%s", STRING_TYPES [itemType], STRING_TYPES [DDC_ARRAY]);
                         DDException (DDE_INVALID_VALUE, "Data Type", tempBuffer, type, mark, length);
            }

            type |= (itemType << 4);
            return obj;
        }

        case DDC_LIST:
        {
            qint64 v = readNumber (false);
            if  (v > DDC_MAX_ITEMS_NUMBER) DDException (DDE_INVALID_SIZE, "List Size", v, DDC_MAX_ITEMS_NUMBER, DDC_LIST, mark, length);

            DDList *list = new DDList ();
            addNew (list,  DDC_LIST);

            int   objectType, size = static_cast <int> (v);
            void *obj;

            for (int i = 0, j = 0; i < size; i++)
            {
                if ((i & 1) > 0) objectType = j & 0x0F;
                else
                {
                    if (offset >= length) DDException (DDE_BUFFER_OVERFLOW, 1, offset, length);
                    j = buffer [offset++] & 0xFF;
                    objectType = j >> 4;
                }

                obj = objectType == DDC_NULL ? nullptr : read (objectType);
                list->addData (obj, objectType);
            }

            return list;
        }

        default: DDException (DDE_INVALID_VALUE, "Data Type", STRING_TYPES [DDC_NULL], DDC_NULL, mark, length);
    }
}

//  +----------+
//  | readByte |
//  +----------+

char DDBuffer::readByte ()
{
    if (offset >= length) DDException (DDE_BUFFER_OVERFLOW, 1, offset, length);
    return buffer [offset++];
}

//  +-----------+
//  | readShort |
//  +-----------+

short DDBuffer::readShort ()
{
    if ((offset + 2) > length) DDException (DDE_BUFFER_OVERFLOW, 2, offset, length);

    int  v =        buffer [offset++] & 0xFF;
    v = (v << 8) | (buffer [offset++] & 0xFF);

    return static_cast <short> (v);
}

//  +---------+
//  | readInt |
//  +---------+

int DDBuffer::readInt ()
{
    if ((offset + 4) > length) DDException (DDE_BUFFER_OVERFLOW, 4, offset, length);

    int  v =        buffer [offset++] & 0xFF;
    v = (v << 8) | (buffer [offset++] & 0xFF);
    v = (v << 8) | (buffer [offset++] & 0xFF);
    v = (v << 8) | (buffer [offset++] & 0xFF);

    return v;
}

//  +----------+
//  | readLong |
//  +----------+

qint64 DDBuffer::readLong ()
{
    if ((offset + 8) > length) DDException (DDE_BUFFER_OVERFLOW, 8, offset, length);

    qint64  v =     buffer [offset++] & 0xFF;
    v = (v << 8) | (buffer [offset++] & 0xFF);
    v = (v << 8) | (buffer [offset++] & 0xFF);
    v = (v << 8) | (buffer [offset++] & 0xFF);
    v = (v << 8) | (buffer [offset++] & 0xFF);
    v = (v << 8) | (buffer [offset++] & 0xFF);
    v = (v << 8) | (buffer [offset++] & 0xFF);
    v = (v << 8) | (buffer [offset++] & 0xFF);

    return v;
}

//  +------------+
//  | readNumber |
//  +------------+

qint64 DDBuffer::readNumber (bool withSign)
{
    int len = length - offset;
    mark    = offset;

    if (len > 0)
    {
        char    b     = -1;
        char    fb    =  buffer [offset];
        int     max   =  withSign ?  10 : 9;
        quint64 value =  0;

        if (len > max) len = max;
        int lc  = len;

        while (--len >= 0 && b < 0)
        {
            b     =  buffer [offset++];
            value = (value << 7) | (b & 0x7F);
        }

        if (b >= 0)
        {
            if (!withSign) return static_cast <qint64> (value);

            if (len > 0 || lc < 10 || (fb & 0x7E) == 0)
            {
                value = (value & 1) > 0 ?  ~(value >> 1) : value >> 1;
                return static_cast <qint64> (value);
            }
        }

        if (offset <= length) DDException (DDE_INVALID_DYNAMIC_NUMBER, fb, b, mark, length);
    }

    DDException (DDE_BUFFER_OVERFLOW, 1, mark, length);
}

//  +----------+
//  | readTime |
//  +----------+

int DDBuffer::readTime ()
{
    qint64 v = readNumber (false);
    if   (!v)  return 0;

    int ms = v & 1;

    v   >>=  1;
    if (ms) {ms = v & 0x3FF; v >>= 10;}

    int sec = v & 0x3F;    v >>= 6;
    int min = v & 0x3F;    v >>= 6;

    if (ms < 1000 && sec < 60 && min < 60 && v >= 0 && v < 24)
        return ((static_cast <int> (v) * 100 + min) * 100 + sec) * 1000 + ms;

    snprintf    (tempBuffer, sizeof (tempBuffer), "%02lld:%02d:%02d.%03d", v, min, sec, ms);
    DDException (DDE_INVALID_VALUE, "Time", tempBuffer, DDC_TIME, mark, length);
}

//  +----------+
//  | readDate |
//  +----------+

int DDBuffer::readDate ()
{
    qint64 v = readNumber (true);
    if   (!v)  return 0;

    int day = v & 0x1F;    v >>= 5;
    int mon = v & 0x0F;    v >>= 4;

    qint64 year = v < 0 ? -v - 1 : v;

    if (v && year <= 214747 && mon && mon <= 12 && day &&
       (day <= MONTH_DAYS [mon - 1] || (day == 29 && mon == 2 && isLeapYear (year))))
            return (static_cast <int> (v) * 100 + mon) * 100 + day;

    snprintf    (tempBuffer, sizeof (tempBuffer), "%04lld-%02d-%02d", v, mon, day);
    DDException (DDE_INVALID_VALUE, "Date", tempBuffer, DDC_DATE, mark, length);
}

//  +--------------+
//  | readDateTime |
//  +--------------+

qint64 DDBuffer::readDateTime ()
{
    qint64 v = readNumber (true);
    if   (!v)  return 0;

    int ms = v & 1;

    v >>= 1;
    if (ms) {ms = v & 0x3FF; v >>= 10;}

    int sec  = v & 0x3F;    v >>= 6;
    int min  = v & 0x3F;    v >>= 6;
    int hour = v & 0x1F;    v >>= 5;
    int day  = v & 0x1F;    v >>= 5;
    int mon  = v & 0x0F;    v >>= 4;

    qint64 year = v < 0 ? -v - 1 : v;

    if (ms < 1000 && sec < 60 && min < 60 && hour >= 0 && hour < 24 &&
        v && year <= 214747 && mon && mon <= 12 && day &&
       (day <= DDBuffer::MONTH_DAYS [mon - 1] || (day == 29 && mon == 2 && isLeapYear (year))))
            return ((((((v * 100 + mon) * 100 + day) * 100) + hour) * 100 + min) * 100 + sec) * 1000 + ms;

    snprintf    (tempBuffer, sizeof (tempBuffer), "%04lld-%02d-%02d_%02d:%02d:%02d.%03d", v, mon, day, hour, min, sec, ms);
    DDException (DDE_INVALID_VALUE, "DateTime", tempBuffer, DDC_DATETIME, mark, length);
}

//  +-----------+
//  | readBytes |
//  +-----------+

void DDBuffer::readBytes (QByteArray &bytes)
{
    qint64 v = readNumber (false);

    if (v > DDC_MAX_BYTES_NUMBER)
        DDException (DDE_INVALID_SIZE, "Bytes Size", v, DDC_MAX_BYTES_NUMBER, DDC_BYTES, mark, length);

    int size = static_cast <int> (v);
    mark     = offset;

    if ((offset += size) > length) DDException (DDE_BUFFER_OVERFLOW, size, mark, length);
    bytes = QByteArray (buffer + mark, size);
}

//  +------------+
//  | readString |
//  +------------+

void DDBuffer::readString (QString &string)
{
    qint64 v = readNumber (false);

    if (v > DDC_MAX_BYTES_NUMBER)
        DDException (DDE_INVALID_SIZE, "String Size", v, DDC_MAX_BYTES_NUMBER, DDC_STRING, mark, length);

    int size = static_cast <int> (v);
    mark     = offset;

    if ((offset += size) > length) DDException (DDE_BUFFER_OVERFLOW, size, mark, length);

    if (size && buffer [offset - 1] < 0)
    {
        snprintf    (tempBuffer, sizeof (tempBuffer), "0x%x", static_cast <unsigned> (buffer [offset - 1]));
        DDException (DDE_INVALID_VALUE, "String Termination", tempBuffer, DDC_STRING, mark, length);
    }

    char b;
    int  i,  j;

    string   = QString (size, 0);
    QChar *c = string.data ();

    for (j = 0; mark < offset; )
    {
        b = buffer [mark++];
        i = b & 0x7F;

        if (b < 0)
        {
            b =  buffer [mark++];
            i = (i << 7) | (b & 0x7F);

            if (b < 0)
            {
                b =  buffer [mark++];
                i = (i << 7) | (b & 0x7F);

                if (b < 0 || i > 0xFFFF)
                {
                    if (b < 0) i |= 0x80;
                    snprintf    (tempBuffer, sizeof (tempBuffer), "0x%x", i);
                    DDException (DDE_INVALID_VALUE, "String Item", tempBuffer, DDC_STRING, mark - 3, length);
                }
            }
        }

        c [j++] = i;
    }
}

//  +-----------------+
//  | readArrayBitmap |
//  +-----------------+

int DDBuffer::readArrayBitmap ()
{
    if (offset >= length) DDException (DDE_BUFFER_OVERFLOW, 1, offset, length);
    return buffer [offset++] & 0xFF;
}

//  #===============#
//  # readTypeAndId #
//  #===============#

int DDBuffer::readTypeAndId ()
{
    if (offset > length - 2) DDException (DDE_BUFFER_OVERFLOW, 2, offset, length);

    int     res = (buffer [offset++] & 0xFF) << 8;
    return (res | (buffer [offset++] & 0xFF));
}

//  #=======#
//  # write #
//  #=======#

#define WRITE_ARRAY_1(vectorType,method) \
{ \
    for (size = i; size > 0 && !static_cast <QVector <vectorType> *> (data)->data () [size - 1];) {size--;} \
    writeArrayInfo (size, i, type); \
\
    for (i = 0; i < size; i++) \
    { \
        if (!(i & 7)) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;} \
        bm <<= 1; \
\
        if          (!static_cast <QVector <vectorType> *> (data)->data () [i])  bm |= 0; \
        else {method (static_cast <QVector <vectorType> *> (data)->data () [i]); bm |= 1;} \
    } \
}

#define WRITE_ARRAY_2(vectorType,method,param) \
{ \
    for (size = i; size > 0 && !static_cast <QVector <vectorType> *> (data)->data () [size - 1];) {size--;} \
    writeArrayInfo (size, i, type); \
\
    for (i = 0; i < size; i++) \
    { \
        if (!(i & 7)) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;} \
        bm <<= 1; \
\
        if          (!static_cast <QVector <vectorType> *> (data)->data () [i])         bm |= 0; \
        else {method (static_cast <QVector <vectorType> *> (data)->data () [i], param); bm |= 1;} \
    } \
}

#define WRITE_ARRAY_3(vectorType,method) \
{ \
    for (size = i; size > 0 && static_cast <QVector <vectorType> *> (data)->data () [size - 1].isNull ();) {size--;} \
    writeArrayInfo (size, i, type); \
\
    for (i = 0; i < size; i++) \
    { \
        if (!(i & 7)) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;} \
        bm <<= 1; \
\
        if           (static_cast <QVector <vectorType> *> (data)->data () [i].isNull ()) bm |= 0; \
        else {method (static_cast <QVector <vectorType> *> (data)->data () [i]);          bm |= 1;} \
    } \
}

void DDBuffer::write (void *data, int type)
{
    switch (type & 0x0F)
    {
        case DDC_BYTE:     writeByte     ( static_cast <char > (reinterpret_cast <qint64> (data))); return;
        case DDC_SHORT:    writeShort    ( static_cast <short> (reinterpret_cast <qint64> (data))); return;
        case DDC_FLOAT:
        case DDC_INT:      writeInt      ( static_cast <int  > (reinterpret_cast <qint64> (data))); return;
        case DDC_TIME:     writeTime     ( static_cast <int  > (reinterpret_cast <qint64> (data))); return;
        case DDC_DATE:     writeDate     ( static_cast <int  > (reinterpret_cast <qint64> (data))); return;
        case DDC_DOUBLE:
        case DDC_LONG:     writeLong     ( reinterpret_cast <qint64> (data));        return;
        case DDC_NUMBER:   writeNumber   ( reinterpret_cast <qint64> (data), false); return;
        case DDC_S_NUMBER: writeNumber   ( reinterpret_cast <qint64> (data), true);  return;
        case DDC_DATETIME: writeDateTime ( reinterpret_cast <qint64> (data));        return;

        case DDC_BYTES:    writeBytes    (*static_cast <QByteArray*> (data));        return;
        case DDC_STRING:   writeString   (*static_cast <QString   *> (data));        return;

        case DDC_ARRAY:
        {
            type >>= 4;
            int  i = static_cast <QVector <int> *> (data)->size ();    // any type, needs only to get size
            int  size;
            int  bm = 0, bmp = 0;

            switch (type)
            {
                default:           WRITE_ARRAY_1 (qint64,     writeLong);          break;    // double, long
                case DDC_BYTE:     WRITE_ARRAY_1 (char,       writeByte);          break;
                case DDC_SHORT:    WRITE_ARRAY_1 (short,      writeShort);         break;
                case DDC_FLOAT:
                case DDC_INT:      WRITE_ARRAY_1 (int,        writeInt);           break;
                case DDC_TIME:     WRITE_ARRAY_1 (int,        writeTime);          break;
                case DDC_DATE:     WRITE_ARRAY_1 (int,        writeDate);          break;
                case DDC_NUMBER:   WRITE_ARRAY_2 (qint64,     writeNumber, false); break;
                case DDC_S_NUMBER: WRITE_ARRAY_2 (qint64,     writeNumber, true);  break;

                case DDC_DATETIME: WRITE_ARRAY_1 (qint64,     writeDateTime);      break;
                case DDC_BYTES:    WRITE_ARRAY_3 (QByteArray, writeBytes);         break;
                case DDC_STRING:   WRITE_ARRAY_3 (QString,    writeString);
            }

            if (i)
            {
                if ((i % 8)) bm <<= (8 - i % 8);
                buffer [bmp] = static_cast <char> (bm);
            }

            return;
        }

        default:    // case DDC_LIST:
        {
            DDList *list = static_cast <DDList *> (data);
            writeNumber (list->getDataNumber (), false);

            void *obj;
            int   j, k, i = -1;

            list->begin ();

            while (!list->isEndOfList ())
            {
                obj = list->getData (-1);
                j   = list->getCurrentDataType ();
                k   = j & 0x0F;

                if (i >= 0)
                {
                    buffer [i] |= k;
                    i = -1;
                }
                else
                {
                    if (!ensureCapacity (1)) DDException (DDE_BUFFER_OVERFLOW, 1, offset, length);

                    i = offset;
                    buffer [offset++] = static_cast <char> (k << 4);
                }

                if (j != DDC_NULL) write (obj, j);
                list->next ();
            }
        }
    }
}

//  +-----------+
//  | writeByte |
//  +-----------+

void DDBuffer::writeByte (char value)
{
    if (!ensureCapacity (1)) DDException (DDE_BUFFER_OVERFLOW, 1, offset, length);
    buffer [offset++] = value;
}

//  +------------+
//  | writeShort |
//  +------------+

void DDBuffer::writeShort (short value)
{
    if (!ensureCapacity (2)) DDException (DDE_BUFFER_OVERFLOW, 2, offset, length);

    buffer [offset + 1] = static_cast <char> (value);    value >>= 8;
    buffer [offset]     = static_cast <char> (value);
    offset += 2;
}

//  +----------+
//  | writeInt |
//  +----------+

void DDBuffer::writeInt (int value)
{
    if (!ensureCapacity (4)) DDException (DDE_BUFFER_OVERFLOW, 4, offset, length);

    buffer [offset + 3] = static_cast <char> (value);    value >>= 8;
    buffer [offset + 2] = static_cast <char> (value);    value >>= 8;
    buffer [offset + 1] = static_cast <char> (value);    value >>= 8;
    buffer [offset]     = static_cast <char> (value);
    offset += 4;
}

//  +-----------+
//  | writeLong |
//  +-----------+

void DDBuffer::writeLong (qint64 value)
{
    if (!ensureCapacity (8)) DDException (DDE_BUFFER_OVERFLOW, 8, offset, length);

    buffer [offset + 7] = static_cast <char> (value);    value >>= 8;
    buffer [offset + 6] = static_cast <char> (value);    value >>= 8;
    buffer [offset + 5] = static_cast <char> (value);    value >>= 8;
    buffer [offset + 4] = static_cast <char> (value);    value >>= 8;
    buffer [offset + 3] = static_cast <char> (value);    value >>= 8;
    buffer [offset + 2] = static_cast <char> (value);    value >>= 8;
    buffer [offset + 1] = static_cast <char> (value);    value >>= 8;
    buffer [offset]     = static_cast <char> (value);
    offset += 8;
}

//  +-------------+
//  | writeNumber |
//  +-------------+

void DDBuffer::writeNumber (qint64 value, bool withSign)
{
    int dynamicNumberSize = 0;
    if (withSign) value = value < 0 ? ((~value) << 1) | 1 : value << 1;

    for (tempBuffer [0] = static_cast <char> (value & 0x7F), value = static_cast <quint64> (value) >> 7; value; value = static_cast <quint64> (value) >> 7)
         tempBuffer [++dynamicNumberSize] = static_cast <char> ((value & 0x7F) | 0x80);

    if (!ensureCapacity (dynamicNumberSize + 1)) DDException (DDE_BUFFER_OVERFLOW, dynamicNumberSize + 1, offset, length);
    for (; dynamicNumberSize >= 0; dynamicNumberSize--) buffer [offset++] = tempBuffer [dynamicNumberSize];
}

//  +-----------+
//  | writeTime |
//  +-----------+

void DDBuffer::writeTime (int value)
{
    int ms  = value % 1000;    value /= 1000;
    int sec = value % 100;     value /= 100;
    int min = value % 100;     value /= 100;

    value = (value << 6) | min;
    value = (value << 6) | sec;

    if (!ms) value =  value << 1;
    else     value = (value << 11) | (ms << 1) | 1;

    writeNumber (value, false);
}

//  +-----------+
//  | writeDate |
//  +-----------+

void DDBuffer::writeDate (int value)
{
    int day = value % 100;    value /= 100;
    int mon = value % 100;    value /= 100;

    value = (value << 4) | mon;
    value = (value << 5) | day;

    writeNumber (value, true);
}

//  +---------------+
//  | writeDateTime |
//  +---------------+

void DDBuffer::writeDateTime (qint64 value)
{
    int ms   = value % 1000;    value /= 1000;
    int sec  = value % 100;     value /= 100;
    int min  = value % 100;     value /= 100;
    int hour = value % 100;     value /= 100;
    int day  = value % 100;     value /= 100;
    int mon  = value % 100;     value /= 100;

    value = (value << 4) | mon;
    value = (value << 5) | day;
    value = (value << 5) | hour;
    value = (value << 6) | min;
    value = (value << 6) | sec;

    if (!ms) value =  value << 1;
    else     value = (value << 11) | (ms << 1) | 1;

    writeNumber (value, true);
}

//  +------------+
//  | writeBytes |
//  +------------+

void DDBuffer::writeBytes (QByteArray &value)
{
    int size  =  value.size ();
    writeNumber (size, false);

    if (size > 0)
    {
        if (!ensureCapacity (size)) DDException (DDE_BUFFER_OVERFLOW, size, offset, length);
        memcpy (buffer + offset, value.data (), static_cast <size_t> (size));
        offset += size;
    }
}

//  +-------------+
//  | writeString |
//  +-------------+

void DDBuffer::writeString (QString &value)
{
    int             i,  size;
    int             j = value.size ();
    QChar          *c = value.data ();
    unsigned short  cu;

    for (size = j, i = 0; i < j; i++)
        if (c [i] > 0x7F) size += c [i] > 0x3FFF ? 2 : 1;    // Calculate size

    writeNumber (size,  false);
    if (!ensureCapacity (size)) DDException (DDE_BUFFER_OVERFLOW, size, offset, length);

    for (i = 0; i < j; i++)
    {
        cu = c [i].unicode ();

        if (cu > 0x7F)
        {
            if (cu > 0x3FFF) buffer [offset++] = static_cast <char> (((cu >> 14) & 0x7F) | 0x80);
            buffer [offset++] = static_cast <char> (((cu >> 7) & 0x7F) | 0x80);
        }

        buffer [offset++] = static_cast <char> (cu & 0x7F);
    }
}

//  +------------------+
//  | writeArrayBitmap |
//  +------------------+

void DDBuffer::writeArrayBitmap (int pos, int bmp, int bm)
{
    if (pos) buffer [bmp] = static_cast <char> (bm);
    if (offset >= length) DDException (DDE_BUFFER_OVERFLOW, 1, offset, length);
}

//  +----------------+
//  | writeArrayInfo |
//  +----------------+

void DDBuffer::writeArrayInfo (int size, int numbers, int type)
{
    int x = size < numbers ? 0x10 : 0;

    writeNumber ((size << 5) | x | type, false);
    if (x) writeNumber  (numbers - size, false);
}

//  #================#
//  # writeTypeAndId #
//  #================#

void DDBuffer::writeTypeAndId (int typeAndId)
{
    if (!ensureCapacity (2)) DDException (DDE_BUFFER_OVERFLOW, 2, offset, length);

    buffer [offset++] = static_cast <char> (typeAndId >> 8);
    buffer [offset++] = static_cast <char> (typeAndId);
}

//  #=============#
//  # isValidTime #
//  #=============#

bool DDBuffer::isValidTime (int data)
{
    if (!data) return true;

    data   /= 1000;
    int sec = data %  100;    data /= 100;
    int min = data %  100;    data /= 100;

    return sec < 60 && min < 60 && data >= 0 && data < 24;
}

//  #=============#
//  # isValidDate #
//  #=============#

bool DDBuffer::isValidDate (int data)
{
    if (!data) return true;

    int day = data % 100;    data /= 100;
    int mon = data % 100;    data /= 100;

    int year = data < 0 ? -data - 1 : data;

    return data && year <= 214747 && mon && mon <= 12 && day &&
          (day <= DDBuffer::MONTH_DAYS [mon - 1] || (day == 29 && mon == 2 && isLeapYear (year)));
}

//  #=================#
//  # isValidDateTime #
//  #=================#

bool DDBuffer::isValidDateTime (qint64 data)
{
    if (!data) return true;

    data    /= 1000;
    int sec  = data % 100;     data /= 100;
    int min  = data % 100;     data /= 100;
    int hour = data % 100;     data /= 100;
    int day  = data % 100;     data /= 100;
    int mon  = data % 100;     data /= 100;

    int year = static_cast <int> (data < 0 ? -data - 1 : data);

    return sec < 60 && min < 60 && hour < 24 && data && year <= 214747 && mon && mon <= 12 && day &&
          (day <= DDBuffer::MONTH_DAYS [mon - 1] || (day == 29 && mon == 2 && isLeapYear (year)));
}

//  #============#
//  # stringData #
//  #============#

#define STRING_ARRAY_1(vectorType) \
{ \
    for (; i < len; i++) \
    { \
        if (sep) QTextStream (str) << ','; \
        sep = true; \
\
        QTextStream (str) << static_cast <int> (static_cast <QVector <vectorType> *> (data)->data () [i]); \
    } \
}

#define STRING_ARRAY_2(vectorType) \
{ \
    for (; i < len; i++) \
    { \
        if (sep) QTextStream (str) << ','; \
        sep = true; \
\
        QTextStream (str) << static_cast <QVector <vectorType> *> (data)->data () [i]; \
    } \
}

#define STRING_ARRAY_3(vectorType,method) \
{ \
    for (; i < len; i++) \
    { \
        if (sep) QTextStream (str) << ','; \
        sep = true; \
\
        if (              !static_cast <QVector <vectorType> *> (data)->data () [i].isNull ()) \
             method (str, &static_cast <QVector <vectorType> *> (data)->data () [i]);} \
}

#define STRING_ARRAY_4(vectorType,method) \
{ \
    for (; i < len; i++) \
    { \
        if (sep) QTextStream (str) << ','; \
        sep = true; \
\
        if (              static_cast <QVector <vectorType> *> (data)->data () [i]) \
             method (str, static_cast <QVector <vectorType> *> (data)->data () [i]);} \
}

void DDBuffer::stringData (QString *str, void *data, int type)
{
    switch (type & 0x0F)
    {
        default: QTextStream (str) << reinterpret_cast <qint64> (data); return;    // byte, short, int, long, number, signedNumber

        case DDC_FLOAT:
        {
            union {float fv; int iv;};
            iv = static_cast <int > (reinterpret_cast <qint64> (data));
            QTextStream (str) << fv;
            return;
        }
        case DDC_DOUBLE:
        {
            union {double dv; qint64 lv;};
            lv = reinterpret_cast <qint64> (data);
            QTextStream (str) << dv;
            return;
        }
        case DDC_TIME:     stringTime     (str, static_cast <int> (reinterpret_cast <qint64> (data))); return;
        case DDC_DATE:     stringDate     (str, static_cast <int> (reinterpret_cast <qint64> (data))); return;
        case DDC_DATETIME: stringDateTime (str,                    reinterpret_cast <qint64> (data));  return;

        case DDC_BYTES:    stringBytes    (str, static_cast <QByteArray*> (data)); return;
        case DDC_STRING:   stringString   (str, static_cast <QString   *> (data)); return;

        case DDC_ARRAY:
        {
            int len = static_cast <QVector <int> *> (data)->size ();    // any type, needs anly to get size
            type  >>= 4;

            QTextStream (str) << len << DDBuffer::DATA_TYPES [type] << '[';
            bool sep = false;
            int  i   = 0;

            switch (type)
            {
                default:           STRING_ARRAY_2 (qint64);    break;   // long, number, signedNumber
                case DDC_BYTE:     STRING_ARRAY_1 (char);      break;
                case DDC_SHORT:    STRING_ARRAY_2 (short);     break;
                case DDC_INT:      STRING_ARRAY_2 (int);       break;
                case DDC_FLOAT:    STRING_ARRAY_2 (float);     break;
                case DDC_DOUBLE:   STRING_ARRAY_2 (double);    break;

                case DDC_TIME:     STRING_ARRAY_4 (int,        stringTime);     break;
                case DDC_DATE:     STRING_ARRAY_4 (int,        stringDate);     break;
                case DDC_DATETIME: STRING_ARRAY_4 (qint64,     stringDateTime); break;
                case DDC_BYTES:    STRING_ARRAY_3 (QByteArray, stringBytes);    break;
                case DDC_STRING:   STRING_ARRAY_3 (QString,    stringString);
            }

            QTextStream (str) << ']';
            return;
        }

        case DDC_LIST: static_cast <DDList *> (data)->toString (str);
    }
}

//  +------------+
//  | stringTime |
//  +------------+

void DDBuffer::stringTime (QString *str, int data)
{
    int ms  = data % 1000;    data /= 1000;
    int sec = data % 100;     data /= 100;
    int min = data % 100;     data /= 100;

    snprintf (tempBuffer, sizeof (tempBuffer), "%02d:%02d:%02d.%03d", data, min, sec, ms);
    QTextStream (str) <<  tempBuffer;
}

//  +------------+
//  | stringDate |
//  +------------+

void DDBuffer::stringDate (QString *str, int data)
{
    int day = data % 100;    data /= 100;
    int mon = data % 100;    data /= 100;

    snprintf (tempBuffer, sizeof (tempBuffer), "%04d-%02d-%02d", data, mon, day);
    QTextStream (str) <<  tempBuffer;
}

//  +----------------+
//  | stringDateTime |
//  +----------------+

void DDBuffer::stringDateTime (QString *str, qint64 data)
{
    int ms   = data % 1000;    data /= 1000;
    int sec  = data % 100;     data /= 100;
    int min  = data % 100;     data /= 100;
    int hour = data % 100;     data /= 100;
    int day  = data % 100;     data /= 100;
    int mon  = data % 100;     data /= 100;

    snprintf (tempBuffer, sizeof (tempBuffer), "%04lld-%02d-%02d_%02d:%02d:%02d.%03d", data, mon, day, hour, min, sec, ms);
    QTextStream (str) <<  tempBuffer;
}

//  +-------------+
//  | stringBytes |
//  +-------------+

void DDBuffer::stringBytes (QString *str, QByteArray *data)
{
    QTextStream (str) << data->size () << 'x';
    char c,  h,  l;

    for (int i = 0; i < data->size (); i++)
    {
        c =  data->data () [i];
        l =  c       & 0x0F;    l = l > 9 ? l + 55 : l + 48;
        h = (c >> 4) & 0x0F;    h = h > 9 ? h + 55 : h + 48;

        QTextStream (str) << h << l;
    }
}

//  +--------------+
//  | stringString |
//  +--------------+

void DDBuffer::stringString (QString *str, QString *data)
{
    QTextStream (str) << data->size () << '"';

    unsigned short x;
    QChar          c;
    char           hh, ll, h, l;

    for (int i = 0; i < data->size (); i++)
        if ((c = data->at (i)) >= ' ' && c <= '~' && c != '\\') QTextStream (str) << c;
        else
        {
            QTextStream (str) << '\\';

            if (((x = c.unicode ()) & 0xFF00))
            {
                hh =  x >> 12;            hh = hh > 9 ? hh + 55 : hh + 48;
                ll = (x >>  8) & 0x0F;    ll = ll > 9 ? ll + 55 : ll + 48;

                QTextStream (str) << '\\' << hh << ll;
            }

            h = (x >> 4) & 0x0F;    h = h > 9 ? h + 55 : h + 48;
            l =  x       & 0x0F;    l = l > 9 ? l + 55 : l + 48;

            QTextStream (str) << h << l;
        }

    QTextStream (str) << '"';
}
