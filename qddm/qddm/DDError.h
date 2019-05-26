//  #====================================#
//  # Dynamic Data Error - Version 1.0.0 #
//  #                                    #
//  # Author: Marco Vagnoni              #
//  # Email:  marco.vagnoni@yahoo.com    #
//  # Date:   April     2019 (v1.0.0)    #
//  #====================================#

#ifndef DDERROR_H
#define DDERROR_H

#include <QString>
                                               //  DDBuffer
#define DDE_BUFFER_OVERFLOW            "%s (%s@%d) Buffer overflow [requiredBytes=%d, position=%d, length=%d]"
#define DDE_INVALID_ARRAY_ITEM         "%s (%s@%d) Invalid array item [itemType=%d, itemNumbers=%lld, position=%d, length=%d]"
#define DDE_INVALID_CRC                "%s (%s@%d) Invalid CRC [expected=0x%x, found=0x%x]"
#define DDE_INVALID_DYNAMIC_NUMBER     "%s (%s@%d) Invalid dynamic number [firstByte=0x%x, lastByte=0x%x, position=%d, length=%d]"
#define DDE_INVALID_PROTOCOL_INFO      "%s (%s@%d) Invalid protocol info [expected=0x4444501(DDP1), received=0x%x]"
#define DDE_INVALID_SIZE               "%s (%s@%d) Invalid %s [size=%lld, max=%d, type=%d, position=%d, length=%d]"
#define DDE_INVALID_VALUE              "%s (%s@%d) Invalid %s [value=%s, type=%d, position=%d, length=%d]"

                                               //  DDList
#define DDE_END_OF_LIST                "%s (%s@%d) End of list"
#define DDE_INVALID_LIST_DATA          "%s (%s@%d) Invalid data [offset=%d]"
#define DDE_LIST_OVERFLOW              "%s (%s@%d) List overflow [itemNumber=%d, itemMax=%d]"

                                               //  DynamicDataManager
#define DDE_CHECK_IN                   "%s (%s@%d) Value not allowed [id=%d, name=%s, value=%lld]"
#define DDE_CHECK_IN_FLOAT             "%s (%s@%d) Value not allowed [id=%d, name=%s, value=%E, bitsValue=0x%llx]"
#define DDE_CHECK_IN_STRING            "%s (%s@%d) Value not allowed [id=%d, name=%s, value=%s]"
#define DDE_CHECK_IN_STRUCT            "%s (%s@%d) Value not allowed [id=%d, name=%s]"
#define DDE_CHECK_MIN                  "%s (%s@%d) %s < min [id=%d, name=%s, value=%lld, min=%lld]"
#define DDE_CHECK_MAX                  "%s (%s@%d) %s > max [id=%d, name=%s, value=%lld, max=%lld]"
#define DDE_CHECK_MIN_FLOAT            "%s (%s@%d) Value < min [id=%d, name=%s, value=%E, bitsValue=0x%llx, min=%E, bitsMin=0x%llx]"
#define DDE_CHECK_MAX_FLOAT            "%s (%s@%d) Value > max [id=%d, name=%s, value=%E, bitsValue=0x%llx, max=%E, bitsMax=0x%llx]"
#define DDE_DATA_ALREADY_PRESENT       "%s (%s@%d) Data already present [id=%d]"
#define DDE_DATA_IS_NULL               "%s (%s@%d) Data is null [id=%d]"
#define DDE_DATA_NOT_FOUND             "%s (%s@%d) Data not found [id=%d]"
#define DDE_INVALID_DATA               "%s (%s@%d) Invalid data [id=%d]"
#define DDE_INVALID_DATA_TYPE          "%s (%s@%d) Invalid data type [id=%d, expected=%s, found=%s]"
#define DDE_INVALID_DECODE_ARGUMENT    "%s (%s@%d) Invalid decode argument [offset=%d, length=%d]"
#define DDE_INVALID_ENCODE_ARGUMENT    "%s (%s@%d) Invalid encode argument [reservedSizeBefore=%d, reservedSizeAfter=%d]"
#define DDE_INVALID_ID                 "%s (%s@%d) Invalid id [id=%d]"
#define DDE_INVALID_STRUCT_SIZE        "%s (%s@%d) Invalid %s size [id=%d, size=%d, expected=%lld]"
#define DDE_LIST_PROPERTIES_NOT_FOUND  "%s (%s@%d) List properties not found [id=%d]"
#define DDE_MISSING_DATA               "%s (%s@%d) Required data is missing [id=%d, name=%s]"
#define DDE_PROPERTIES_ALREADY_PRESENT "%s (%s@%d) Properties already present [id=%d]"
#define DDE_PROPERTIES_NOT_FOUND       "%s (%s@%d) Properties not found [id=%d]"

#define DDExceptionNoArgs(err,...) {throw QString::asprintf (err, __FUNCTION__, DDError::extractName (__FILE__), __LINE__);}
#define DDException(err,...)       {throw QString::asprintf (err, __FUNCTION__, DDError::extractName (__FILE__), __LINE__, __VA_ARGS__);}

class DDError
{
public:

    static const char *extractName (const char *name);
};

#endif    // DDERROR_H
