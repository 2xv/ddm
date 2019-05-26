//  #=========================================#
//  # Dynamic Data Properties - Version 1.0.0 #
//  #                                         #
//  # Author: Marco Vagnoni                   #
//  # Email:  marco.vagnoni@yahoo.com         #
//  # Date:   April     2019 (v1.0.0)         #
//  #=========================================#

package it.xv2.ddm;

public final class DDProps
{
    public final String name;
    public final int    id;
    public final long   min;
    public final long   max;
    public final int    number;
    public final int    operType;
    public final Object def;
    public final Object in;

    public DDProps (String name, int id, long min, long max, int number, int operType, Object def, Object in)
    {
        this.name      = name;
        this.id        = id;
        this.min       = min;
        this.max       = max;
        this.number    = number;
        this.operType  = operType;
        this.def       = def;
        this.in        = in;
    }

    @Override
    public String toString ()
    {
        return "DDProps {name="   + name +
                        ", id="   + id +
                       ", min="   + min +
                       ", max="   + max +
                    ", number="   + number +
                  ", operType=0x" + Integer.toHexString (operType) +
                   ", default="   + def +
                        ", in="   + in + '}';
    }

    public static String toStringHeaderOnly () {return "\nDDProps {name, id, min, max, number, operType, default, in}";}

    public String toStringDataOnly ()
    {
        return "\n"   + name + ", " + id + ", " + min  + ", " + max + ", " + number +
               ", 0x" + Integer.toHexString (operType) + ", " + def + ", " + in;
    }
}