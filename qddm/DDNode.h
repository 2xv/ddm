//  #===================================#
//  # Dynamic Data Node - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

#ifndef DDNODE_H
#define DDNODE_H

class DDNode
{
public:

    DDNode (int id, void *object, int type);

    int     getId     ();
    DDNode *getNext   ();
    void   *getObject ();
    int     getType   ();

    void    setNext   (DDNode *next);

private:

    DDNode *next;
    void   *object;
    int     type;
    int     id;
};

#endif    // DDNODE_H
