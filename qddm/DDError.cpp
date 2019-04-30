//  #====================================#
//  # Dynamic Data Error - Version 1.0.0 #
//  #                                    #
//  # Author: Marco Vagnoni              #
//  # Date:   April     2019 (v1.0.0)    #
//  #====================================#

#include <cstring>
#include "DDError.h"

const char *DDError::extractName (const char *path)
{
    if (!path || !*path) return path;

    int  i;
    for (i = static_cast <int> (strlen (path)); i > 0 && path [i - 1] != '/' && path [i - 1] != '\\'; i--);
    return path + i;
}
