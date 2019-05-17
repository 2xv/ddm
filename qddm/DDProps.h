//  #=========================================#
//  # Dynamic Data Properties - Version 1.0.0 #
//  #                                         #
//  # Author: Marco Vagnoni                   #
//  # Email:  marco.vagnoni@yahoo.com         #
//  # Date:   April     2019 (v1.0.0)         #
//  #=========================================#

#ifndef DDPROPS_H
#define DDPROPS_H

#include <QtGlobal>
#include <QString>

struct DDProps
{
    const char    *name;
    int            id;
    qint64         min;
    qint64         max;
    int            number;
    int            operType;
    void          *def;
    void          *in;

    QString        toString           ();
    QString        toStringDataOnly   ();
    static QString toStringHeaderOnly ();
};

#endif    // DDPROPS_H
