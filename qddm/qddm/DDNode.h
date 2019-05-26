//  #===================================#
//  # Dynamic Data Node - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Email:  marco.vagnoni@yahoo.com   #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

#ifndef DDNODE_H
#define DDNODE_H

class DDNode
{
public:

    DDNode (int offset, void *object, int type);

    int     getOffset ();
    DDNode *getNext   ();
    void   *getObject ();
    int     getType   ();

    void    setNext   (DDNode *next);

private:

    DDNode *next;
    void   *object;
    int     type;
    int     offset;
};

#endif    // DDNODE_H
