//  #========================================#
//  # Dynamic Data Constants - Version 1.0.0 #
//  #                                        #
//  # Author: Marco Vagnoni                  #
//  # Email:  marco.vagnoni@yahoo.com        #
//  # Date:   April     2019 (v1.0.0)        #
//  #========================================#

package it.xv2.ddm;

public final class DDConst
{
    static final int MAX_BYTES_NUMBER  = 65530;
    static final int MAX_ITEMS_NUMBER  = 65530;

    public static final int NULL       = 0x00000000;
    public static final int BYTE       = 0x00000001;
    public static final int SHORT      = 0x00000002;
    public static final int INT        = 0x00000003;
    public static final int LONG       = 0x00000004;
    public static final int NUMBER     = 0x00000005;
    public static final int S_NUMBER   = 0x00000006;
    public static final int FLOAT      = 0x00000007;
    public static final int DOUBLE     = 0x00000008;
    public static final int TIME       = 0x00000009;
    public static final int DATE       = 0x0000000A;
    public static final int DATETIME   = 0x0000000B;
    public static final int BYTES      = 0x0000000C;
    public static final int STRING     = 0x0000000D;
    public static final int ARRAY      = 0x0000000E;
    public static final int LIST       = 0x0000000F;

    public static final int A_BYTE     = 0x0000001E;
    public static final int A_SHORT    = 0x0000002E;
    public static final int A_INT      = 0x0000003E;
    public static final int A_LONG     = 0x0000004E;
    public static final int A_NUMBER   = 0x0000005E;
    public static final int A_S_NUMBER = 0x0000006E;
    public static final int A_FLOAT    = 0x0000007E;
    public static final int A_DOUBLE   = 0x0000008E;
    public static final int A_TIME     = 0x0000009E;
    public static final int A_DATE     = 0x000000AE;
    public static final int A_DATETIME = 0x000000BE;
    public static final int A_BYTES    = 0x000000CE;
    public static final int A_STRING   = 0x000000DE;

    public static final int CHKMIN     = 0x00000100;
    public static final int CHKMAX     = 0x00000200;
    public static final int CHKLIM     = 0x00000300;
    public static final int REQ        = 0x00001000;
}