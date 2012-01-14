/*
 * Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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


#ifndef COFLO_EXCEPTIONS_HPP
#define COFLO_EXCEPTIONS_HPP

#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>

/**
 * Exception base class for all exceptions CoFlo itself will throw.
 */
struct coflo_exception_base: virtual std::exception, virtual boost::exception { };

/**
 * Exception to throw when some functionality is not yet implemented.
 */
struct not_implemented: virtual coflo_exception_base { };


#endif /* COFLO_EXCEPTIONS_HPP */
