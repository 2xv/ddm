//  #===================================#
//  # Dynamic Data Node - Version 1.0.0 #
//  #                                   #
//  # Author: Marco Vagnoni             #
//  # Email:  marco.vagnoni@yahoo.com   #
//  # Date:   April     2019 (v1.0.0)   #
//  #===================================#

#include "DDNode.h"

DDNode::DDNode (int offset, void *object, int type)
{
    this->offset = offset;
    this->object = object;
    this->type   = type;
    next         = nullptr;
}

int     DDNode::getOffset () {return offset;}
DDNode *DDNode::getNext   () {return next;}
void   *DDNode::getObject () {return object;}
int     DDNode::getType   () {return type;}

void    DDNode::setNext   (DDNode *next) {this->next = next;}
