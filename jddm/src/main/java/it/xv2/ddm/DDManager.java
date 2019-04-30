//  #======================================#
//  # Dynamic Data Manager - Version 1.0.0 #
//  #                                      #
//  # Author: Marco Vagnoni                #
//  # Date:   April     2019 (v1.0.0)      #
//  #======================================#

package it.xv2.ddm;

import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;

public final class DDManager
{
    private final static Calendar calendar = Calendar.getInstance ();

    private final Object  [] dataRoot       = new Object  [16];
    private final Object  [] propertiesRoot = new Object  [16];
    private final boolean [] checked        = new boolean [4096];
    private final byte    [] dataTypes      = new byte    [4096];

    private boolean          dataCheck      = true;
    private boolean          dataDefault    = true;

    private int              dataCount;
    private int              propertiesCount;

//  #=====#
//  # get #
//  #=====#

    public int getDataCount       () {return dataCount;}
    public int getPropertiesCount () {return propertiesCount;}

    public byte    getByte                (int id) throws DDException {return (Byte)      getData (id, DDConst.BYTE);}
    public short   getShort               (int id) throws DDException {return (Short)     getData (id, DDConst.SHORT);}
    public int     getInt                 (int id) throws DDException {return (Integer)   getData (id, DDConst.INT);}
    public long    getLong                (int id) throws DDException {return (Long)      getData (id, DDConst.LONG);}
    public float   getFloat               (int id) throws DDException {return (Float)     getData (id, DDConst.FLOAT);}
    public double  getDouble              (int id) throws DDException {return (Double)    getData (id, DDConst.DOUBLE);}
    public long    getNumber              (int id) throws DDException {return (Long)      getData (id, DDConst.NUMBER);}
    public long    getSignedNumber        (int id) throws DDException {return (Long)      getData (id, DDConst.S_NUMBER);}
    public Date    getTime                (int id) throws DDException {return (Date)      getData (id, DDConst.TIME);}
    public Date    getDate                (int id) throws DDException {return (Date)      getData (id, DDConst.DATE);}
    public Date    getDateTime            (int id) throws DDException {return (Date)      getData (id, DDConst.DATETIME);}
    public byte [] getBytes               (int id) throws DDException {return (byte [])   getData (id, DDConst.BYTES);}
    public String  getString              (int id) throws DDException {return (String)    getData (id, DDConst.STRING);}
    public DDList  getList                (int id) throws DDException {return (DDList)    getData (id, DDConst.LIST);}

    public byte   [] getByteArray         (int id) throws DDException {return (byte   []) getData (id, DDConst.A_BYTE);}
    public short  [] getShortArray        (int id) throws DDException {return (short  []) getData (id, DDConst.A_SHORT);}
    public int    [] getIntArray          (int id) throws DDException {return (int    []) getData (id, DDConst.A_INT);}
    public long   [] getLongArray         (int id) throws DDException {return (long   []) getData (id, DDConst.A_LONG);}
    public float  [] getFloatArray        (int id) throws DDException {return (float  []) getData (id, DDConst.A_FLOAT);}
    public double [] getDoubleArray       (int id) throws DDException {return (double []) getData (id, DDConst.A_DOUBLE);}
    public long   [] getNumberArray       (int id) throws DDException {return (long   []) getData (id, DDConst.A_NUMBER);}
    public long   [] getSignedNumberArray (int id) throws DDException {return (long   []) getData (id, DDConst.A_S_NUMBER);}
    public Date   [] getTimeArray         (int id) throws DDException {return (Date   []) getData (id, DDConst.A_TIME);}
    public Date   [] getDateArray         (int id) throws DDException {return (Date   []) getData (id, DDConst.A_DATE);}
    public Date   [] getDateTimeArray     (int id) throws DDException {return (Date   []) getData (id, DDConst.A_DATETIME);}
    public byte [][] getBytesArray        (int id) throws DDException {return (byte [][]) getData (id, DDConst.A_BYTES);}
    public String [] getStringArray       (int id) throws DDException {return (String []) getData (id, DDConst.A_STRING);}

//  +---------+
//  | getData |
//  +---------+

    private Object getData (int id, int type) throws DDException
    {
        if (id < 0 || id > 4095) throw new DDException (DDError.INVALID_ID.getMessage (id));

        if (dataTypes [id] == DDConst.NULL)
        {
            if (dataDefault)
            {
                DDProps props = (DDProps) seekObject (id, propertiesRoot);
                if (props != null && props.def != null) return props.def;     // Default
            }

            throw new DDException (DDError.DATA_NOT_FOUND.getMessage (id));
        }

        Object data = seekObject (id, dataRoot);

        if (dataCheck && !checked [id])
        {
            DDProps props = (DDProps) seekObject (id, propertiesRoot);
            if (props == null) throw new DDException (DDError.PROPERTIES_NOT_FOUND.getMessage (id));

            checkData (type, data, props);
            checked [id] = true;
        }

        if (type != (dataTypes [id] & 0xFF)) unexpectedData (id, dataTypes [id] & 0xFF, type);
        return data;
    }

//  #===============#
//  # getProperties #
//  #===============#

    public DDProps getProperties (int id) throws DDException
    {
        if (id < 0 || id > 4095) throw new DDException (DDError.INVALID_ID.getMessage (id));
        return (DDProps) seekObject (id, propertiesRoot);
    }

//  #====#
//  # is #
//  #====#

    public boolean isDataCheck   () {return dataCheck;}
    public boolean isDataDefault () {return dataDefault;}

//  #===============#
//  # isDataPresent #
//  #===============#

    public boolean isDataPresent (int id) throws DDException
    {
        if (id < 0 || id > 4095) throw new DDException (DDError.INVALID_ID.getMessage (id));

        DDProps props = (DDProps) seekObject (id,  propertiesRoot);
        return  dataTypes [id] != DDConst.NULL || (dataDefault && props != null && props.def != null);
    }

//  #=====#
//  # set #
//  #=====#

    public void setDataCheck   (boolean dataCheck)   {this.dataCheck   = dataCheck;}
    public void setDataDefault (boolean dataDefault) {this.dataDefault = dataDefault;}

    public void setByte              (int id, byte      data) throws DDException {setData (id, data, DDConst.BYTE);}
    public void setShort             (int id, short     data) throws DDException {setData (id, data, DDConst.SHORT);}
    public void setInt               (int id, int       data) throws DDException {setData (id, data, DDConst.INT);}
    public void setLong              (int id, long      data) throws DDException {setData (id, data, DDConst.LONG);}
    public void setFloat             (int id, float     data) throws DDException {setData (id, data, DDConst.FLOAT);}
    public void setDouble            (int id, double    data) throws DDException {setData (id, data, DDConst.DOUBLE);}
    public void setSignedNumber      (int id, long      data) throws DDException {setData (id, data, DDConst.S_NUMBER);}
    public void setTime              (int id, Date      data) throws DDException {setData (id, data, DDConst.TIME);}
    public void setDate              (int id, Date      data) throws DDException {setData (id, data, DDConst.DATE);}
    public void setDateTime          (int id, Date      data) throws DDException {setData (id, data, DDConst.DATETIME);}
    public void setBytes             (int id, byte   [] data) throws DDException {setData (id, data, DDConst.BYTES);}
    public void setString            (int id, String    data) throws DDException {setData (id, data, DDConst.STRING);}
    public void setList              (int id, DDList    data) throws DDException {setData (id, data, DDConst.LIST);}

    public void setByteArray         (int id, byte   [] data) throws DDException {setData (id, data, DDConst.A_BYTE);}
    public void setShortArray        (int id, short  [] data) throws DDException {setData (id, data, DDConst.A_SHORT);}
    public void setIntArray          (int id, int    [] data) throws DDException {setData (id, data, DDConst.A_INT);}
    public void setLongArray         (int id, long   [] data) throws DDException {setData (id, data, DDConst.A_LONG);}
    public void setFloatArray        (int id, float  [] data) throws DDException {setData (id, data, DDConst.A_FLOAT);}
    public void setDoubleArray       (int id, double [] data) throws DDException {setData (id, data, DDConst.A_DOUBLE);}
    public void setSignedNumberArray (int id, long   [] data) throws DDException {setData (id, data, DDConst.A_S_NUMBER);}
    public void setTimeArray         (int id, Date   [] data) throws DDException {setData (id, data, DDConst.A_TIME);}
    public void setDateArray         (int id, Date   [] data) throws DDException {setData (id, data, DDConst.A_DATE);}
    public void setDateTimeArray     (int id, Date   [] data) throws DDException {setData (id, data, DDConst.A_DATETIME);}
    public void setBytesArray        (int id, byte [][] data) throws DDException {setData (id, data, DDConst.A_BYTES);}
    public void setStringArray       (int id, String [] data) throws DDException {setData (id, data, DDConst.A_STRING);}

    public void setNumber (int id, long data) throws DDException
    {
        if (data < 0) throw new DDException (DDError.INVALID_DATA.getMessage (id));
        setData (id, data, DDConst.NUMBER);
    }

    public void setNumberArray (int id, long [] data) throws DDException
    {
        for (int i = 0; i < data.length; i++)
            if (data [i] < 0) throw new DDException (DDError.INVALID_DATA.getMessage (id));

        setData (id, data, DDConst.A_NUMBER);
    }

//  +---------+
//  | setData |
//  +---------+

    private void setData (int id, Object data, int type) throws DDException
    {
        if (id < 0 || id > 4095) throw new DDException (DDError.INVALID_ID.getMessage (id));
        if (data == null) throw new DDException (DDError.DATA_IS_NULL.getMessage (id));
        storeData (id, data, type);
    }

//  #===============#
//  # setProperties #
//  #===============#

    public void setProperties (DDProps properties) throws DDException
    {
        if (properties == null) throw new DDException (DDError.PROPERTIES_IS_NULL.getMessage ());

        Object [] propsTable = linkObject (properties.id, propertiesRoot);

        if (propsTable [properties.id & 0x0F] != null)
            throw new DDException (DDError.PROPERTIES_ALREADY_PRESENT.getMessage (properties.id));

        propsTable [properties.id & 0x0F] = properties;
        propertiesCount++;
    }

    public void setProperties (DDProps [] properties) throws DDException
        {for (DDProps props : properties) setProperties (props);}

//  +----------------+
//  | unexpectedData |
//  +----------------+

    private void unexpectedData (int id, int expected, int requested) throws DDException
    {
        int expectedIdH  = expected  >> 4;
        int requestedIdH = requested >> 4;

        throw new DDException (DDError.INVALID_DATA_TYPE.getMessage (id,
            (expectedIdH  == 0 ? "" : DDBuffer.STRING_TYPES [expectedIdH])  + DDBuffer.STRING_TYPES [expected  & 0x0F],
            (requestedIdH == 0 ? "" : DDBuffer.STRING_TYPES [requestedIdH]) + DDBuffer.STRING_TYPES [requested & 0x0F]));
    }

//  +-----------+
//  | checkData |
//  +-----------+

    private void checkData (int type, Object data, DDProps props) throws DDException    // return true if is default else false
    {
        int dataType = type & 0x0F;

        switch (dataType)
        {
            case DDConst.BYTE:      checkByte     (dataType, (Byte)    data, props); return;
            case DDConst.SHORT:     checkShort    (dataType, (Short)   data, props); return;
            case DDConst.INT:       checkInt      (dataType, (Integer) data, props); return;
            case DDConst.NUMBER:
            case DDConst.S_NUMBER:
            case DDConst.LONG:      checkLong     (dataType, (Long)    data, props); return;
            case DDConst.FLOAT:     checkFloat    (dataType, (Float)   data, props); return;
            case DDConst.DOUBLE:    checkDouble   (dataType, (Double)  data, props); return;
            case DDConst.TIME:      checkTime     (dataType, (Date)    data, props); return;
            case DDConst.DATE:      checkDate     (dataType, (Date)    data, props); return;
            case DDConst.DATETIME:  checkDateTime (dataType, (Date)    data, props); return;
            case DDConst.BYTES:     checkBytes    (dataType, (byte []) data, props); return;
            case DDConst.STRING:    checkString   (dataType, (String)  data, props); return;
            case DDConst.ARRAY:     checkArray    (type,               data, props); return;

            default:    // case DDConst.LIST:

                if ((props.operType & 0x0F) != DDConst.LIST)
                     unexpectedData (props.id, props.operType & 0x0F, DDConst.LIST);

                DDList value = (DDList) data;
                int    i     =  value.getDataCount ();

                if (props.number > 0 && props.number != i)
                    throw new DDException (DDError.INVALID_STRUCT_SIZE.getMessage ("list", props.id, i,  props.number));

                if (props.number < 0 && props.number > -i)
                    throw new DDException (DDError.INVALID_STRUCT_SIZE.getMessage ("list", props.id, i, -props.number));

                DDProps    itemProps;
                DDProps [] propsTable = (DDProps []) props.inOrProps;
                int        j, k       =  propsTable.length;
                Object     obj;

                value.begin ();

                while (!value.isEndOfList ())
                {
                    obj = value.getData (-1);
                    i   = value.getCurrentDataId   ();
                    j   = value.getCurrentDataType ();

                    itemProps = propsTable [i % k];

                    dataType = j & 0x0F;
                    value.next ();

                    switch (dataType)
                    {
                        default:               checkLong     (dataType, (Long)    obj, itemProps); continue;    // long, number, s_number
                        case DDConst.BYTE:     checkByte     (dataType, (Byte)    obj, itemProps); continue;
                        case DDConst.SHORT:    checkShort    (dataType, (Short)   obj, itemProps); continue;
                        case DDConst.INT:      checkInt      (dataType, (Integer) obj, itemProps); continue;
                        case DDConst.FLOAT:    checkFloat    (dataType, (Float)   obj, itemProps); continue;
                        case DDConst.DOUBLE:   checkDouble   (dataType, (Double)  obj, itemProps); continue;
                        case DDConst.TIME:     checkTime     (dataType, (Date)    obj, itemProps); continue;
                        case DDConst.DATE:     checkDate     (dataType, (Date)    obj, itemProps); continue;
                        case DDConst.DATETIME: checkDateTime (dataType, (Date)    obj, itemProps); continue;
                        case DDConst.BYTES:    checkBytes    (dataType, (byte []) obj, itemProps); continue;
                        case DDConst.STRING:   checkString   (dataType, (String)  obj, itemProps); continue;
                        case DDConst.ARRAY:    checkArray    (j,                  obj, itemProps);
                        case DDConst.NULL:
                    }
                }
            }
    }

//  +-------------+
//  | checkCommon |
//  +-------------+

    private boolean checkCommon (int type, long value, DDProps props) throws DDException
    {
        if (type >= 0 && type != (props.operType & 0x0F)) unexpectedData (props.id, props.operType & 0x0F, type);
        if (props.inOrProps != null ) return false;

        if ((props.operType & DDConst.CHKMIN) != 0 && value < props.min)
             throw new DDException (DDError.CHECK_MIN.getMessage
                                  ((type == DDConst.BYTES || type == DDConst.STRING ? "Size" : "Value"),
                                    props.id, props.name, value, props.min));

        if ((props.operType & DDConst.CHKMAX) != 0 && value > props.max)
             throw new DDException (DDError.CHECK_MAX.getMessage
                                  ((type == DDConst.BYTES || type == DDConst.STRING ? "Size" : "Value"),
                                    props.id, props.name, value, props.max));
        return true;
    }

//  +-----------+
//  | checkByte |
//  +-----------+

    private void checkByte (int type, byte value, DDProps props) throws DDException
    {
        if (checkCommon (type, value, props)) return;

        for (int i = 0; i < ((byte []) props.inOrProps).length; i++)
            if (value    == ((byte []) props.inOrProps) [i]) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, value));
    }

//  +------------+
//  | checkShort |
//  +------------+

    private void checkShort (int type, short value, DDProps props) throws DDException
    {
        if (checkCommon (type, value, props)) return;

        for (int i = 0; i < ((short []) props.inOrProps).length; i++)
             if (value   == ((short []) props.inOrProps) [i]) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, value));
    }

//  +----------+
//  | checkInt |
//  +----------+

    private void checkInt (int type, int value, DDProps props) throws DDException
    {
        if (checkCommon (type, value, props)) return;

        for (int i = 0; i < ((int []) props.inOrProps).length; i++)
             if (value   == ((int []) props.inOrProps) [i]) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, value));
    }

//  +-----------+
//  | checkLong |
//  +-----------+

    private void checkLong (int type, long value, DDProps props) throws DDException
    {
        if (checkCommon (type, value, props)) return;

        for (int i = 0; i < ((long []) props.inOrProps).length; i++)
             if (value   == ((long []) props.inOrProps) [i]) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, value));
    }

//  +------------+
//  | checkFloat |
//  +------------+

    private void checkFloat (int type, float value, DDProps props) throws DDException
    {
        if (type >= 0 && type != (props.operType & 0x0F))
            unexpectedData (props.id, props.operType & 0x0F, type);

        if (props.inOrProps == null)
        {
            if ((props.operType & DDConst.CHKMIN) != 0 && value < Float.intBitsToFloat ((int) props.min))
                 throw new DDException (DDError.CHECK_MIN_FLOAT.getMessage (props.id, props.name, value,
                                        value, Float.intBitsToFloat ((int) props.min), props.min));

            if ((props.operType & DDConst.CHKMAX) != 0 && value > Float.intBitsToFloat ((int) props.max))
                 throw new DDException (DDError.CHECK_MAX_FLOAT.getMessage (props.id, props.name, value,
                                        value, Float.intBitsToFloat ((int) props.min), props.min));
            return;
        }

        int iv = Float.floatToIntBits (value);

        for (int i = 0; i < ((int []) props.inOrProps).length; i++)
             if (iv      == ((int []) props.inOrProps) [i]) return;

        throw new DDException (DDError.CHECK_IN_FLOAT.getMessage (props.id, props.name, value, iv));
    }

//  +-------------+
//  | checkDouble |
//  +-------------+

    private void checkDouble (int type, double value, DDProps props) throws DDException
    {
        if (type >= 0 && type != (props.operType & 0x0F))
            unexpectedData (props.id, props.operType & 0x0F, type);

        if (props.inOrProps == null)
        {
            if ((props.operType & DDConst.CHKMIN) != 0 && value < Double.longBitsToDouble (props.min))
                 throw new DDException (DDError.CHECK_MIN_FLOAT.getMessage (props.id, props.name, value,
                                        value, Double.longBitsToDouble (props.min), props.min));

            if ((props.operType & DDConst.CHKMAX) != 0 && value > Double.longBitsToDouble (props.max))
                 throw new DDException (DDError.CHECK_MAX_FLOAT.getMessage (props.id, props.name, value,
                                        value, Double.longBitsToDouble (props.max), props.max));
            return;
        }

        long lv = Double.doubleToLongBits (value);

        for (int i = 0; i < ((long []) props.inOrProps).length; i++)
             if (lv      == ((long []) props.inOrProps) [i]) return;

        throw new DDException (DDError.CHECK_IN_FLOAT.getMessage (props.id, props.name, value, lv));
    }

//  +-----------+
//  | checkTime |
//  +-----------+

    private void checkTime (int type, Date value, DDProps props) throws DDException
    {
        if (value != null)
        {
            calendar.setTime (value);
            long time = calendar.get (Calendar.HOUR_OF_DAY) * 10000000 +
                        calendar.get (Calendar.MINUTE)      *   100000 +
                        calendar.get (Calendar.SECOND)      *     1000 +
                        calendar.get (Calendar.MILLISECOND);

            if (checkCommon (type, time, props)) return;

            for (Date item : (Date []) props.inOrProps)
                 if (value.equals(item)) return;
        }
        else
            if ((props.operType & DDConst.CHKLIM) == 0)
                 if (props.inOrProps == null) return;
                 else for (Date item : (Date []) props.inOrProps)
                           if (item == null) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, value));
    }

//  +-----------+
//  | checkDate |
//  +-----------+

    private void checkDate (int type, Date value, DDProps props) throws DDException
    {
        if (value != null)
        {
            calendar.setTime (value);
            long date = calendar.get (Calendar.YEAR)  * 10000 +
                        calendar.get (Calendar.MONTH) *   100 +
                        calendar.get (Calendar.DAY_OF_MONTH);

            if (checkCommon (type, date, props)) return;

            for (Date item : (Date []) props.inOrProps)
                 if (value.equals(item)) return;
        }
        else
            if ((props.operType & DDConst.CHKLIM) == 0)
                 if (props.inOrProps == null) return;
                 else for (Date item : (Date []) props.inOrProps)
                           if (item == null) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, value));
    }

//  +---------------+
//  | checkDateTime |
//  +---------------+

    private void checkDateTime (int type, Date value, DDProps props) throws DDException
    {
        if (value != null)
        {
            calendar.setTime (value);
            long datetime = calendar.get (Calendar.YEAR)         * 10000000000000L +
                            calendar.get (Calendar.MONTH)        *   100000000000L +
                            calendar.get (Calendar.DAY_OF_MONTH) *     1000000000  +
                            calendar.get (Calendar.HOUR_OF_DAY)  *       10000000  +
                            calendar.get (Calendar.MINUTE)       *         100000  +
                            calendar.get (Calendar.SECOND)       *           1000  +
                            calendar.get (Calendar.MILLISECOND);

            if (checkCommon (type, datetime, props)) return;

            for (Date item : (Date []) props.inOrProps)
                 if (value.equals(item)) return;
        }
        else
            if ((props.operType & DDConst.CHKLIM) == 0)
                 if (props.inOrProps == null) return;
                 else for (Date item : (Date []) props.inOrProps)
                           if (item == null) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, value));
    }

//  +------------+
//  | checkBytes |
//  +------------+

    private void checkBytes (int type, byte [] value, DDProps props) throws DDException
    {
        if (value != null)
        {
            if (checkCommon (type, value.length, props)) return;

            for (byte [] item : (byte [][]) props.inOrProps)
                 if (Arrays.equals (value, item)) return;
        }
        else
            if ((props.operType & DDConst.CHKLIM) == 0)
                 if (props.inOrProps == null) return;
                 else for (byte [] item : (byte [][]) props.inOrProps)
                           if (item == null) return;

        throw new DDException (DDError.CHECK_IN.getMessage
                              (props.id, props.name, Arrays.toString (value).replace (" ", "")));
    }

//  +-------------+
//  | checkString |
//  +-------------+

    private void checkString (int type, String value, DDProps props) throws DDException
    {
        if (value != null)
        {
            if (checkCommon (type, value.length (), props)) return;

            for (String item : (String []) props.inOrProps)
                if (value.equals (item)) return;
        }
        else
            if ((props.operType & DDConst.CHKLIM) == 0)
                 if (props.inOrProps == null) return;
                 else for (String item : (String []) props.inOrProps)
                           if (item == null) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, value));
    }

//  +------------+
//  | checkArray |
//  +------------+

    private void checkArray (int type, Object data, DDProps props) throws DDException
    {
        if (type != (props.operType & 0xFF)) unexpectedData (props.id, props.operType & 0xFF, type);

        int  len;
        type >>= 4;

        switch (type)
        {
            case DDConst.BYTE:     len = ((byte   []) data).length; break;
            case DDConst.SHORT:    len = ((short  []) data).length; break;
            case DDConst.INT:      len = ((int    []) data).length; break;
            case DDConst.NUMBER:
            case DDConst.S_NUMBER:
            case DDConst.LONG:     len = ((long   []) data).length; break;
            case DDConst.FLOAT:    len = ((float  []) data).length; break;
            case DDConst.DOUBLE:   len = ((double []) data).length; break;
            default:               len = ((Object []) data).length;
        }

        if (props.number > 0 && props.number != len)
            throw new DDException (DDError.INVALID_STRUCT_SIZE.getMessage ("array", props.id, len,  props.number));

        if (props.number < 0 && props.number > -len)
            throw new DDException (DDError.INVALID_STRUCT_SIZE.getMessage ("array", props.id, len, -props.number));

        int i = 0;

        switch (type)
        {
            case DDConst.BYTE:      for (; i < len; i++) checkByte     (-1, ((byte   []) data) [i], props); break;
            case DDConst.SHORT:     for (; i < len; i++) checkShort    (-1, ((short  []) data) [i], props); break;
            case DDConst.INT:       for (; i < len; i++) checkInt      (-1, ((int    []) data) [i], props); break;
            case DDConst.NUMBER:
            case DDConst.S_NUMBER:
            case DDConst.LONG:      for (; i < len; i++) checkLong     (-1, ((long   []) data) [i], props); break;
            case DDConst.FLOAT:     for (; i < len; i++) checkFloat    (-1, ((float  []) data) [i], props); break;
            case DDConst.DOUBLE:    for (; i < len; i++) checkDouble   (-1, ((double []) data) [i], props); break;
            case DDConst.TIME:      for (; i < len; i++) checkTime     (-1, ((Date   []) data) [i], props); break;
            case DDConst.DATE:      for (; i < len; i++) checkDate     (-1, ((Date   []) data) [i], props); break;
            case DDConst.DATETIME:  for (; i < len; i++) checkDateTime (-1, ((Date   []) data) [i], props); break;
            case DDConst.BYTES:     for (; i < len; i++) checkBytes    (-1, ((byte [][]) data) [i], props); break;
            default:                for (; i < len; i++) checkString   (-1, ((String []) data) [i], props);
        }
    }

//  +------------------+
//  | checkDataDefault |
//  +------------------+

    private boolean checkDataDefault (int type, Object data1, Object data2) throws DDException
    {
        switch (type & 0x0F)
        {
            default:            return data1.equals      ( data2);
            case DDConst.BYTES: return Arrays.equals     ((byte []) data1, (byte []) data2);
            case DDConst.ARRAY: return checkArrayDefault ( type,    data1,  data2);

            case DDConst.LIST:

                DDList list    = (DDList) data1;
                DDList listDef = (DDList) data2;

                if (list.getDataCount () != listDef.getDataCount ()) return false;

                Object obj, objDef;
                int    i,   j;

                list.begin    ();
                listDef.begin ();

                while (!list.isEndOfList ())
                {
                    obj    = list.getData   (-1);
                    objDef = listDef.getData (-1);
                    i      = list.getCurrentDataType   ();
                    j      = listDef.getCurrentDataType ();

                    list.next   ();
                    listDef.next ();

                    if (i == DDConst.NULL || j == DDConst.NULL)
                        if (obj == null && objDef == null) continue;
                        else return false;

                    if (i != j) return false;

                    switch (i & 0x0F)
                    {
                        default:            if (obj.equals (objDef)) continue; else return false;
                        case DDConst.BYTES: if (Arrays.equals ((byte []) obj, (byte []) objDef)) continue; else return false;
                        case DDConst.ARRAY: if (!checkArrayDefault (i, obj, objDef)) return false;
                    }
                }

                return true;
        }
    }

//  +-------------------+
//  | checkArrayDefault |
//  +-------------------+

    private boolean checkArrayDefault (int type, Object array1, Object array2)
    {
        switch (type >> 4)
        {
            default:             return Arrays.equals ((long   []) array1, (long   []) array2);
            case DDConst.BYTE:   return Arrays.equals ((byte   []) array1, (byte   []) array2);
            case DDConst.SHORT:  return Arrays.equals ((short  []) array1, (short  []) array2);
            case DDConst.INT:    return Arrays.equals ((int    []) array1, (int    []) array2);
            case DDConst.FLOAT:  return Arrays.equals ((float  []) array1, (float  []) array2);
            case DDConst.DOUBLE: return Arrays.equals ((double []) array1, (double []) array2);

            case DDConst.TIME:
            case DDConst.DATE:
            case DDConst.DATETIME:

                if (((Date []) array1).length != ((Date []) array2).length) return false;

                for (int i = 0; i < ((Date []) array1).length; i++)
                    if ((((Date []) array1) [i]) != null)
                    {
                        if (!(((Date []) array1) [i].equals (((Date []) array2) [i]))) return false;
                    }
                    else
                        if  ((((Date []) array2) [i]) != null) return false;

                return true;

            case DDConst.BYTES:

                if (((byte [][]) array1).length != ((byte [][]) array2).length) return false;

                for (int i = 0; i < ((byte [][]) array1).length; i++)
                    if ((((byte [][]) array1) [i]) != null)
                    {
                        if (!Arrays.equals (((byte [][]) array1) [i], ((byte [][]) array2) [i])) return false;
                    }
                    else
                        if ((((byte [][]) array2) [i]) != null) return false;

                return true;

            case DDConst.STRING:

                if (((String []) array1).length != ((String []) array2).length) return false;

                for (int i = 0; i < ((String []) array1).length; i++)
                    if ((((String []) array1) [i]) != null)
                    {
                        if (!(((String []) array1) [i].equals (((String []) array2) [i]))) return false;
                    }
                    else
                        if  ((((String []) array2) [i]) != null) return false;

                return true;
        }
    }

//  #========#
//  # decode #
//  #========#

    public void decode (byte [] buffer) throws DDException
        {decode (buffer, 0, buffer.length, true, true);}

    public void decode (byte [] buffer, int offset) throws DDException
        {decode (buffer, offset, buffer.length, true, true);}

    public void decode (byte [] buffer, boolean checkCRC32) throws DDException
        {decode (buffer, 0, buffer.length, true, checkCRC32);}

    public void decode (byte [] buffer, boolean checkProtocolInfo, boolean checkCRC32) throws DDException
        {decode (buffer, 0, buffer.length, checkProtocolInfo, checkCRC32);}

    public void decode (byte [] buffer, int offset, boolean checkCRC32) throws DDException
        {decode (buffer, offset, buffer.length, true, checkCRC32);}

    public void decode (byte [] buffer, int offset, boolean checkProtocolInfo, boolean checkCRC32) throws DDException
        {decode (buffer, offset, buffer.length, checkProtocolInfo, checkCRC32);}

    public void decode (byte [] buffer, int offset, int length, boolean checkProtocolInfo, boolean checkCRC32) throws DDException
    {
        if (buffer == null || offset < 0 || length < 0 || offset + length > buffer.length)
            throw new DDException (DDError.INVALID_DECODE_ARGUMENT.getMessage ((buffer == null ? null : "byte[" + buffer.length + ']'),
                                                                                         offset, length));
        DDBuffer ddb = new DDBuffer (buffer, offset, length);

        if (checkProtocolInfo) ddb.checkProtocolInfo ();
        if (checkCRC32)        ddb.checkCRC32        ();

        byte [] type = new byte [1];
        int     id;

        while (!ddb.endOfBuffer ())
        {
            id       =  ddb.readTypeAndId ();
            type [0] = (byte)((id >> 12) & 0x0F);
            storeData (id & 0xFFF, ddb.read (type), type [0]);
        }
    }

//  +------------+
//  | linkObject |
//  +------------+

    private Object [] linkObject (int id, Object [] obj)
    {
        int idx = id  >> 8;
        if (obj [idx] == null) obj [idx] = new Object [16];

        obj = (Object []) obj [idx];
        idx = (id >> 4) & 0x0F;

        if (obj [idx] == null) obj [idx] = new Object [16];
        return (Object []) obj [idx];
    }

//  +------------+
//  | seekObject |
//  +------------+

    private Object seekObject (int id, Object [] obj)
    {
        if ((obj = (Object [])(obj [ id >>> 8]))         != null  &&
            (obj = (Object [])(obj [(id >>> 4) & 0x0F])) != null) return (obj [id & 0x0F]);

        return null;
    }

//  +-----------+
//  | storeData |
//  +-----------+

    private void storeData (int id, Object data, int type) throws DDException
    {
        if (dataTypes [id] != DDConst.NULL) throw new DDException (DDError.DATA_ALREADY_PRESENT.getMessage (id));

        if (dataDefault)
        {
            DDProps props = (DDProps) seekObject (id, propertiesRoot);
            if (props != null && props.def != null && (props.operType & DDConst.REQ) == 0 &&
                checkDataDefault (type, data, props.def)) return;                               // Default
        }

        dataTypes  [id] =  (byte)  type;
        linkObject (id, dataRoot) [id & 0x0F] = data;
        dataCount++;
    }

//  #========#
//  # encode #
//  #========#

    public byte [] encode () throws DDException
        {return encode (0, 0, true, true);}

    public byte [] encode (int reservedSizeBefore) throws DDException
        {return encode (reservedSizeBefore, 0, true, true);}

    public byte [] encode (boolean addCRC32) throws DDException
        {return encode (0, 0, true, addCRC32);}

    public byte [] encode (boolean addProtocolInfo, boolean addCRC32) throws DDException
        {return encode (0, 0, addProtocolInfo, addCRC32);}

    public byte [] encode (int reservedSizeBefore, boolean addCRC32) throws DDException
        {return encode (reservedSizeBefore, 0, true, addCRC32);}

    public byte [] encode (int reservedSizeBefore, boolean addProtocolInfo, boolean addCRC32) throws DDException
        {return encode (reservedSizeBefore, 0, addProtocolInfo, addCRC32);}

    public byte [] encode (int reservedSizeBefore, int reservedSizeAfter, boolean addProtocolInfo, boolean addCRC32)
        throws DDException
    {
        if (reservedSizeBefore < 0 || reservedSizeAfter < 0 || (reservedSizeBefore + reservedSizeAfter > 65535))
            throw new DDException (DDError.INVALID_ENCODE_ARGUMENT.getMessage (reservedSizeBefore, reservedSizeAfter));

        DDBuffer DDBuffer = new DDBuffer (new byte [4096], 0, 4096);
        if (addProtocolInfo) DDBuffer.addProtocolInfo ();

        int        i, j, k, x;
        Object     data;
        Object  [] objTable1, objTable2;
        DDProps [] propsTable;
        DDList     list;
        DDProps    props;

        if (propertiesCount > 0)
            for (i = 0; i < 16; i++)      // Check Required Flag
            {
                if ((objTable1 = (Object [])(propertiesRoot [i])) == null) continue;

                for (j = 0; j < 16; j++)
                {
                    if ((objTable2 = (Object [])(objTable1 [j])) == null) continue;

                    for (k = 0; k < 16; k++)
                    {
                        if ((data = objTable2 [k]) == null) continue;
                        x = (i << 8) | (j << 4) | k;

                        if ((((DDProps) data).operType & DDConst.REQ) > 0 && dataTypes [x] == DDConst.NULL)
                            throw new DDException (DDError.MISSING_DATA.getMessage (x, ((DDProps) data).name));

                        if (dataTypes [x] != DDConst.LIST || (propsTable = (DDProps [])((DDProps) data).inOrProps) == null)
                            continue;

                        list = (DDList) seekObject (x, dataRoot);
                        x    =  propsTable.length;

                        list.begin ();

                        while (!list.isEndOfList ())
                        {
                            props = propsTable [list.getCurrentDataId () % x];

                            if ((props.operType & DDConst.REQ) != 0 && list.getCurrentDataType () == DDConst.NULL)
                                 throw new DDException (DDError.MISSING_DATA.getMessage (props.id, props.name));

                            list.next ();
                        }

                        for (x = list.getDataCount (); x < propsTable.length; x++)
                             if ((propsTable [x].operType & DDConst.REQ) != 0)
                                 throw new DDException (DDError.MISSING_DATA.getMessage (propsTable [x].id, propsTable [x].name));
                    }
                }
            }

        int y;

        for (i = 0; i < 16; i++)
        {
            if ((objTable1 = (Object [])(dataRoot [i])) == null) continue;

            for (j = 0; j < 16; j++)
            {
                if ((objTable2 = (Object [])(objTable1 [j])) == null) continue;

                for (k = 0; k < 16; k++)
                {
                    x = (i << 8) | (j << 4) | k;
                    y =  dataTypes [x] & 0xFF;

                    if (y == DDConst.NULL) continue;
                    data = objTable2 [k];

                    if (dataCheck && !checked [x])
                    {
                        props =  (DDProps) seekObject (x, propertiesRoot);
                        if (props == null) throw new DDException (DDError.PROPERTIES_NOT_FOUND.getMessage (x));

                        checkData (y, data, props);
                        checked [x] = true;
                    }

                    DDBuffer.writeTypeAndId (((y & 0x0F) << 12) | x);
                    DDBuffer.write (data, y);
                }
            }
        }

        if (addCRC32) DDBuffer.addCRC32 ();

        int     offset = DDBuffer.getOffset ();
        byte [] buffer = new byte  [offset + reservedSizeBefore + reservedSizeAfter];

        if (offset > 0) System.arraycopy (DDBuffer.getBuffer (), 0, buffer, reservedSizeBefore, offset);
        return buffer;
    }

//  #==========#
//  # toString #
//  #==========#

    @Override
    public String toString ()
    {
        StringBuilder sb  = new StringBuilder (1024);
        boolean       sep = false;
        Object []     dataTable1, dataTable2;
        DDProps       props;
        Object        data;
        int           i, j, k, x;

        sb.append (dataCount).append ('{');

        for (i = 0; i < 16; i++)
        {
            if ((dataTable1 = (Object [])(dataRoot [i])) == null) continue;

            for (j = 0; j < 16; j++)
            {
                if ((dataTable2 = (Object [])(dataTable1 [j])) == null) continue;

                for (k = 0; k < 16; k++)
                {
                    x = (i << 8) | (j << 4) | k;
                    if (dataTypes [x] == DDConst.NULL) continue;

                    if (sep) sb.append (' ');
                    sep = true;

                    data = dataTable2 [k];
                    if ((props = (DDProps) seekObject (x, propertiesRoot)) != null) sb.append (props.name).append ('@');

                    sb.append (x).append ('=').append (DDBuffer.DATA_TYPES [dataTypes [x] & 0x0F]);
                    DDBuffer.stringData (sb, data, dataTypes [x] & 0xFF, props);
                }
            }
        }

        sb.append ('}');
        return sb.toString ();
    }
}