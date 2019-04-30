//  #===================================#
//  # Dynamic Data Node - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

#include "DDNode.h"

DDNode::DDNode (int id, void *object, int type)
{
    this->id     = id;
    this->object = object;
    this->type   = type;
    next         = nullptr;
}

int     DDNode::getId     () {return id;}
DDNode *DDNode::getNext   () {return next;}
void   *DDNode::getObject () {return object;}
int     DDNode::getType   () {return type;}

void    DDNode::setNext   (DDNode *next) {this->next = next;}
