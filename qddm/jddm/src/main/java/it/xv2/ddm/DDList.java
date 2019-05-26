//  #===================================#
//  # Dynamic Data List - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Email:  marco.vagnoni@yahoo.com   #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

package it.xv2.ddm;

public final class DDList
{
    private boolean    endOfList = true;
    private int        dataNumber;
    private DDProps [] properties;

    private DDNode     currentNode;
    private DDNode     firstNode;
    private DDNode     lastNode;

//  #=====#
//  # get #
//  #=====#

    public int        getCurrentDataOffset () {return currentNode == null ? -1 : currentNode.getOffset ();}
    public int        getCurrentDataType   () {return currentNode == null ? -1 : currentNode.getType   ();}
    public int        getDataNumber        () {return dataNumber;}
    public DDProps [] getProperties        () {return properties;}

    public byte       getByte              ()  throws DDException {return (byte)      getData (DDConst.BYTE);}
    public short      getShort             ()  throws DDException {return (short)     getData (DDConst.SHORT);}
    public int        getInt               ()  throws DDException {return (int)       getData (DDConst.INT);}
    public long       getLong              ()  throws DDException {return (long)      getData (DDConst.LONG);}
    public float      getFloat             ()  throws DDException {return (float)     getData (DDConst.FLOAT);}
    public double     getDouble            ()  throws DDException {return (double)    getData (DDConst.DOUBLE);}
    public long       getNumber            ()  throws DDException {return (long)      getData (DDConst.NUMBER);}
    public long       getSignedNumber      ()  throws DDException {return (long)      getData (DDConst.S_NUMBER);}
    public int        getTime              ()  throws DDException {return (int)       getData (DDConst.TIME);}
    public int        getDate              ()  throws DDException {return (int)       getData (DDConst.DATE);}
    public long       getDateTime          ()  throws DDException {return (long)      getData (DDConst.DATETIME);}
    public byte   []  getBytes             ()  throws DDException {return (byte [])   getData (DDConst.BYTES);}
    public String     getString            ()  throws DDException {return (String)    getData (DDConst.STRING);}
    public DDList     getList              ()  throws DDException {return (DDList)    getData (DDConst.LIST);}
    public byte   []  getByteArray         ()  throws DDException {return (byte   []) getData (DDConst.A_BYTE);}
    public short  []  getShortArray        ()  throws DDException {return (short  []) getData (DDConst.A_SHORT);}
    public int    []  getIntArray          ()  throws DDException {return (int    []) getData (DDConst.A_INT);}
    public long   []  getLongArray         ()  throws DDException {return (long   []) getData (DDConst.A_LONG);}
    public float  []  getFloatArray        ()  throws DDException {return (float  []) getData (DDConst.A_FLOAT);}
    public double []  getDoubleArray       ()  throws DDException {return (double []) getData (DDConst.A_DOUBLE);}
    public long   []  getNumberArray       ()  throws DDException {return (long   []) getData (DDConst.A_NUMBER);}
    public long   []  getSignedNumberArray ()  throws DDException {return (long   []) getData (DDConst.A_S_NUMBER);}
    public int    []  getTimeArray         ()  throws DDException {return (int    []) getData (DDConst.A_TIME);}
    public int    []  getDateArray         ()  throws DDException {return (int    []) getData (DDConst.A_DATE);}
    public long   []  getDateTimeArray     ()  throws DDException {return (long   []) getData (DDConst.A_DATETIME);}
    public byte [][]  getBytesArray        ()  throws DDException {return (byte [][]) getData (DDConst.A_BYTES);}
    public String []  getStringArray       ()  throws DDException {return (String []) getData (DDConst.A_STRING);}

//  #====#
//  # is #
//  #====#

    public boolean isEndOfList () {return endOfList;}

//  #=====#
//  # set #
//  #=====#

    public void setProperties (DDProps [] properties) {this.properties = properties;}

//  #=====#
//  # add #
//  #=====#

    public void addByte              (byte      data) throws DDException {addData (data, DDConst.BYTE);}
    public void addShort             (short     data) throws DDException {addData (data, DDConst.SHORT);}
    public void addInt               (int       data) throws DDException {addData (data, DDConst.INT);}
    public void addLong              (long      data) throws DDException {addData (data, DDConst.LONG);}
    public void addFloat             (float     data) throws DDException {addData (data, DDConst.FLOAT);}
    public void addDouble            (double    data) throws DDException {addData (data, DDConst.DOUBLE);}
    public void addSignedNumber      (long      data) throws DDException {addData (data, DDConst.S_NUMBER);}
    public void addBytes             (byte   [] data) throws DDException {addData (data, DDConst.BYTES);}
    public void addString            (String    data) throws DDException {addData (data, DDConst.STRING);}
    public void addByteArray         (byte   [] data) throws DDException {addData (data, DDConst.A_BYTE);}
    public void addShortArray        (short  [] data) throws DDException {addData (data, DDConst.A_SHORT);}
    public void addIntArray          (int    [] data) throws DDException {addData (data, DDConst.A_INT);}
    public void addLongArray         (long   [] data) throws DDException {addData (data, DDConst.A_LONG);}
    public void addFloatArray        (float  [] data) throws DDException {addData (data, DDConst.A_FLOAT);}
    public void addDoubleArray       (double [] data) throws DDException {addData (data, DDConst.A_DOUBLE);}
    public void addSignedNumberArray (long   [] data) throws DDException {addData (data, DDConst.A_S_NUMBER);}
    public void addBytesArray        (byte [][] data) throws DDException {addData (data, DDConst.A_BYTES);}
    public void addStringArray       (String [] data) throws DDException {addData (data, DDConst.A_STRING);}

    public void addNumber (long data) throws DDException
        {addData (data, (data < 0 ? DDConst.S_NUMBER : DDConst.NUMBER));}

    public void addTime (int data) throws DDException
    {
        if (!DDBuffer.isValidTime (data)) throw new DDException (DDError.INVALID_LIST_DATA.getMessage (dataNumber));
        addData (data, DDConst.TIME);
    }

    public void addDate (int data) throws DDException
    {
        if (!DDBuffer.isValidDate (data)) throw new DDException (DDError.INVALID_LIST_DATA.getMessage (dataNumber));
        addData (data, DDConst.DATE);
    }

    public void addDateTime (long data) throws DDException
    {
        if (!DDBuffer.isValidDateTime (data)) throw new DDException (DDError.INVALID_LIST_DATA.getMessage (dataNumber));
        addData (data, DDConst.DATETIME);
    }

    public void addNumberArray (long [] data) throws DDException
    {
        int type = DDConst.A_NUMBER;

        if (data != null)
            for (long v : data)
                 if  (v < 0) {type = DDConst.A_S_NUMBER; break;}

        addData (data, type);
    }

    public void addTimeArray (int [] data) throws DDException
    {
        if (data != null)
            for (int v : data)
                 if (!DDBuffer.isValidTime (v)) throw new DDException (DDError.INVALID_LIST_DATA.getMessage (dataNumber));

        addData (data, DDConst.A_TIME);
    }

    public void addDateArray (int [] data) throws DDException
    {
        if (data != null)
            for (int v : data)
                 if (!DDBuffer.isValidDate (v)) throw new DDException (DDError.INVALID_LIST_DATA.getMessage (dataNumber));

        addData (data, DDConst.A_DATE);
    }

    public void addDateTimeArray (long [] data) throws DDException
    {
        if (data != null)
            for (long v : data)
                 if (!DDBuffer.isValidDateTime (v)) throw new DDException (DDError.INVALID_LIST_DATA.getMessage (dataNumber));

        addData (data, DDConst.A_DATETIME);
    }

//  #=========#
//  # addData #
//  #=========#

    void addData (Object data, int type) throws DDException
    {
        if (dataNumber >= DDConst.MAX_ITEMS_NUMBER)
            throw new DDException (DDError.LIST_OVERFLOW.getMessage (dataNumber, DDConst.MAX_ITEMS_NUMBER));

        DDNode newNode = new DDNode (dataNumber++, data, (data == null ? DDConst.NULL : type));

        if (firstNode == null) firstNode = newNode;
        else lastNode.setNext (newNode);

        lastNode = newNode;
    }

//  #=========#
//  # getData #
//  #=========#

    Object getData (int type) throws DDException
    {
        if (currentNode == null) throw new DDException (DDError.END_OF_LIST.getMessage ());
        int currentType =  currentNode.getType ();

        if (type >= DDConst.NULL   && type != currentType       &&
           (type <  DDConst.LONG   || type > DDConst.S_NUMBER   || currentType < DDConst.LONG   || currentType > DDConst.S_NUMBER) &&
           (type <  DDConst.A_LONG || type > DDConst.A_S_NUMBER || currentType < DDConst.A_LONG || currentType > DDConst.A_S_NUMBER))
                DDBuffer.unexpectedData (currentNode.getOffset (), currentType, type);

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
        toString (sb);
        return sb.toString ();
    }

    public void toString (StringBuilder sb)
    {
        sb.append (getDataNumber ()).append ('{');

        Object  obj  = null;
        boolean sep  = false;
        int     i, j = 0, k;

        if (properties != null) j = properties.length;
        begin ();

        while (!isEndOfList ())
        {
            if (sep) sb.append (';');
            sep = true;

            k = getCurrentDataOffset ();

            if (j > 0) sb.append (properties [k % j].name).append ('@').append (properties [k % j].id);
            sb.append ('#').append (k).append ('=');

            i = getCurrentDataType ();

            try   {obj = getData (-1);}
            catch (Exception ignore) {}

            if (i == DDConst.NULL) sb.append ("null");
            else
            {
                sb.append  (DDBuffer.DATA_TYPES [i & 0x0F]);
                DDBuffer.stringData (sb, obj, i);
            }

            next ();
        }

        sb.append ('}');
    }
}