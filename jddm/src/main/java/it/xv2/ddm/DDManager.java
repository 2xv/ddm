//  #======================================#
//  # Dynamic Data Manager - Version 1.0.0 #
//  #                                      #
//  # Author: Marco Vagnoni                #
//  # Email:  marco.vagnoni@yahoo.com      #
//  # Date:   April     2019 (v1.0.0)      #
//  #======================================#

package it.xv2.ddm;

import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;

public final class DDManager
{
    private final Object  [] dataRoot       = new Object  [16];
    private final Object  [] propertiesRoot = new Object  [16];
    private final boolean [] checked        = new boolean [4096];
    private final byte    [] dataTypes      = new byte    [4096];

    private boolean          dataCheck      = true;
    private boolean          dataDefault    = true;

    private int              dataNumber;
    private int              propertiesNumber;

//  #=====#
//  # get #
//  #=====#

    public int       getDataNumber        () {return dataNumber;}
    public int       getPropertiesNumber  () {return propertiesNumber;}

    public byte      getByte              (int id) throws DDException {return (byte)      getData (id, DDConst.BYTE);}
    public short     getShort             (int id) throws DDException {return (short)     getData (id, DDConst.SHORT);}
    public int       getInt               (int id) throws DDException {return (int)       getData (id, DDConst.INT);}
    public long      getLong              (int id) throws DDException {return (long)      getData (id, DDConst.LONG);}
    public float     getFloat             (int id) throws DDException {return (float)     getData (id, DDConst.FLOAT);}
    public double    getDouble            (int id) throws DDException {return (double)    getData (id, DDConst.DOUBLE);}
    public long      getNumber            (int id) throws DDException {return (long)      getData (id, DDConst.NUMBER);}
    public long      getSignedNumber      (int id) throws DDException {return (long)      getData (id, DDConst.S_NUMBER);}
    public int       getTime              (int id) throws DDException {return (int)       getData (id, DDConst.TIME);}
    public int       getDate              (int id) throws DDException {return (int)       getData (id, DDConst.DATE);}
    public long      getDateTime          (int id) throws DDException {return (long)      getData (id, DDConst.DATETIME);}
    public byte   [] getBytes             (int id) throws DDException {return (byte [])   getData (id, DDConst.BYTES);}
    public String    getString            (int id) throws DDException {return (String)    getData (id, DDConst.STRING);}
    public DDList    getList              (int id) throws DDException {return (DDList)    getData (id, DDConst.LIST);}

    public byte   [] getByteArray         (int id) throws DDException {return (byte   []) getData (id, DDConst.A_BYTE);}
    public short  [] getShortArray        (int id) throws DDException {return (short  []) getData (id, DDConst.A_SHORT);}
    public int    [] getIntArray          (int id) throws DDException {return (int    []) getData (id, DDConst.A_INT);}
    public long   [] getLongArray         (int id) throws DDException {return (long   []) getData (id, DDConst.A_LONG);}
    public float  [] getFloatArray        (int id) throws DDException {return (float  []) getData (id, DDConst.A_FLOAT);}
    public double [] getDoubleArray       (int id) throws DDException {return (double []) getData (id, DDConst.A_DOUBLE);}
    public long   [] getNumberArray       (int id) throws DDException {return (long   []) getData (id, DDConst.A_NUMBER);}
    public long   [] getSignedNumberArray (int id) throws DDException {return (long   []) getData (id, DDConst.A_S_NUMBER);}
    public int    [] getTimeArray         (int id) throws DDException {return (int    []) getData (id, DDConst.A_TIME);}
    public int    [] getDateArray         (int id) throws DDException {return (int    []) getData (id, DDConst.A_DATE);}
    public long   [] getDateTimeArray     (int id) throws DDException {return (long   []) getData (id, DDConst.A_DATETIME);}
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

        if (type != dataTypes [id] &&
           (type <  DDConst.LONG   || type > DDConst.S_NUMBER   || dataTypes [id] < DDConst.LONG   || dataTypes [id] > DDConst.S_NUMBER) &&
           (type <  DDConst.A_LONG || type > DDConst.A_S_NUMBER || dataTypes [id] < DDConst.A_LONG || dataTypes [id] > DDConst.A_S_NUMBER))
                DDBuffer.unexpectedData (id, dataTypes [id] & 0xFF, type);

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

    public DDProps [] getProperties ()
    {
        if (propertiesNumber == 0) return null;

        DDProps [] propsTable = new DDProps [propertiesNumber];
        Object  [] propsTable1, propsTable2;
        DDProps    props;
        int        i, j, k, x = 0;

        for (i = 0; i < 16; i++)
        {
            if ((propsTable1 = (Object [])(propertiesRoot [i])) == null) continue;

            for (j = 0; j < 16; j++)
            {
                if ((propsTable2 = (Object [])(propsTable1 [j])) == null) continue;

                for (k = 0; k < 16; k++)
                    if ((props = (DDProps)(propsTable2 [k])) != null) propsTable [x++] = props;
            }
        }

        return propsTable;
    }

//  #===================#
//  # getListProperties #
//  #===================#

    public DDProps [] getListProperties (int id) throws DDException
    {
        if (id < 0 || id > 4095) throw new DDException (DDError.INVALID_ID.getMessage (id));

        if (dataTypes [id] != DDConst.LIST)
            throw new DDException (DDError.INVALID_DATA_TYPE.getMessage (id, DDBuffer.STRING_TYPES [DDConst.LIST],
                                                                             DDBuffer.STRING_TYPES [dataTypes [id]]));
        return ((DDList) seekObject (id, dataRoot)).getProperties ();
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

    public void setDataCheck         (boolean dataCheck)   {this.dataCheck   = dataCheck;}
    public void setDataDefault       (boolean dataDefault) {this.dataDefault = dataDefault;}

    public void setByte              (int id, byte            data) throws DDException {setData (id, data, DDConst.BYTE);}
    public void setShort             (int id, short           data) throws DDException {setData (id, data, DDConst.SHORT);}
    public void setInt               (int id, int             data) throws DDException {setData (id, data, DDConst.INT);}
    public void setLong              (int id, long            data) throws DDException {setData (id, data, DDConst.LONG);}
    public void setFloat             (int id, float           data) throws DDException {setData (id, data, DDConst.FLOAT);}
    public void setDouble            (int id, double          data) throws DDException {setData (id, data, DDConst.DOUBLE);}
    public void setSignedNumber      (int id, long            data) throws DDException {setData (id, data, DDConst.S_NUMBER);}
    public void setBytes             (int id, final byte   [] data) throws DDException {setData (id, data, DDConst.BYTES);}
    public void setString            (int id, final String    data) throws DDException {setData (id, data, DDConst.STRING);}
    public void setList              (int id, final DDList    data) throws DDException {setData (id, data, DDConst.LIST);}

    public void setByteArray         (int id, final byte   [] data) throws DDException {setData (id, data, DDConst.A_BYTE);}
    public void setShortArray        (int id, final short  [] data) throws DDException {setData (id, data, DDConst.A_SHORT);}
    public void setIntArray          (int id, final int    [] data) throws DDException {setData (id, data, DDConst.A_INT);}
    public void setLongArray         (int id, final long   [] data) throws DDException {setData (id, data, DDConst.A_LONG);}
    public void setFloatArray        (int id, final float  [] data) throws DDException {setData (id, data, DDConst.A_FLOAT);}
    public void setDoubleArray       (int id, final double [] data) throws DDException {setData (id, data, DDConst.A_DOUBLE);}
    public void setSignedNumberArray (int id, final long   [] data) throws DDException {setData (id, data, DDConst.A_S_NUMBER);}
    public void setBytesArray        (int id, final byte [][] data) throws DDException {setData (id, data, DDConst.A_BYTES);}
    public void setStringArray       (int id, final String [] data) throws DDException {setData (id, data, DDConst.A_STRING);}

    public void setNumber (int id, long data) throws DDException
        {setData (id, data, (data < 0 ? DDConst.S_NUMBER : DDConst.NUMBER));}

    public void setTime (int id, int data) throws DDException
    {
        if (!DDBuffer.isValidTime (data)) throw new DDException (DDError.INVALID_DATA.getMessage (id));
        setData (id, data, DDConst.TIME);
    }

    public void setDate (int id, int data) throws DDException
    {
        if (!DDBuffer.isValidDate (data)) throw new DDException (DDError.INVALID_DATA.getMessage (id));
        setData (id, data, DDConst.DATE);
    }

    public void setDateTime (int id, long data) throws DDException
    {
        if (!DDBuffer.isValidDateTime (data)) throw new DDException (DDError.INVALID_DATA.getMessage (id));
        setData (id, data, DDConst.DATETIME);
    }

    public void setNumberArray (int id, long [] data) throws DDException
    {
        int type = DDConst.A_NUMBER;

        if (data != null)
            for (long v : data)
                 if  (v < 0) {type = DDConst.A_S_NUMBER; break;}

        setData (id, data, type);
    }

    public void setTimeArray (int id, int [] data) throws DDException
    {
        if (data != null)
            for (int v : data)
                 if (!DDBuffer.isValidTime (v)) throw new DDException (DDError.INVALID_DATA.getMessage (id));

        setData (id, data, DDConst.A_TIME);
    }

    public void setDateArray (int id, int [] data) throws DDException
    {
        if (data != null)
            for (int v : data)
                if (!DDBuffer.isValidDate (v)) throw new DDException (DDError.INVALID_DATA.getMessage (id));

        setData (id, data, DDConst.A_DATE);
    }

    public void setDateTimeArray (int id, long   [] data) throws DDException
    {
        if (data != null)
            for (long v : data)
                 if (!DDBuffer.isValidDateTime (v)) throw new DDException (DDError.INVALID_DATA.getMessage (id));

        setData (id, data, DDConst.A_DATETIME);
    }

//  +---------+
//  | setData |
//  +---------+

    private void setData (int id, final Object data, int type) throws DDException
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
        propertiesNumber++;
    }

    public void setProperties (DDProps [] properties) throws DDException
        {for (DDProps props : properties) setProperties (props);}

//  #===================#
//  # setListProperties #
//  #===================#

    public void setListProperties (int id, DDProps [] properties) throws DDException
    {
        if (id < 0 || id > 4095) throw new DDException (DDError.INVALID_ID.getMessage (id));

        if (dataTypes [id] != DDConst.LIST)
            throw new DDException (DDError.INVALID_DATA_TYPE.getMessage (id, DDBuffer.STRING_TYPES [DDConst.LIST],
                                                                             DDBuffer.STRING_TYPES [dataTypes [id]]));
        ((DDList) seekObject (id, dataRoot)).setProperties (properties);
    }

//  +-----------+
//  | checkData |
//  +-----------+

    private void checkData (int type, Object data, DDProps props) throws DDException    // return true if is default else false
    {
        int dataType = type & 0x0F;

        switch (dataType)
        {
            default:                checkLong     (dataType, (long)    data, props); return;
            case DDConst.BYTE:      checkByte     (dataType, (byte)    data, props); return;
            case DDConst.SHORT:     checkShort    (dataType, (short)   data, props); return;
            case DDConst.INT:       checkInt      (dataType, (int)     data, props); return;
            case DDConst.FLOAT:     checkFloat    (dataType, (float)   data, props); return;
            case DDConst.DOUBLE:    checkDouble   (dataType, (double)  data, props); return;
            case DDConst.TIME:      checkTime     (dataType, (int)     data, props); return;
            case DDConst.DATE:      checkDate     (dataType, (int)     data, props); return;
            case DDConst.DATETIME:  checkDateTime (dataType, (long)    data, props); return;
            case DDConst.BYTES:     checkBytes    (dataType, (byte []) data, props); return;
            case DDConst.STRING:    checkString   (dataType, (String)  data, props); return;
            case DDConst.ARRAY:     checkArray    (type,               data, props); return;

            case DDConst.LIST:

                if ((props.operType & 0x0F) != DDConst.LIST)
                     DDBuffer.unexpectedData (props.id, props.operType & 0x0F, DDConst.LIST);

                DDList value = (DDList) data;
                int    i     =  value.getDataNumber ();

                if ((props.operType & DDConst.CHKMIN) != 0 && i < props.min)
                     throw new DDException (DDError.INVALID_STRUCT_SIZE.getMessage ("list", props.id, i, props.min));

                if ((props.operType & DDConst.CHKMAX) != 0 && i > props.max)
                     throw new DDException (DDError.INVALID_STRUCT_SIZE.getMessage ("list", props.id, i, props.max));

                if (props.in != null)
                {
                    for (int x = 0; x < ((DDList []) props.in).length; x++)
                    if (compareList (value,  ((DDList []) props.in) [x])) return;

                    throw new DDException (DDError.CHECK_IN_STRUCT.getMessage (props.id, props.name));
                }

                DDProps [] propsTable = value.getProperties ();
                if (propsTable == null) throw new DDException (DDError.LIST_PROPERTIES_NOT_FOUND.getMessage (props.id));

                DDProps itemProps;
                int     j, k = propsTable.length;
                Object  obj;

                value.begin ();

                while (!value.isEndOfList ())
                {
                    obj = value.getData (-1);
                    i   = value.getCurrentDataOffset ();
                    j   = value.getCurrentDataType   ();

                    itemProps = propsTable [i % k];

                    dataType = j & 0x0F;
                    value.next ();

                    switch (dataType)
                    {
                        default:               checkLong     (dataType, (long)    obj, itemProps); continue;    // long, number, s_number
                        case DDConst.BYTE:     checkByte     (dataType, (byte)    obj, itemProps); continue;
                        case DDConst.SHORT:    checkShort    (dataType, (short)   obj, itemProps); continue;
                        case DDConst.INT:      checkInt      (dataType, (int)     obj, itemProps); continue;
                        case DDConst.FLOAT:    checkFloat    (dataType, (float)   obj, itemProps); continue;
                        case DDConst.DOUBLE:   checkDouble   (dataType, (double)  obj, itemProps); continue;
                        case DDConst.TIME:     checkTime     (dataType, (int)     obj, itemProps); continue;
                        case DDConst.DATE:     checkDate     (dataType, (int)     obj, itemProps); continue;
                        case DDConst.DATETIME: checkDateTime (dataType, (long)    obj, itemProps); continue;
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

    private boolean checkCommon (int type, long data, DDProps props) throws DDException
    {
        if (type >= 0 && type != (props.operType & 0x0F))
            DDBuffer.unexpectedData (props.id, props.operType & 0x0F, type);

        if (props.in != null ) return false;

        if ((props.operType & DDConst.CHKMIN) != 0 && data < props.min)
             throw new DDException (DDError.CHECK_MIN.getMessage
                                  ((type == DDConst.BYTES || type == DDConst.STRING ? "Size" : "Value"),
                                    props.id, props.name, data, props.min));

        if ((props.operType & DDConst.CHKMAX) != 0 && data > props.max)
             throw new DDException (DDError.CHECK_MAX.getMessage
                                  ((type == DDConst.BYTES || type == DDConst.STRING ? "Size" : "Value"),
                                    props.id, props.name, data, props.max));
        return true;
    }

//  +-----------+
//  | checkByte |
//  +-----------+

    private void checkByte (int type, byte data, DDProps props) throws DDException
    {
        if (checkCommon (type, data, props)) return;

        for (byte v : (byte []) props.in)
             if  (v == data) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, data));
    }

//  +------------+
//  | checkShort |
//  +------------+

    private void checkShort (int type, short data, DDProps props) throws DDException
    {
        if (checkCommon (type, data, props)) return;

        for (short v : (short []) props.in)
             if (v == data) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, data));
    }

//  +----------+
//  | checkInt |
//  +----------+

    private void checkInt (int type, int data, DDProps props) throws DDException
    {
        if (checkCommon (type, data, props)) return;

        for (int v : (int []) props.in)
             if (v == data) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, data));
    }

//  +-----------+
//  | checkLong |
//  +-----------+

    private void checkLong (int type, long data, DDProps props) throws DDException
    {
        if (checkCommon (type, data, props)) return;

        for (long v : (long []) props.in)
             if (v == data) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, data));
    }

//  +------------+
//  | checkFloat |
//  +------------+

    private void checkFloat (int type, float data, DDProps props) throws DDException
    {
        if (type >= 0 && type != (props.operType & 0x0F))
            DDBuffer.unexpectedData (props.id, props.operType & 0x0F, type);

        if (props.in == null)
        {
            if ((props.operType & DDConst.CHKMIN) != 0 && data < Float.intBitsToFloat ((int) props.min))
                 throw new DDException (DDError.CHECK_MIN_FLOAT.getMessage (props.id, props.name, data,
                                        data, Float.intBitsToFloat ((int) props.min), props.min));

            if ((props.operType & DDConst.CHKMAX) != 0 && data > Float.intBitsToFloat ((int) props.max))
                 throw new DDException (DDError.CHECK_MAX_FLOAT.getMessage (props.id, props.name, data,
                                        data, Float.intBitsToFloat ((int) props.min), props.min));
            return;
        }

        int iv = Float.floatToIntBits (data);

        for (int v : (int []) props.in)
             if (v == iv) return;

        throw new DDException (DDError.CHECK_IN_FLOAT.getMessage (props.id, props.name, data, iv));
    }

//  +-------------+
//  | checkDouble |
//  +-------------+

    private void checkDouble (int type, double data, DDProps props) throws DDException
    {
        if (type >= 0 && type != (props.operType & 0x0F))
            DDBuffer.unexpectedData (props.id, props.operType & 0x0F, type);

        if (props.in == null)
        {
            if ((props.operType & DDConst.CHKMIN) != 0 && data < Double.longBitsToDouble (props.min))
                 throw new DDException (DDError.CHECK_MIN_FLOAT.getMessage (props.id, props.name, data,
                                        data, Double.longBitsToDouble (props.min), props.min));

            if ((props.operType & DDConst.CHKMAX) != 0 && data > Double.longBitsToDouble (props.max))
                 throw new DDException (DDError.CHECK_MAX_FLOAT.getMessage (props.id, props.name, data,
                                        data, Double.longBitsToDouble (props.max), props.max));
            return;
        }

        long lv = Double.doubleToLongBits (data);

        for (long v : (long []) props.in)
             if (v == lv) return;

        throw new DDException (DDError.CHECK_IN_FLOAT.getMessage (props.id, props.name, data, lv));
    }

//  +-----------+
//  | checkTime |
//  +-----------+

    private void checkTime (int type, int data, DDProps props) throws DDException
    {
        if (checkCommon (type, data, props)) return;

        for (int v : (int []) props.in)
             if (v == data) return;

        int ms  = data % 1000;    data /= 1000;
        int sec = data % 100;     data /= 100;
        int min = data % 100;     data /= 100;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name,
                               String.format ("%02d:%02d:%02d.%03d", data, min, sec, ms)));
    }

//  +-----------+
//  | checkDate |
//  +-----------+

    private void checkDate (int type, int data, DDProps props) throws DDException
    {
        if (checkCommon (type, data, props)) return;

        for (int v : (int []) props.in)
             if (v == data) return;

        int day = data % 100;    data /= 100;
        int mon = data % 100;    data /= 100;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name,
                  String.format ("%04d-%02d-%02d", data, mon, day)));
    }

//  +---------------+
//  | checkDateTime |
//  +---------------+

    private void checkDateTime (int type, long data, DDProps props) throws DDException
    {
        if (checkCommon (type, data, props)) return;

        for (long v : (long []) props.in)
             if (v == data) return;

        int ms   = (int)(data % 1000);    data /= 1000;
        int sec  = (int)(data % 100);     data /= 100;
        int min  = (int)(data % 100);     data /= 100;
        int hour = (int)(data % 100);     data /= 100;
        int day  = (int)(data % 100);     data /= 100;
        int mon  = (int)(data % 100);     data /= 100;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name,
                  String.format ("%04d-%02d-%02d_%02d:%02d:%02d.%03d", data, mon, day, hour, min, sec, ms)));
    }

//  +------------+
//  | checkBytes |
//  +------------+

    private void checkBytes (int type, byte [] data, DDProps props) throws DDException
    {
        if (data != null)
        {
            if (checkCommon (type, data.length, props)) return;

            for (byte [] v : (byte [][]) props.in)
                 if (Arrays.equals (data, v)) return;
        }
        else
            if ((props.operType & DDConst.CHKLIM) == 0)
                 if (props.in == null) return;
                 else for (byte [] v : (byte [][]) props.in)
                           if (v == null) return;

        throw new DDException (DDError.CHECK_IN.getMessage
                              (props.id, props.name, Arrays.toString (data).replace (" ", "")));
    }

//  +-------------+
//  | checkString |
//  +-------------+

    private void checkString (int type, String data, DDProps props) throws DDException
    {
        if (data != null)
        {
            if (checkCommon (type, data.length (), props)) return;

            for (String v : (String []) props.in)
                if (data.equals (v)) return;
        }
        else
            if ((props.operType & DDConst.CHKLIM) == 0)
                 if (props.in == null) return;
                 else for (String item : (String []) props.in)
                           if (item == null) return;

        throw new DDException (DDError.CHECK_IN.getMessage (props.id, props.name, data));
    }

//  +------------+
//  | checkArray |
//  +------------+

    private void checkArray (int type, Object data, DDProps props) throws DDException
    {
        if (type != (props.operType & 0xFF))
            DDBuffer.unexpectedData (props.id, props.operType & 0xFF, type);

        int  len;
        type >>= 4;

        switch (type)
        {
            default:               len = ((long   []) data).length; break;    // long, number, s_number, datetime
            case DDConst.BYTE:     len = ((byte   []) data).length; break;
            case DDConst.SHORT:    len = ((short  []) data).length; break;
            case DDConst.TIME:
            case DDConst.DATE:
            case DDConst.INT:      len = ((int    []) data).length; break;
            case DDConst.FLOAT:    len = ((float  []) data).length; break;
            case DDConst.DOUBLE:   len = ((double []) data).length; break;
            case DDConst.BYTES:    len = ((byte [][]) data).length; break;
            case DDConst.STRING:   len = ((String []) data).length;
        }

        if (props.number > 0 && props.number != len)
            throw new DDException (DDError.INVALID_STRUCT_SIZE.getMessage ("array", props.id, len,  props.number));

        if (props.number < 0 && props.number > -len)
            throw new DDException (DDError.INVALID_STRUCT_SIZE.getMessage ("array", props.id, len, -props.number));

        if (props.in != null)
        {
            for (Object obj : (Object [][]) props.in)
                if (compareArray (type, data, obj)) return;

            throw new DDException (DDError.CHECK_IN_STRUCT.getMessage (props.id, props.name));
        }

        int i = 0;

        switch (type)
        {
            default:                for (; i < len; i++) checkLong     (-1, ((long   []) data) [i], props); break;
            case DDConst.BYTE:      for (; i < len; i++) checkByte     (-1, ((byte   []) data) [i], props); break;
            case DDConst.SHORT:     for (; i < len; i++) checkShort    (-1, ((short  []) data) [i], props); break;
            case DDConst.INT:       for (; i < len; i++) checkInt      (-1, ((int    []) data) [i], props); break;
            case DDConst.FLOAT:     for (; i < len; i++) checkFloat    (-1, ((float  []) data) [i], props); break;
            case DDConst.DOUBLE:    for (; i < len; i++) checkDouble   (-1, ((double []) data) [i], props); break;
            case DDConst.TIME:      for (; i < len; i++) checkTime     (-1, ((int    []) data) [i], props); break;
            case DDConst.DATE:      for (; i < len; i++) checkDate     (-1, ((int    []) data) [i], props); break;
            case DDConst.DATETIME:  for (; i < len; i++) checkDateTime (-1, ((long   []) data) [i], props); break;
            case DDConst.BYTES:     for (; i < len; i++) checkBytes    (-1, ((byte [][]) data) [i], props); break;
            case DDConst.STRING:    for (; i < len; i++) checkString   (-1, ((String []) data) [i], props);
        }
    }

//  +-------------+
//  | compareData |
//  +-------------+

    private boolean compareData (int type, final Object data1, Object data2) throws DDException
    {
        switch (type & 0x0F)
        {
            default:            return data1.equals  ( data2);
            case DDConst.BYTES: return Arrays.equals ((byte []) data1, (byte []) data2);
            case DDConst.ARRAY: return compareArray  ( type,    data1,  data2);
            case DDConst.LIST:  return compareList   ( data1,   data2);
        }
    }

//  +--------------+
//  | compareArray |
//  +--------------+

    private boolean compareArray (int type, final Object array1, Object array2)
    {
        switch (type >> 4)
        {
            default:             return Arrays.equals ((long   []) array1, (long   []) array2);
            case DDConst.BYTE:   return Arrays.equals ((byte   []) array1, (byte   []) array2);
            case DDConst.SHORT:  return Arrays.equals ((short  []) array1, (short  []) array2);
            case DDConst.TIME:
            case DDConst.DATE:
            case DDConst.INT:    return Arrays.equals ((int    []) array1, (int    []) array2);
            case DDConst.FLOAT:  return Arrays.equals ((float  []) array1, (float  []) array2);
            case DDConst.DOUBLE: return Arrays.equals ((double []) array1, (double []) array2);

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

//  +-------------+
//  | compareList |
//  +-------------+

    private boolean compareList (Object data1, Object data2) throws DDException
    {
        DDList list1 = (DDList) data1;
        DDList list2 = (DDList) data2;

        if (list1.getDataNumber () != list2.getDataNumber ()) return false;

        Object obj1, obj2;
        int    i,    j;

        list1.begin ();
        list2.begin ();

        while (!list1.isEndOfList ())
        {
            obj1 = list1.getData (-1);
            obj2 = list2.getData (-1);
            i    = list1.getCurrentDataType ();
            j    = list2.getCurrentDataType ();

            list1.next ();
            list2.next ();

            if (i == DDConst.NULL || j == DDConst.NULL)
                if (obj1 == null && obj2 == null) continue;
                else return false;

            if (i != j) return false;

            switch (i & 0x0F)
            {
                default:            if (obj1.equals   (obj2)) continue; else return false;
                case DDConst.BYTES: if (Arrays.equals ((byte []) obj1, (byte []) obj2)) continue; else return false;
                case DDConst.ARRAY: if (!compareArray (i, obj1, obj2)) return false;
            }
        }

        return true;
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

        int [] type = new int [1];
        int    id;

        while (!ddb.endOfBuffer ())
        {
            id       =  ddb.readTypeAndId ();
            type [0] =  id >> 12;
            storeData  (id & 0xFFF, ddb.read (type), type [0]);
        }
    }

//  +------------+
//  | linkObject |
//  +------------+

    private final Object [] linkObject (int id, Object [] obj)
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

    private void storeData (int id, final Object data, int type) throws DDException
    {
        if (dataTypes [id] != DDConst.NULL) throw new DDException (DDError.DATA_ALREADY_PRESENT.getMessage (id));

        if (dataDefault)
        {
            DDProps props = (DDProps) seekObject (id, propertiesRoot);
            if (props != null && props.def != null && (props.operType & DDConst.REQ) == 0 &&
                compareData (type, data, props.def)) return;                               // Default
        }

        dataTypes  [id] =  (byte)  type;
        linkObject (id, dataRoot) [id & 0x0F] = data;
        dataNumber++;
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

        if (propertiesNumber > 0)
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

                        if (dataTypes [x] != DDConst.LIST) continue;

                        list = (DDList) seekObject (x, dataRoot);
                        propsTable = list.getProperties ();

                        if (propsTable == null)
                            throw new DDException (DDError.LIST_PROPERTIES_NOT_FOUND.getMessage (((DDProps) data).id));

                        x = propsTable.length;
                        list.begin ();

                        while (!list.isEndOfList ())
                        {
                            props = propsTable [list.getCurrentDataOffset () % x];

                            if ((props.operType & DDConst.REQ) != 0 && list.getCurrentDataType () == DDConst.NULL)
                                 throw new DDException (DDError.MISSING_DATA.getMessage (props.id, props.name));

                            list.next ();
                        }

                        for (x = list.getDataNumber (); x < propsTable.length; x++)
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

//  #=====================#
//  # Date Time Utilities #
//  #=====================#

    public static int getYear   (int date) {return  date /    10000; }
    public static int getMonth  (int date) {return (date /      100) %  100;}
    public static int getDay    (int date) {return  date             %  100;}
    public static int getHour   (int time) {return  time / 10000000; }
    public static int getMinute (int time) {return (time /   100000) %  100;}
    public static int getSecond (int time) {return (time /     1000) %  100;}
    public static int getMSec   (int time) {return  time             % 1000;}

    public static int getYear   (long datetime) {return getYear   ((int)(datetime / 1000000000));}
    public static int getMonth  (long datetime) {return getMonth  ((int)(datetime / 1000000000));}
    public static int getDay    (long datetime) {return getDay    ((int)(datetime / 1000000000));}
    public static int getHour   (long datetime) {return getHour   ((int)(datetime % 1000000000));}
    public static int getMinute (long datetime) {return getMinute ((int)(datetime % 1000000000));}
    public static int getSecond (long datetime) {return getSecond ((int)(datetime % 1000000000));}
    public static int getMSec   (long datetime) {return getMSec   ((int)(datetime % 1000000000));}

    public static int getTime (int hour, int minute, int second, int msec) throws DDException
    {
        if (msec < 1000 && second < 60 && minute < 60 && hour >= 0 && hour < 24)
            return ((hour * 100 + minute) * 100 + second) * 1000 + msec;

        return -1;
    }

    public static int getDate (int year, int month, int day) throws DDException
    {
        if (year != 0 && year <= 214747 && month != 0 && month <= 12 && day != 0 &&
           (day <= DDBuffer.MONTH_DAYS [month - 1] ||
           (month == 2 && day == 29 && (year % 400) == 0 || ((year % 4) == 0 && (year % 100) != 0))))    // Leap Year
                return (year * 100 + month) * 100 + day;

        return -1;
    }

    public static long getDateTime (int year, int month, int day, int  hour, int minute, int second, int msec)
        throws DDException
    {
        if (msec < 1000 && second < 60 && minute < 60 && hour < 24 &&
            year != 0 && year <= 214747 && month != 0 && month <= 12 && day != 0 &&
           (day <= DDBuffer.MONTH_DAYS [month - 1] ||
           (month == 2 && day == 29 && (year % 400) == 0 || ((year % 4) == 0 && (year % 100) != 0))))    // Leap Year
                return (((((year * 100 + month) * 100 + day) * 100 + hour) * 100 + minute) * 100 + second) * 1000 + msec;

        return -1;
    }

    public static int getTime (final Date time)
    {
        if (time == null) return -1;

        Calendar cal = Calendar.getInstance ();
        cal.setTime (time);

        return ((cal.get (Calendar.HOUR_OF_DAY) * 100  +
                 cal.get (Calendar.MINUTE))     * 100  +
                 cal.get (Calendar.SECOND))     * 1000 +
                 cal.get (Calendar.MILLISECOND);
    }

    public static int getDate (final Date date)
    {
        if (date == null) return -1;

        Calendar cal = Calendar.getInstance ();
        cal.setTime (date);

        return (cal.get (Calendar.YEAR)         * 100 +
                cal.get (Calendar.MONTH) + 1)   * 100 +
                cal.get (Calendar.DAY_OF_MONTH);
    }

    public static long getDateTime (final Date datetime)
    {
        if (datetime == null) return -1;

        Calendar cal = Calendar.getInstance ();
        cal.setTime (datetime);

        return (((((cal.get (Calendar.YEAR)          * 100  +
                    cal.get (Calendar.MONTH) + 1)    * 100  +
                    cal.get (Calendar.DAY_OF_MONTH)) * 100  +
                    cal.get (Calendar.HOUR_OF_DAY))  * 100  +
                    cal.get (Calendar.MINUTE))       * 100  +
                    cal.get (Calendar.SECOND))       * 1000 +
                    cal.get (Calendar.MILLISECOND);
    }

    public static Date getJTime (int time)
    {
        Calendar cal = Calendar.getInstance ();
        cal.clear ();

        cal.set (Calendar.MILLISECOND, time % 1000);    time /= 1000;
        cal.set (Calendar.SECOND,      time % 100);     time /= 100;
        cal.set (Calendar.MINUTE,      time % 100);
        cal.set (Calendar.HOUR_OF_DAY, time / 100);

        return cal.getTime ();
    }

    public static Date getJDate (int date)
    {
        Calendar cal = Calendar.getInstance ();
        cal.clear ();

        cal.set (Calendar.DAY_OF_MONTH, date % 100);        date /= 100;
        cal.set (Calendar.MONTH,        date % 100 - 1);
        cal.set (Calendar.YEAR,         date / 100);

        return cal.getTime ();
    }

    public static Date getJDateTime (long datetime)
    {
        Calendar cal = Calendar.getInstance ();
        cal.clear ();

        cal.set (Calendar.MILLISECOND,  (int)(datetime % 1000));       datetime /= 1000;
        cal.set (Calendar.SECOND,       (int)(datetime % 100));        datetime /= 100;
        cal.set (Calendar.MINUTE,       (int)(datetime % 100));        datetime /= 100;
        cal.set (Calendar.HOUR_OF_DAY,  (int)(datetime % 100));        datetime /= 100;
        cal.set (Calendar.DAY_OF_MONTH, (int)(datetime % 100));        datetime /= 100;
        cal.set (Calendar.MONTH,        (int)(datetime % 100 - 1));
        cal.set (Calendar.YEAR,         (int)(datetime / 100));

        return cal.getTime ();
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

        sb.append (dataNumber).append ('{');

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
                    DDBuffer.stringData (sb, data, dataTypes [x] & 0xFF);
                }
            }
        }

        sb.append ('}');
        return sb.toString ();
    }
}