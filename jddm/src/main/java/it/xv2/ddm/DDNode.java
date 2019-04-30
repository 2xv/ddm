//  #===================================#
//  # Dynamic Data Node - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

package it.xv2.ddm;

final class DDNode
{
    private DDNode       next;
    private final Object object;
    private final int    type;
    private final int    id;

    DDNode (int id, Object object, int type)
    {
        this.id     = id;
        this.object = object;
        this.type   = type;
    }

    int    getId     () {return id;}
    DDNode getNext   () {return next;}
    Object getObject () {return object;}
    int    getType   () {return type;}

    void setNext (DDNode next) {this.next = next;}
}