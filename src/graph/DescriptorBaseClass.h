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

#ifndef DESCRIPTORBASECLASS_H_
#define DESCRIPTORBASECLASS_H_

#include <ostream>

// Forward declarations, needed for the overload of operator<<.
template <typename T>
class DescriptorBaseClass;
template <typename T>
std::ostream& operator<<(std::ostream& os, const DescriptorBaseClass<T>& desc);

/**
 * Base class for all descriptors.
 */
template <typename T>
class DescriptorBaseClass
{
public:
	/**
	 * Default constructor.
	 */
	DescriptorBaseClass() : m_ptr(NULL) {};
	/**
	 * Construct and initialize with the passed pointer.
	 * @param ptr
	 */
	explicit DescriptorBaseClass(T* ptr) : m_ptr(ptr) {};
	/**
	 * Copy constructor.
	 * @param other
	 */
	DescriptorBaseClass(const DescriptorBaseClass& other) : m_ptr(other.m_ptr) {};
	/**
	 * Assignment operator.
	 */
	DescriptorBaseClass& operator=(const DescriptorBaseClass& other) { m_ptr = other.m_ptr; return *this; };

	/// @name Member function templates for underlying type coercion support.
	/// This is an implementation of the Coercion by Member Template idiom.
	//@{

	/**
	 * Pseudo-copy constructor which allows implicit coercion.
	 *
	 * @note A U* must be dynamic_cast-able to T*, or this will throw an exception.
	 */
	template <typename U>
	DescriptorBaseClass (const DescriptorBaseClass<U>& other) : m_ptr(dynamic_cast<T*>(other.m_ptr)) {};

	/**
	 * Pseudo-assignment operator which allows implicit type coercion.
	 *
	 * @param other
	 * @return
	 */
	template <typename U>
	DescriptorBaseClass& operator=(const DescriptorBaseClass<U>& other) { m_ptr = dynamic_cast<T*>(other.m_ptr); return *this; };

	//@}

	/**
	 * Destructor.
	 */
	~DescriptorBaseClass() {};

	/**
	 * Returns a null descriptor of type DescriptorBaseClass<T>.
	 * @return
	 */
	static DescriptorBaseClass GetNullDescriptor();

	/**
	 * Dereference operator.  Returns a pointer to the payload of type T*.
	 *
	 * @return
	 */
	T* operator*() const { return m_ptr; };

	/**
	 * Dereference operator which returns a pointer to the payload of type U*.
	 * @note T* must be dynamic_cast-able to U*.
	 * @return
	 */
	template <typename U>
	U* operator*() const { return dynamic_cast<U*>(m_ptr); };

	/**
	 * Conversion function which returns the vertex_index.
	 * @param other
	 * @return
	 */
	operator std::size_t() const { return m_ptr->GetDescriptorIndex(); };

	/**
	 * Equality operator, required for EqualityComparable concept.
	 * @param other
	 * @return
	 */
	bool operator==(const DescriptorBaseClass& other) const { return m_ptr == other.m_ptr; };
	bool operator!=(const DescriptorBaseClass& other) const { return m_ptr != other.m_ptr; };

	/**
	 * Stream insertion operator.  Inserts @a desc into stream @a os.
	 *
	 * @param os Reference to the std::ostream to insert into.
	 * @param desc Reference to the descriptor to insert.
	 * @return Reference to @a os.
	 */
	friend std::ostream& operator<< <> (std::ostream& os, const DescriptorBaseClass<T>& desc);

	// Should be private, but the typename U templates above need to have access to it.
	T *m_ptr;

	/// The Null descriptor instance.
	static DescriptorBaseClass null_descriptor;
};

template <typename T> DescriptorBaseClass<T> DescriptorBaseClass<T>::null_descriptor;

template <typename T>
std::ostream& operator<<(std::ostream& os, const DescriptorBaseClass<T>& desc)
{
	// Stream out the address of the descriptor's payload.
    os << desc.m_ptr;
    return os;
}

#endif /* DESCRIPTORBASECLASS_H_ */
