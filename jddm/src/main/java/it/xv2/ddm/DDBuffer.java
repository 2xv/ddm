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

package it.xv2.ddm;

import java.util.zip.CRC32;

final class DDBuffer
{                                       //  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
    final static char   [] DATA_TYPES   = {'z','b','s','i','I','n','N','f','F','t','d','D','B','S','A','L'};
    final static byte   [] MONTH_DAYS   = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    final static String [] STRING_TYPES = {"Null","Byte","Short","Integer","Long","Number","SignedNumber","Float",
                                           "Double","Time","Date","DateTime","Bytes","String","Array","List"};

    private final byte  [] tempBuffer = new byte [10];
    private byte        [] buffer;
    private int            length;
    private int            mark;
    private int            offset;
    private final int      start;

//  #=============#
//  # Constructor #
//  #=============#

    DDBuffer (byte [] buffer, int offset, int length)
    {
        this.buffer = buffer;
        this.length = length;
        this.offset = offset;
        this.start  = offset;
    }

//  #=====#
//  # get #
//  #=====#

    byte [] getBuffer () {return buffer;}
    int     getOffset () {return offset;}

//  #=============#
//  # endOfBuffer #
//  #=============#

    boolean endOfBuffer () {return offset >= length;}

//  +----------------+
//  | ensureCapacity |
//  +----------------+

    private boolean ensureCapacity (int bytesNumber)
    {
        if ((offset + bytesNumber) <= length) return true;
        if  (buffer.length         >= 0xFFFF) return false;

        int newSize = buffer.length  << 1;
        if (newSize > 0xFFFF) newSize = 0xFFFF;

        byte [] newBuffer = new byte [newSize];
        System.arraycopy (buffer, 0,  newBuffer, 0, offset);

        length += newSize - buffer.length;
        buffer  = newBuffer;

        return (offset + bytesNumber) <= length;
    }

//  +----------------+
//  | unexpectedData |
//  +----------------+

    static void unexpectedData (int id, int expected, int found) throws DDException
    {
        int expectedIdH = expected >> 4;
        int foundIdH    = found    >> 4;

        throw new DDException (DDError.INVALID_DATA_TYPE.getMessage (id,
            (expectedIdH == 0 ? "" : DDBuffer.STRING_TYPES [expectedIdH]) + DDBuffer.STRING_TYPES [expected & 0x0F],
            (foundIdH    == 0 ? "" : DDBuffer.STRING_TYPES [foundIdH])    + DDBuffer.STRING_TYPES [found    & 0x0F]));
    }

//  #=================#
//  # addProtocolInfo #
//  #=================#

    void addProtocolInfo () throws DDException
    {
        if (!ensureCapacity (4)) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (4, offset, length));

        buffer [offset++] = 'D';
        buffer [offset++] = 'D';
        buffer [offset++] = 'P';
        buffer [offset++] =  0x10;
    }

//  #===================#
//  # checkProtocolInfo #
//  #===================#

    void checkProtocolInfo () throws DDException
    {
        if (offset > length - 4) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (4, offset, length));

        int id = (buffer [offset + 3] >> 4) & 0x0F;    // id = Version 0001-0000 -> valid 0001-????

        if (buffer [offset] != 'D' || buffer [offset + 1] !=  'D' || buffer [offset + 2] != 'P' || id != 1)
            throw new DDException (DDError.INVALID_PROTOCOL_INFO.getMessage (Integer.toHexString ((buffer [offset] << 20) |
                                  (buffer [offset + 1] << 12) | (buffer [offset + 2] << 4) | id)));
        offset += 4;
    }

//  #==========#
//  # addCRC32 #
//  #==========#

    int addCRC32 () throws DDException
    {
        if (!ensureCapacity (4)) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (4, offset, length));

        CRC32 crc32 = new CRC32 ();
        crc32.update (buffer, start, offset - start);

        int crc = (int) crc32.getValue ();

        buffer [offset++] = (byte)(crc >> 24);
        buffer [offset++] = (byte)(crc >> 16);
        buffer [offset++] = (byte)(crc >>  8);
        buffer [offset++] = (byte) crc;

        return offset;
    }

//  #=============#
//  # checkCRC32  #
//  #=============#

    void checkCRC32 () throws DDException
    {
        int len = length - start - 4;
        if (len < 0) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (len, start, length - 4));

        int crcFound = ((buffer [length - 4] & 0xFF) << 24) | ((buffer [length - 3] & 0xFF) << 16) |
                       ((buffer [length - 2] & 0xFF) <<  8) |  (buffer [length - 1] & 0xFF);

        CRC32 crc32 = new CRC32 ();
        crc32.update (buffer, start, len);
 
        int crcExpected = (int) crc32.getValue ();

        if (crcFound != crcExpected)
            throw new DDException (DDError.INVALID_CRC.getMessage (Integer.toHexString (crcExpected),
                                                                   Integer.toHexString (crcFound)));
        length -= 4;
    }

//  #======#
//  # read #
//  #======#

    Object read (int [] dataType) throws DDException
    {
        switch (dataType [0] & 0x0F)
        {
            case DDConst.BYTE:     return        readByte         ();
            case DDConst.SHORT:    return        readShort        ();
            case DDConst.INT:      return        readInt          ();
            case DDConst.LONG:     return        readLong         ();
            case DDConst.NUMBER:   return        readNumber       (false);
            case DDConst.S_NUMBER: return        readNumber       (true);
            case DDConst.FLOAT:    return  Float.intBitsToFloat   (readInt  ());
            case DDConst.DOUBLE:   return Double.longBitsToDouble (readLong ());
            case DDConst.TIME:     return        readTime         ();
            case DDConst.DATE:     return        readDate         ();
            case DDConst.DATETIME: return        readDateTime     ();
            case DDConst.BYTES:    return        readBytes        ();
            case DDConst.STRING:   return        readString       ();

            case DDConst.ARRAY:
            {
                long v               =  readNumber (false);
                int  itemRemovedFlag = (int) v & 0x10;                //  Item Removed Flag
                int  itemType        = (int) v & 0x0F;                //  Item Type
                v                  >>=  5;                            //  Item Numbers
                int  size            = (int) v;

                if (itemRemovedFlag != 0) v += readNumber (false);    // +Item Removed Numbers

                if (itemType <= DDConst.NULL || itemType >= DDConst.ARRAY || v > DDConst.MAX_ITEMS_NUMBER)
                    throw new DDException (DDError.INVALID_ARRAY_ITEM.getMessage (itemType, v, mark, length));

                int    itemNumbers = (int) v;
                int    i = 0,  bm  =  0;
                Object obj;

                switch (itemType)
                {
                    case DDConst.BYTE:

                        for (obj = new byte [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((byte []) obj) [i] = readByte ();
                        }

                        break;

                    case DDConst.SHORT:

                        for (obj = new short [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((short []) obj) [i] = readShort ();
                        }

                        break;

                    case DDConst.INT:

                        for (obj = new int [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((int []) obj) [i] = readInt ();
                        }

                        break;

                    case DDConst.LONG:

                        for (obj = new long [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((long []) obj) [i] = readLong ();
                        }

                        break;

                    case DDConst.FLOAT:

                        for (obj = new float [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((float []) obj) [i] = Float.intBitsToFloat (readInt ());
                        }

                        break;

                    case DDConst.DOUBLE:

                        for (obj = new double [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((double []) obj) [i] = Double.longBitsToDouble (readLong ());
                        }

                        break;

                    case DDConst.NUMBER:

                        for (obj = new long [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((long []) obj) [i] = readNumber (false);
                        }

                        break;

                    case DDConst.S_NUMBER:

                        for (obj = new long [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((long []) obj) [i] = readNumber (true);
                        }

                        break;

                    case DDConst.TIME:

                        for (obj = new int [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((int []) obj) [i] = readTime ();
                        }

                        break;

                    case DDConst.DATE:

                        for (obj = new int [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((int []) obj) [i] = readDate ();
                        }

                        break;

                    case DDConst.DATETIME:

                        for (obj = new long [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((long []) obj) [i] = readDateTime ();
                        }

                        break;

                    case DDConst.BYTES:

                        for (obj = new byte [itemNumbers][]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((byte [][]) obj) [i] = readBytes ();
                        }

                        break;

                    case DDConst.STRING:

                        for (obj = new String [itemNumbers]; i < size; i++, bm <<= 1)
                        {
                            if ((i & 7) == 0) bm = readArrayBitmap ();
                            if ((bm & 0x80) != 0) ((String []) obj) [i] = readString ();
                        }

                        break;

                    default:

                        throw new DDException (DDError.INVALID_VALUE.getMessage ("Data Type",
                                               STRING_TYPES [itemType] + STRING_TYPES [DDConst.ARRAY],
                                               dataType [0] & 0xFF, mark, length));
                }

                dataType [0] |= (itemType << 4);
                return obj;
            }

            case DDConst.LIST:
            {
                long v = readNumber (false);

                if (v > DDConst.MAX_ITEMS_NUMBER)
                    throw new DDException (DDError.INVALID_SIZE.getMessage ("List Size", v,
                                           DDConst.MAX_ITEMS_NUMBER, DDConst.LIST, mark, length));

                int [] objectType = new int [1];
                DDList list       = new DDList ();

                int size = (int) v;
                int k;

                for (int i = 0, j = 0; i < size; i++)
                {
                    if ((i & 1) > 0) k = j & 0x0F;
                    else
                    {
                        if (offset >= length) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (1, offset, length));
                        j = buffer [offset++] & 0xFF;
                        k = j >> 4;
                    }

                    objectType  [0]  = k;
                    list.addData (k == DDConst.NULL ? null : read (objectType), objectType [0]);
                }

                return list;
            }

            default: throw new DDException (DDError.INVALID_VALUE.getMessage
                                           ("Data Type", STRING_TYPES [DDConst.NULL], DDConst.NULL, mark, length));
        }
    }

//  +----------+
//  | readByte |
//  +----------+

    private byte readByte () throws DDException
    {
        if (offset >= length) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (1, offset, length));
        return buffer [offset++];
    }

//  +-----------+
//  | readShort |
//  +-----------+

    private short readShort () throws DDException
    {
        if ((offset + 2) > length) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (2, offset, length));

        int  v =        buffer [offset++] & 0xFF;
        v = (v << 8) | (buffer [offset++] & 0xFF);

        return (short) v;
    }

//  +---------+
//  | readInt |
//  +---------+

    private int readInt () throws DDException
    {
        if ((offset + 4) > length) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (4, offset, length));

        int  v =        buffer [offset++] & 0xFF;
        v = (v << 8) | (buffer [offset++] & 0xFF);
        v = (v << 8) | (buffer [offset++] & 0xFF);
        v = (v << 8) | (buffer [offset++] & 0xFF);

        return v;
    }

//  +----------+
//  | readLong |
//  +----------+

    private long readLong () throws DDException
    {
        if ((offset + 8) > length) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (8, offset, length));

        long v =        buffer [offset++] & 0xFF;
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

    private long readNumber (boolean withSign) throws DDException
    {
        int len = length - offset;
        mark    = offset;

        if (len > 0)
        {
            byte b     = -1;
            byte fb    =  buffer [offset];
            int  max   =  withSign ? 10 : 9;
            long value =  0;

            if (len > max) len = max;
            int lc  = len;

            while (--len >= 0 && b < 0)
            {
                b     =  buffer [offset++];
                value = (value << 7) | (b & 0x7F);
            }

            if (b >= 0)
            {
                if (!withSign) return value;
 
                if (len > 0 || lc < 10 || (fb & 0x7E) == 0)
                {
                    value = (value & 1) > 0 ? ~(value >>> 1) : value >>> 1;
                    return   value;
                }
            }

            if (offset <= length)
                throw new DDException (DDError.INVALID_DYNAMIC_NUMBER.getMessage
                                      (Integer.toHexString (fb), Integer.toHexString (b), mark, length));
        }

        throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (1, mark, length));
    }

//  +----------+
//  | readTime |
//  +----------+

    private int readTime () throws DDException
    {
        long v =  readNumber (false);
        if  (v == 0) return   0;

        int ms = (int) v & 1;

        v   >>=  1;
        if (ms > 0) {ms = (int) v & 0x3FF; v >>= 10;}

        int sec = (int) v & 0x3F;    v >>= 6;
        int min = (int) v & 0x3F;    v >>= 6;

        if (ms < 1000 && sec < 60 && min < 60 && v >= 0 && v < 24)
            return (((int) v * 100 + min) * 100 + sec) * 1000 + ms;

        throw new DDException (DDError.INVALID_VALUE.getMessage ("Time",
                  String.format ("%02d:%02d:%02d.%03d", (int)(v), min, sec, ms), DDConst.TIME, mark, length));
    }

//  +----------+
//  | readDate |
//  +----------+

    private int readDate () throws DDException
    {
        long v =  readNumber (true);
        if  (v == 0) return   0;

        int day = (int) v & 0x1F;    v >>= 5;
        int mon = (int) v & 0x0F;    v >>= 4;

        long year = v < 0 ? -v - 1 : v;

        if (v != 0 && year <= 214747 && mon != 0 && mon <= 12 && day != 0 && (day <= MONTH_DAYS [mon - 1] ||
           (mon == 2 && day == 29 && (year % 400) == 0 || ((year % 4) == 0 && (year % 100) != 0))))    // Leap Year
                return ((int) v * 100 + mon) * 100 + day;

        throw new DDException (DDError.INVALID_VALUE.getMessage ("Date",
                               String.format ("%04d-%02d-%02d", (int)(v), mon, day), DDConst.DATE, mark, length));
    }

//  +--------------+
//  | readDateTime |
//  +--------------+

    private long readDateTime () throws DDException
    {
        long v =  readNumber (true);
        if  (v == 0) return   0;

        int ms = (int) v & 1;

        v   >>=  1;
        if (ms > 0) {ms = (int) v & 0x3FF; v >>= 10;}

        int sec  = (int) v & 0x3F;    v >>= 6;
        int min  = (int) v & 0x3F;    v >>= 6;
        int hour = (int) v & 0x1F;    v >>= 5;
        int day  = (int) v & 0x1F;    v >>= 5;
        int mon  = (int) v & 0x0F;    v >>= 4;

        long year = v < 0 ? -v - 1 : v;

        if (ms < 1000 && sec < 60 && min < 60 && hour < 24 &&
            v != 0 && year <= 214747 && mon != 0 && mon <= 12 && day != 0 && (day <= MONTH_DAYS [mon - 1] ||
           (mon == 2 && day == 29 && (year % 400) == 0 || ((year % 4) == 0 && (year % 100) != 0))))    // Leap Year
                return ((((((v * 100 + mon) * 100 + day) * 100) + hour) * 100 + min) * 100 + sec) * 1000 + ms;

        throw new DDException (DDError.INVALID_VALUE.getMessage ("DateTime",
                               String.format ("%04d-%02d-%02d_%02d:%02d:%02d.%03d", (int)(v), mon, day,
                                               hour, min, sec, ms), DDConst.DATETIME, mark, length));
    }

//  +-----------+
//  | readBytes |
//  +-----------+

    private byte [] readBytes () throws DDException
    {
        long v = readNumber (false);

        if (v > DDConst.MAX_BYTES_NUMBER)
            throw new DDException (DDError.INVALID_SIZE.getMessage
                                  ("Bytes Size", v, DDConst.MAX_BYTES_NUMBER, DDConst.BYTES, mark, length));
        int size = (int) v;
        mark     =  offset;

        if ((offset += size) > length) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (size, mark, length));

        byte [] bytes = new byte [size];
        if (size > 0) System.arraycopy (buffer, mark, bytes, 0, size);
        return bytes;
    }

//  +------------+
//  | readString |
//  +------------+

    private String readString () throws DDException
    {
        long v = readNumber (false);

        if (v > DDConst.MAX_BYTES_NUMBER)
            throw new DDException (DDError.INVALID_SIZE.getMessage
                                  ("String Size", v, DDConst.MAX_BYTES_NUMBER, DDConst.STRING, mark, length));
        int size = (int) v;
        mark     =  offset;

        if ((offset += size) > length) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (size, mark, length));

        if (size > 0 && buffer [offset - 1] < 0)
            throw new DDException (DDError.INVALID_VALUE.getMessage ("String Termination", "0x" +
                                   Integer.toHexString (buffer [offset - 1]), DDConst.STRING, mark, length));
        byte b;
        int  i, j;
        char [] c = new char [size << 1];

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
                        throw new DDException (DDError.INVALID_VALUE.getMessage ("String Item", "0x" +
                                               Integer.toHexString (i), DDConst.STRING, mark - 3, length));
                    }
                }
            }

            c [j++] = (char) i;
        }

        return new String (c, 0, size);
    }

//  +-----------------+
//  | readArrayBitmap |
//  +-----------------+

    private int readArrayBitmap () throws DDException
    {
        if (offset >= length) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (1, offset, length));
        return buffer [offset++] & 0xFF;
    }

//  #===============#
//  # readTypeAndId #
//  #===============#

    int readTypeAndId () throws DDException
    {
        if (offset > length - 2) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (2, offset, length));

        int res = (buffer [offset++] & 0xFF) << 8;
        return (res | (buffer [offset++] & 0xFF));
    }

//  #=======#
//  # write #
//  #=======#

    void write (Object data, int type) throws DDException
    {
        switch (type & 0x0F)
        {
            case DDConst.BYTE:     writeByte     ((byte)    data);                           return;
            case DDConst.SHORT:    writeShort    ((short)   data);                           return;
            case DDConst.INT:      writeInt      ((int)     data);                           return;
            case DDConst.LONG:     writeLong     ((long)    data);                           return;
            case DDConst.NUMBER:   writeNumber   ((long)    data, false);                    return;
            case DDConst.S_NUMBER: writeNumber   ((long)    data, true);                     return;
            case DDConst.FLOAT:    writeInt       (Float.floatToIntBits    ((Float)  data)); return;
            case DDConst.DOUBLE:   writeLong      (Double.doubleToLongBits ((Double) data)); return;
            case DDConst.TIME:     writeTime     ((int)     data);                           return;
            case DDConst.DATE:     writeDate     ((int)     data);                           return;
            case DDConst.DATETIME: writeDateTime ((long)    data);                           return;
            case DDConst.BYTES:    writeBytes    ((byte []) data);                           return;
            case DDConst.STRING:   writeString   ((String)  data);                           return;

            case DDConst.ARRAY:
            {
                type >>= 4;
                int  i,      size;
                int  bm = 0, bmp = 0;

                switch (type)
                {
                    case DDConst.BYTE:

                        for (i = ((byte []) data).length, size = i; size > 0 && ((byte []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if              (((byte []) data) [i] == 0) bm |= 0;
                            else {writeByte (((byte []) data) [i]);     bm |= 1;}
                        }

                        break;

                    case DDConst.SHORT:

                        for (i = ((short []) data).length, size = i; size > 0 && ((short []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if               (((short []) data) [i] == 0) bm |= 0;
                            else {writeShort (((short []) data) [i]);     bm |= 1;}
                        }

                        break;

                    case DDConst.INT:

                        for (i = ((int []) data).length, size = i; size > 0 && ((int []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if             (((int []) data) [i] == 0) bm |= 0;
                            else {writeInt (((int []) data) [i]);     bm |= 1;}
                        }

                        break;

                    case DDConst.LONG:

                        for (i = ((long []) data).length, size = i; size > 0 && ((long []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if              (((long []) data) [i] == 0) bm |= 0;
                            else {writeLong (((long []) data) [i]);     bm |= 1;}
                        }

                        break;

                    case DDConst.FLOAT:

                        for (i = ((float []) data).length, size = i; size > 0 && ((float []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if                                   (((float  []) data) [i] == 0) bm |= 0;
                            else {writeInt (Float.floatToIntBits (((float  []) data) [i]));    bm |= 1;}
                        }

                        break;

                    case DDConst.DOUBLE:

                        for (i = ((double []) data).length, size = i; size > 0 && ((double []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if                                       (((double []) data) [i] == 0) bm |= 0;
                            else {writeLong (Double.doubleToLongBits (((double []) data) [i]));    bm |= 1;}
                        }

                        break;

                    case DDConst.NUMBER:

                        for (i = ((long []) data).length, size = i; size > 0 && ((long []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if                (((long []) data) [i] == 0)    bm |= 0;
                            else {writeNumber (((long []) data) [i], false); bm |= 1;}
                        }

                        break;

                    case DDConst.S_NUMBER:

                        for (i = ((long []) data).length, size = i; size > 0 && ((long []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if                (((long []) data) [i] == 0)    bm |= 0;
                            else {writeNumber (((long []) data) [i], true);  bm |= 1;}
                        }

                        break;

                    case DDConst.TIME:

                        for (i = ((int []) data).length, size = i; size > 0 && ((int []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if              (((int []) data) [i] == 0) bm |= 0;
                            else {writeTime (((int []) data) [i]);     bm |= 1;}
                        }

                        break;

                    case DDConst.DATE:

                        for (i = ((int []) data).length, size = i; size > 0 && ((int []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if              (((int []) data) [i] == 0) bm |= 0;
                            else {writeDate (((int []) data) [i]);     bm |= 1;}
                        }

                        break;

                    case DDConst.DATETIME:

                        for (i = ((long []) data).length, size = i; size > 0 && ((long []) data) [size - 1] == 0;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if                  (((long []) data) [i] == 0) bm |= 0;
                            else {writeDateTime (((long []) data) [i]);        bm |= 1;}
                        }

                        break;

                    case DDConst.BYTES:

                        for (i = ((byte [][]) data).length, size = i; size > 0 && ((byte [][]) data) [size - 1] == null;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                                bm <<= 1;

                            if               (((byte [][]) data) [i] == null) bm |= 0;
                            else {writeBytes (((byte [][]) data) [i]);        bm |= 1;}
                        }

                        break;

                    default:

                        for (i = ((String []) data).length, size = i; size > 0 && ((String []) data) [size - 1] == null;) {size--;}
                        writeArrayInfo (size, i, type);

                        for (i = 0; i < size; i++)
                        {
                            if ((i & 7) == 0) {writeArrayBitmap (i, bmp, bm); bmp = offset++; bm = 0;}
                            bm <<= 1;

                            if                (((String []) data) [i] == null) bm |= 0;
                            else {writeString (((String []) data) [i]);        bm |= 1;}
                        }
                }

                if (i > 0)
                {
                    if ((i  % 8) > 0) bm <<= (8 - i % 8);
                    buffer [bmp] = (byte) bm;
                }

                return;
            }

            default:    // case DDConst.LIST:
            {
                DDList list = (DDList) data;
                writeNumber (list.getDataNumber (), false);

                Object obj;
                int    j, k, i = -1;

                list.begin ();

                while (!list.isEndOfList ())
                {
                    obj = list.getData (-1);
                    j   = list.getCurrentDataType ();
                    k   = j & 0x0F;

                    if (i >= 0)
                    {
                        buffer [i] |= k;
                        i = -1;
                    }
                    else
                    {
                        if (!ensureCapacity (1))
                             throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (1, offset, length));

                        i = offset;
                        buffer [offset++] = (byte)(k << 4);
                    }

                    if (j != DDConst.NULL) write (obj, j);
                    list.next ();
                }
            }
        }
    }

//  +-----------+
//  | writeByte |
//  +-----------+

    private void writeByte (byte value) throws DDException
    {
        if (!ensureCapacity (1)) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (1, offset, length));
        buffer [offset++] = value;
    }

//  +------------+
//  | writeShort |
//  +------------+

    private void writeShort (short value) throws DDException
    {
        if (!ensureCapacity (2)) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (2, offset, length));

        buffer [offset + 1] = (byte) value;    value >>= 8;
        buffer [offset]     = (byte) value;
        offset += 2;
    }

//  +----------+
//  | writeInt |
//  +----------+

    private void writeInt (int value) throws DDException
    {
        if (!ensureCapacity (4)) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (4, offset, length));

        buffer [offset + 3] = (byte) value;    value >>= 8;
        buffer [offset + 2] = (byte) value;    value >>= 8;
        buffer [offset + 1] = (byte) value;    value >>= 8;
        buffer [offset]     = (byte) value;
        offset += 4;
    }

//  +-----------+
//  | writeLong |
//  +-----------+

    private void writeLong (long value) throws DDException
    {
        if (!ensureCapacity (8)) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (8, offset, length));

        buffer [offset + 7] = (byte) value;    value >>= 8;
        buffer [offset + 6] = (byte) value;    value >>= 8;
        buffer [offset + 5] = (byte) value;    value >>= 8;
        buffer [offset + 4] = (byte) value;    value >>= 8;
        buffer [offset + 3] = (byte) value;    value >>= 8;
        buffer [offset + 2] = (byte) value;    value >>= 8;
        buffer [offset + 1] = (byte) value;    value >>= 8;
        buffer [offset]     = (byte) value;
        offset += 8;
    }

//  +-------------+
//  | writeNumber |
//  +-------------+

    private void writeNumber (long value, boolean withSign) throws DDException
    {
        int dynamicNumberSize = 0;
        if (withSign) value = value < 0 ? ((~value) << 1) | 1 : value << 1;

        for (tempBuffer [0] = (byte)(value & 0x7F), value >>>= 7; value != 0; value >>>= 7)
             tempBuffer [++dynamicNumberSize] = (byte)((value & 0x7F) | 0x80);

        if (!ensureCapacity (dynamicNumberSize + 1))
            throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (dynamicNumberSize + 1, offset, length));

        for (; dynamicNumberSize >= 0; dynamicNumberSize--) buffer [offset++] = tempBuffer [dynamicNumberSize];
    }

//  +-----------+
//  | writeTime |
//  +-----------+

    private void writeTime (int value) throws DDException
    {
        int ms  = value % 1000;    value /= 1000;
        int sec = value % 100;     value /= 100;
        int min = value % 100;     value /= 100;

        value = (value << 6) | min;
        value = (value << 6) | sec;

        if (ms > 0) value = (value << 11) | (ms << 1) | 1;
        else        value =  value << 1;

        writeNumber (value, false);
    }

//  +-----------+
//  | writeDate |
//  +-----------+

    private void writeDate (int value) throws DDException
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

    private void writeDateTime (long value) throws DDException
    {
        int ms   = (int)(value % 1000);    value /= 1000;
        int sec  = (int)(value % 100);     value /= 100;
        int min  = (int)(value % 100);     value /= 100;
        int hour = (int)(value % 100);     value /= 100;
        int day  = (int)(value % 100);     value /= 100;
        int mon  = (int)(value % 100);     value /= 100;

        value = (value << 4) | mon;
        value = (value << 5) | day;
        value = (value << 5) | hour;
        value = (value << 6) | min;
        value = (value << 6) | sec;

        if (ms > 0) value = (value << 11) | (ms << 1) | 1;
        else        value =  value << 1;

        writeNumber (value, true);
    }

//  +------------+
//  | writeBytes |
//  +------------+

    private void writeBytes (byte [] value) throws DDException
    {
        int size = value.length;
        writeNumber (size, false);

        if (size > 0)
        {
            if (!ensureCapacity (size)) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (size, offset, length));
            System.arraycopy (value, 0, buffer, offset, size);
            offset += size;
        }
    }

//  +-------------+
//  | writeString |
//  +-------------+

    private void writeString (String value) throws DDException
    {
        char c;
        int  i,  size;
        int  j = value.length ();

        for (size = j, i = 0; i < j; i++)
             if ((c = value.charAt (i)) > 0x7F) size += c > 0x3FFF ? 2 : 1;    // Calculate size

        writeNumber (size,  false);
        if (!ensureCapacity (size)) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (size, offset, length));

        for (i = 0; i < j; i++)
        {
            if ((c = value.charAt (i)) > 0x7F)
            {
                if (c > 0x3FFF) buffer [offset++] = (byte)(((c >> 14) & 0x7F) | 0x80);
                buffer [offset++] = (byte)(((c >> 7) & 0x7F) | 0x80);
            }

            buffer [offset++] = (byte)(c & 0x7F);
        }
    }

//  +------------------+
//  | writeArrayBitmap |
//  +------------------+

    private void writeArrayBitmap (int pos, int bmp, int bm) throws DDException
    {
        if (pos > 0)  buffer [bmp] = (byte) bm;
        if (offset >= length) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (1, offset, length));
    }

//  +----------------+
//  | writeArrayInfo |
//  +----------------+

    private void writeArrayInfo (int size, int numbers, int type) throws DDException
    {
        int x = size < numbers ? 0x10 : 0;

        writeNumber ((size << 5) | x | type, false);
        if (x != 0)   writeNumber (numbers - size, false);
    }

//  #================#
//  # writeTypeAndId #
//  #================#

    void writeTypeAndId (int typeAndId) throws DDException
    {
        if (!ensureCapacity (2)) throw new DDException (DDError.BUFFER_OVERFLOW.getMessage (2, offset, length));

        buffer [offset++] = (byte)(typeAndId >> 8);
        buffer [offset++] = (byte) typeAndId;
    }

//  #=============#
//  # isValidTime #
//  #=============#

    public static boolean isValidTime (int data)
    {
        if (data == 0) return true;

        data   /= 1000;
        int sec = data % 100;     data /= 100;
        int min = data % 100;     data /= 100;

        return sec < 60 && min < 60 && data >= 0 && data < 24;
    }

//  #=============#
//  # isValidDate #
//  #=============#

    public static boolean isValidDate (int data)
    {
        if (data == 0) return true;

        int day = data % 100;    data /= 100;
        int mon = data % 100;    data /= 100;

        int year = data < 0 ? -data - 1 : data;

        return data != 0 && year <= 214747 && mon != 0 && mon <= 12 && day != 0 && (day <= MONTH_DAYS [mon - 1] ||
              (mon == 2 && day == 29 && (year % 400) == 0 || ((year % 4) == 0 && (year % 100) != 0)));    // Leap Year
    }

//  #=================#
//  # isValidDateTime #
//  #=================#

    public static boolean isValidDateTime (long data)
    {
        if (data == 0) return true;

        data    /=  1000;
        int sec  = (int)(data % 100);     data /= 100;
        int min  = (int)(data % 100);     data /= 100;
        int hour = (int)(data % 100);     data /= 100;
        int day  = (int)(data % 100);     data /= 100;
        int mon  = (int)(data % 100);     data /= 100;

        int year = (int)(data < 0 ? -data - 1 : data);

        return sec < 60 && min < 60 && hour < 24 && data != 0 && year <= 214747 &&
               mon != 0 && mon <= 12 && day != 0 && (day <= MONTH_DAYS [mon - 1] ||
              (mon == 2 && day == 29 && (year % 400) == 0 || ((year % 4) == 0 && (year % 100) != 0)));    // Leap Year
}

//  #============#
//  # stringData #
//  #============#

    static void stringData (StringBuilder sb, Object data, int type)
    {
        switch (type & 0x0F)
        {
            default: sb.append (data); return;    // byte, short, int, float, long, double, number, signedNumber

            case DDConst.TIME:     stringTime     (sb, (int)     data); return;
            case DDConst.DATE:     stringDate     (sb, (int)     data); return;
            case DDConst.DATETIME: stringDateTime (sb, (long)    data); return;
            case DDConst.BYTES:    stringBytes    (sb, (byte []) data); return;
            case DDConst.STRING:   stringString   (sb, (String)  data); return;

            case DDConst.ARRAY:
            {
                int      len,  i = 0;
                boolean  sep = false;
                type >>= 4;

                switch (type)
                {
                    case DDConst.BYTE:

                        len = ((byte []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            sb.append (((byte []) data) [i]);
                            sep = true;
                        }

                        break;

                    case DDConst.SHORT:

                        len = ((short []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            sb.append (((short []) data) [i]);
                            sep = true;
                        }

                        break;

                    case DDConst.INT:

                        len = ((int []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            sb.append (((int []) data) [i]);
                            sep = true;
                        }

                        break;

                    case DDConst.FLOAT:

                        len = ((float []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            sb.append (((float []) data) [i]);
                            sep = true;
                        }

                        break;

                    case DDConst.DOUBLE:

                        len = ((double []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            sb.append (((double []) data) [i]);
                            sep = true;
                        }

                        break;

                    case DDConst.TIME:

                        len = ((int []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            if (((int []) data) [i] != 0) stringTime (sb, ((int []) data) [i]);
                            sep = true;
                        }

                        break;

                    case DDConst.DATE:

                        len = ((int []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            if (((int []) data) [i] != 0) stringDate (sb, ((int []) data) [i]);
                            sep = true;
                        }

                        break;

                    case DDConst.DATETIME:

                        len = ((long []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            if (((long []) data) [i] != 0) stringDateTime (sb, ((long []) data) [i]);
                            sep = true;
                        }

                        break;

                    case DDConst.BYTES:

                        len = ((byte [][]) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            if (((byte [][]) data) [i] != null) stringBytes (sb, ((byte [][]) data) [i]);
                            sep = true;
                        }

                        break;

                    case DDConst.STRING:

                        len = ((String []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            if (((String []) data) [i] != null) stringString (sb, ((String []) data) [i]);
                            sep = true;
                        }

                        break;

                    default:

                        len = ((long []) data).length;
                        sb.append (len).append (DDBuffer.DATA_TYPES [type]).append ('[');

                        for (; i < len; i++)
                        {
                            if (sep) sb.append (',');
                            sb.append (((long []) data) [i]);
                            sep = true;
                        }
                }

                sb.append (']');
                return;
            }

            case DDConst.LIST: ((DDList) data).toString (sb);
        }
    }

//  +------------+
//  | stringTime |
//  +------------+

    private static void stringTime (StringBuilder sb, int data)
    {
        int ms  = data % 1000;    data /= 1000;
        int sec = data % 100;     data /= 100;
        int min = data % 100;     data /= 100;

        sb.append (String.format ("%02d:%02d:%02d.%03d", data, min, sec, ms));
    }

//  +------------+
//  | stringDate |
//  +------------+

    private static void stringDate (StringBuilder sb, int data)
    {
        int day = data % 100;    data /= 100;
        int mon = data % 100;    data /= 100;

        sb.append (String.format ("%04d-%02d-%02d", data, mon, day));
    }

//  +----------------+
//  | stringDateTime |
//  +----------------+

    private static void stringDateTime (StringBuilder sb, long data)
    {
        int ms   = (int)(data % 1000);    data /= 1000;
        int sec  = (int)(data % 100);     data /= 100;
        int min  = (int)(data % 100);     data /= 100;
        int hour = (int)(data % 100);     data /= 100;
        int day  = (int)(data % 100);     data /= 100;
        int mon  = (int)(data % 100);     data /= 100;

        sb.append (String.format ("%04d-%02d-%02d_%02d:%02d:%02d.%03d", data, mon, day, hour, min, sec, ms));
    }

//  +-------------+
//  | stringBytes |
//  +-------------+

    private static void stringBytes (StringBuilder sb, byte [] data)
    {
        sb.append (data.length).append ('x');
        int  h, l;

        for (byte b : data)
        {
            l =  b       & 0x0F;    l = l > 9 ? l + 55 : l + 48;
            h = (b >> 4) & 0x0F;    h = h > 9 ? h + 55 : h + 48;

            sb.append ((char) h).append ((char) l);
        }
    }

//  +--------------+
//  | stringString |
//  +--------------+

    private static void stringString (StringBuilder sb, String data)
    {
        sb.append (data.length ()).append ('"');

        char c;
        int  hh, ll, h, l;

        for (int i = 0; i < data.length (); i++)
            if ((c = data.charAt (i)) >= ' ' && c <= '~' && c != '\\') sb.append (c);
            else
            {
                sb.append ('\\');

                if ((c & 0xFF00) != 0)
                {
                    hh =  c >> 12;            hh = hh > 9 ? hh + 55 : hh + 48;
                    ll = (c >>  8) & 0x0F;    ll = ll > 9 ? ll + 55 : ll + 48;

                    sb.append ('\\').append ((char) hh).append ((char) ll);
                }

                h = (c >> 4) & 0x0F;    h = h > 9 ? h + 55 : h + 48;
                l =  c       & 0x0F;    l = l > 9 ? l + 55 : l + 48;

                sb.append ((char) h).append ((char) l);
            }

        sb.append ('"');
    }
}