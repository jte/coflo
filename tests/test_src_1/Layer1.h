/*
 * Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
 *
 * This file is part of CoFlo.
 *
 * CoFlo is free software: you can redistribute it and/or modify it under the
 * terms of version 3 of the GNU General Public License as published by the Free
 * Software Foundation.
 *
 * CoFlo is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * CoFlo.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file */


#ifndef LAYER1_H
#define LAYER1_H

/**
 * This function is threadsafe or not depending on the state of an extern volatile variable,
 * perhaps something set by an interrupt handler.
 *
 * @param string
 * @param integer
 */
void SometimesSafePrint(char *string, int integer);


#endif /* LAYER1_H */
