/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *                                                                                   *
 *  Technische Universität Berlin, Germany                                           *
 *  Audio Communication Group                                                        *
 *  www.ak.tu-berlin.de                                                              *
 *  Copyright 2006-2008                                                              *
 *                                                                                   *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or modify             *
 *  it under the terms of the GNU General Public License as published by             *
 *  the Free Software Foundation; either version 2 of the License, or                *
 *  (at your option) any later version.                                              *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
 *                                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cstring>
#include "liblo_extended.h"


int issame(lo_address a, lo_address b) {
    if(a == NULL || b == NULL) {
        return -1;    // at least one invalid address
    }

    if(strcmp(lo_address_get_hostname(a), lo_address_get_hostname(b)) == 0 &&
            strcmp(lo_address_get_port(a), lo_address_get_port(b)) == 0) {
        return 1;    // addresses are equal
    }

    return 0; // adresses are not equal

}


void forward(const char* path, const char* types, int argc, lo_arg** argv, lo_address to) {
    lo_message m = lo_message_new();

    for(int i = 0; i < argc; ++i) {
        if(types[ i ] == 's') {
            lo_message_add_string(m, &argv[ i ]->s);
        }

        else if(types[ i ] == 'f') {
            lo_message_add_float(m, argv[ i ]->f);
        }

        else if(types[ i ] == 'i') {
            lo_message_add_int32(m, argv[ i ]->i);
        }
    }

    lo_send_message(to, path, m);

    lo_message_free(m);
}


void forward(const char* types, int argc, lo_arg** argv, lo_address to) {
    lo_message m = lo_message_new();

    for(int i =  1; i < argc; ++i) {
        if(types[ i ] == 's') {
            lo_message_add_string(m, &argv[ i ]->s);
        }

        else if(types[ i ] == 'f') {
            lo_message_add_float(m, argv[ i ]->f);
        }

        else if(types[ i ] == 'i') {
            lo_message_add_int32(m, argv[ i ]->i);
        }
    }

    lo_send_message(to, &argv[ 0 ]->s, m);

    lo_message_free(m);
}
