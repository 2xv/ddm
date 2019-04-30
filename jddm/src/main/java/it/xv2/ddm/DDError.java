//  #====================================#
//  # Dynamic Data Error - Version 1.0.0 #
//  #                                    #
//  # Author: Marco Vagnoni              #
//  # Date:   April     2019 (v1.0.0)    #
//  #====================================#

package it.xv2.ddm;

public enum DDError
{                            //  DDBuffer
    BUFFER_OVERFLOW            ("Buffer overflow [requiredBytes=%s, position=%s, length=%s]"),
    INVALID_ARRAY_ITEM         ("Invalid array item [itemType=%s, itemNumbers=%s, position=%s, length=%s]"),
    INVALID_CRC                ("Invalid CRC [expected=0x%s, received=0x%s]"),
    INVALID_DYNAMIC_NUMBER     ("Invalid dynamic number [firstByte=0x%s, lastByte=0x%s, position=%s, length=%s]"),
    INVALID_PROTOCOL_INFO      ("Invalid protocol info [expected=0x4444501(DDP1), received=0x%s]"),
    INVALID_SIZE               ("Invalid %s [size=%s, max=%s, type=%s, position=%s, length=%s]"),
    INVALID_VALUE              ("Invalid %s [value=%s, type=%s, position=%s, length=%s]"),

                             //  DynamicDataManager
    CHECK_IN                   ("Value not allowed [id=%s, name=%s, value=%s]"),
    CHECK_IN_FLOAT             ("Value not allowed [id=%s, name=%s, value=%s, bitsValue=0x%s]"),
    CHECK_MIN                  ("%s < min [id=%s, name=%s, value=%s, min=%s]"),
    CHECK_MAX                  ("%s > max [id=%s, name=%s, value=%s, max=%s]"),
    CHECK_MIN_FLOAT            ("Value < min [id=%s, name=%s, value=%s, bitsValue=0x%s, min=%s, bitsMin=0x%s]"),
    CHECK_MAX_FLOAT            ("Value > max [id=%s, name=%s, value=%s, bitsValue=0x%s, max=%s, bitsMax=0x%s]"),
    DATA_ALREADY_PRESENT       ("Data already present [id=%s]"),
    DATA_IS_NULL               ("Data is null [id=%s]"),
    DATA_NOT_FOUND             ("Data not found [id=%s]"),
    INVALID_STRUCT_SIZE        ("Invalid %s size [id=%s, size=%s expected=%s]"),
    INVALID_DATA               ("Invalid data [id=%s]"),
    INVALID_DATA_TYPE          ("Invalid data type [id=%s, expected=%s, requested=%s]"),
    INVALID_DECODE_ARGUMENT    ("Invalid decode argument [buffer=%s, offset=%s, length=%s]"),
    INVALID_ENCODE_ARGUMENT    ("Invalid encode argument [reservedSizeBefore=%s, reservedSizeAfter=%s]"),
    INVALID_ID                 ("Invalid id [id=%s]"),
    LIST_OVERFLOW              ("List overflow [itemNumber=%s, itemMax=%s]"),
    MISSING_DATA               ("Required data is missing [id=%s, name=%s]"),
    PROPERTIES_ALREADY_PRESENT ("Properties already present [id=%s]"),
    PROPERTIES_IS_NULL         ("Properties is null"),
    PROPERTIES_NOT_FOUND       ("Properties not found [id=%s]"),
;
    private final String description;

    DDError (String description) {this.description = description;}

    String getMessage ()                      {return getStackInfo () + description;}
    String getMessage (Object ... parameters) {return getStackInfo () + String.format (description, parameters);}

    private String getStackInfo ()
    {
        StackTraceElement stackInfo = Thread.currentThread ().getStackTrace () [3];
        return stackInfo.getMethodName () + " (" +
               stackInfo.getFileName   () + '@' +
               stackInfo.getLineNumber () + ") ";
    }
}