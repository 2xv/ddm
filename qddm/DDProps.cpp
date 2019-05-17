//  #=========================================#
//  # Dynamic Data Properties - Version 1.0.0 #
//  #                                         #
//  # Author: Marco Vagnoni                   #
//  # Email:  marco.vagnoni@yahoo.com         #
//  # Date:   April     2019 (v1.0.0)         #
//  #=========================================#

#include "DDProps.h"

QString DDProps::toString ()
    {return QString::asprintf ("DDProps {name=%s, id=%d, min=%lld, max=%lld, number=%d, operType=0x%x, default=%p, in=%p}",
                                name, id, min, max, number, operType, def, in);}

QString DDProps::toStringDataOnly ()
    {return QString::asprintf ("\n%s, %d, %lld, %lld, %d, 0x%x, %p, %p", name, id, min, max, number, operType, def, in);}

QString DDProps::toStringHeaderOnly ()
    {return "\nDDProps {name, id, min, max, number, operType, default, in}";}
