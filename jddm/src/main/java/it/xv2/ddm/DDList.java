//  #===================================#
//  # Dynamic Data List - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

package it.xv2.ddm;

import java.util.Date;

public final class DDList
{
    private boolean endOfList = true;
    private int     dataCount;

    private DDNode  currentNode;
    private DDNode  firstNode;
    private DDNode  lastNode;

//  #=====#
//  # get #
//  #=====#

    public int getCurrentDataId   () {return currentNode == null ? -1 : currentNode.getId   ();}
    public int getCurrentDataType () {return currentNode == null ? -1 : currentNode.getType ();}
    public int getDataCount       () {return dataCount;}

    public byte    getByte                () throws DDException {return (Byte)      getData (DDConst.BYTE);}
    public short   getShort               () throws DDException {return (Short)     getData (DDConst.SHORT);}
    public int     getInt                 () throws DDException {return (Integer)   getData (DDConst.INT);}
    public long    getLong                () throws DDException {return (Long)      getData (DDConst.LONG);}
    public float   getFloat               () throws DDException {return (Float)     getData (DDConst.FLOAT);}
    public double  getDouble              () throws DDException {return (Double)    getData (DDConst.DOUBLE);}
    public long    getNumber              () throws DDException {return (Long)      getData (DDConst.NUMBER);}
    public long    getSignedNumber        () throws DDException {return (Long)      getData (DDConst.S_NUMBER);}
    public Date    getTime                () throws DDException {return (Date)      getData (DDConst.TIME);}
    public Date    getDate                () throws DDException {return (Date)      getData (DDConst.DATE);}
    public Date    getDateTime            () throws DDException {return (Date)      getData (DDConst.DATETIME);}
    public byte [] getBytes               () throws DDException {return (byte [])   getData (DDConst.BYTES);}
    public String  getString              () throws DDException {return (String)    getData (DDConst.STRING);}
    public DDList  getList                () throws DDException {return (DDList)    getData (DDConst.LIST);}

    public byte   [] getByteArray         () throws DDException {return (byte   []) getData (DDConst.A_BYTE);}
    public short  [] getShortArray        () throws DDException {return (short  []) getData (DDConst.A_SHORT);}
    public int    [] getIntArray          () throws DDException {return (int    []) getData (DDConst.A_INT);}
    public long   [] getLongArray         () throws DDException {return (long   []) getData (DDConst.A_LONG);}
    public float  [] getFloatArray        () throws DDException {return (float  []) getData (DDConst.A_FLOAT);}
    public double [] getDoubleArray       () throws DDException {return (double []) getData (DDConst.A_DOUBLE);}
    public long   [] getNumberArray       () throws DDException {return (long   []) getData (DDConst.A_NUMBER);}
    public long   [] getSignedNumberArray () throws DDException {return (long   []) getData (DDConst.A_S_NUMBER);}
    public Date   [] getTimeArray         () throws DDException {return (Date   []) getData (DDConst.A_TIME);}
    public Date   [] getDateArray         () throws DDException {return (Date   []) getData (DDConst.A_DATE);}
    public Date   [] getDateTimeArray     () throws DDException {return (Date   []) getData (DDConst.A_DATETIME);}
    public byte [][] getBytesArray        () throws DDException {return (byte [][]) getData (DDConst.A_BYTES);}
    public String [] getStringArray       () throws DDException {return (String []) getData (DDConst.A_STRING);}

//  #====#
//  # is #
//  #====#

    public boolean isEndOfList () {return endOfList;}

//  #=====#
//  # add #
//  #=====#

    public void addByte              (byte      data) throws DDException {addData (data, DDConst.BYTE);}
    public void addShort             (short     data) throws DDException {addData (data, DDConst.SHORT);}
    public void addInt               (int       data) throws DDException {addData (data, DDConst.INT);}
    public void addLong              (long      data) throws DDException {addData (data, DDConst.LONG);}
    public void addFloat             (float     data) throws DDException {addData (data, DDConst.FLOAT);}
    public void addDouble            (double    data) throws DDException {addData (data, DDConst.DOUBLE);}
    public void addNumber            (long      data) throws DDException {addData (data, DDConst.NUMBER);}
    public void addSignedNumber      (long      data) throws DDException {addData (data, DDConst.S_NUMBER);}
    public void addTime              (Date      data) throws DDException {addData (data, DDConst.TIME);}
    public void addDate              (Date      data) throws DDException {addData (data, DDConst.DATE);}
    public void addDateTime          (Date      data) throws DDException {addData (data, DDConst.DATETIME);}
    public void addBytes             (byte   [] data) throws DDException {addData (data, DDConst.BYTES);}
    public void addString            (String    data) throws DDException {addData (data, DDConst.STRING);}

    public void addByteArray         (byte   [] data) throws DDException {addData (data, DDConst.A_BYTE);}
    public void addShortArray        (short  [] data) throws DDException {addData (data, DDConst.A_SHORT);}
    public void addIntArray          (int    [] data) throws DDException {addData (data, DDConst.A_INT);}
    public void addLongArray         (long   [] data) throws DDException {addData (data, DDConst.A_LONG);}
    public void addFloatArray        (float  [] data) throws DDException {addData (data, DDConst.A_FLOAT);}
    public void addDoubleArray       (double [] data) throws DDException {addData (data, DDConst.A_DOUBLE);}
    public void addNumberArray       (long   [] data) throws DDException {addData (data, DDConst.A_NUMBER);}
    public void addSignedNumberArray (long   [] data) throws DDException {addData (data, DDConst.A_S_NUMBER);}
    public void addTimeArray         (Date   [] data) throws DDException {addData (data, DDConst.A_TIME);}
    public void addDateArray         (Date   [] data) throws DDException {addData (data, DDConst.A_DATE);}
    public void addDateTimeArray     (Date   [] data) throws DDException {addData (data, DDConst.A_DATETIME);}
    public void addBytesArray        (byte [][] data) throws DDException {addData (data, DDConst.A_BYTES);}
    public void addStringArray       (String [] data) throws DDException {addData (data, DDConst.A_STRING);}

//  #=========#
//  # addData #
//  #=========#

    public void addData (Object data, int type) throws DDException
    {
        boolean valid = true;

        if (type < DDConst.NULL || type > DDConst.A_STRING || (type > DDConst.ARRAY && type < DDConst.A_BYTE) ||
           (type == DDConst.NUMBER && (Long) data < 0)) valid = false;
        else
            if (type == DDConst.A_NUMBER)
                for (int i = 0; i < ((long []) data).length; i++)
                    if (((long []) data) [i] < 0) {valid = false; break;}

        if (!valid) throw new DDException (DDError.INVALID_DATA.getMessage (dataCount));

        if (dataCount >= DDConst.MAX_ITEMS_NUMBER)
            throw new DDException (DDError.LIST_OVERFLOW.getMessage (dataCount, DDConst.MAX_ITEMS_NUMBER));

        if (data == null) type = DDConst.NULL;

        DDNode newNode = new DDNode (dataCount++, data, type);

        if (firstNode == null) firstNode = newNode;
        else lastNode.setNext (newNode);

        lastNode = newNode;
    }

//  #=========#
//  # getData #
//  #=========#

    public Object getData (int type) throws DDException
    {
        if (currentNode == null) return null;

        if (type > DDConst.NULL && currentNode.getType () > DDConst.NULL && type != currentNode.getType ())
        {
            int expectedIdH = currentNode.getType () >> 4;
            int requiredIdH = type >> 4;

            throw new DDException (DDError.INVALID_DATA_TYPE.getMessage (currentNode.getId (),
                          (expectedIdH == 0 ? "" : DDBuffer.STRING_TYPES [expectedIdH]) + DDBuffer.STRING_TYPES [currentNode.getType () & 0x0F],
                          (requiredIdH == 0 ? "" : DDBuffer.STRING_TYPES [requiredIdH]) + DDBuffer.STRING_TYPES [type & 0x0F]));
        }

        return currentNode.getObject ();
    }

//  #=======#
//  # begin #
//  #=======#

    public void begin ()
    {
        currentNode = firstNode;
        endOfList   = currentNode == null;
    }

//  #======#
//  # next #
//  #======#

    public void next ()
    {
        if (currentNode != null)
        {
            currentNode = currentNode.getNext ();
            if (currentNode == null) endOfList = true;
        }
    }

//  #==========#
//  # toString #
//  #==========#

    @Override
    public String toString ()
    {
        StringBuilder sb = new StringBuilder (1024);
        toString (sb, null);
        return sb.toString ();
    }

    public void toString (StringBuilder sb, DDProps props)
    {
        sb.append (getDataCount ()).append ('{');

        Object     obj  = null;
        boolean    sep  = false;
        int        i, j = 0;
        DDProps [] p    = props == null ? null : (DDProps []) props.inOrProps;

        if (p != null) j = p.length;
        begin ();

        while (!isEndOfList ())
        {
            if (sep) sb.append (';');
            sep = true;

            if (p != null) sb.append (p [getCurrentDataId () % j].name).append ('@');
            i = getCurrentDataType ();

            try   {obj = getData (-1);}
            catch (Exception ignore) {}

            if (i != DDConst.NULL)
            {
                sb.append  (DDBuffer.DATA_TYPES [i & 0x0F]);
                DDBuffer.stringData (sb, obj, i, null);
            }

            next ();
        }

        sb.append ('}');
    }
}