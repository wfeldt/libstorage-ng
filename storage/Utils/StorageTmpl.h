/*
 * Copyright (c) [2004-2015] Novell, Inc.
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_STORAGE_TMPL_H
#define STORAGE_STORAGE_TMPL_H


#include <functional>
#include <ostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <set>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Logger.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    template<class Value>
    void operator>>(const string& d, Value& v)
    {
	std::istringstream Data(d);
	classic(Data);
	Data >> v;
    }


    template<class Value>
    std::ostream& operator<<(std::ostream& s, const std::list<Value>& l)
    {
	s << "<";
	for (typename std::list<Value>::const_iterator it = l.begin(); it != l.end(); ++it)
	{
	    if (it != l.begin())
		s << " ";
	    s << *it;
	}
	s << ">";
	return s;
    }


    template<class Value>
    std::ostream& operator<<(std::ostream& s, const std::deque<Value>& l)
    {
	s << "<";
	for (typename std::deque<Value>::const_iterator it = l.begin(); it != l.end(); ++it)
	{
	    if (it != l.begin())
		s << " ";
	    s << *it;
	}
	s << ">";
	return s;
    }


    template<class Value>
    std::ostream& operator<<(std::ostream& s, const std::vector<Value>& l)
    {
	s << "<";
	for (typename std::vector<Value>::const_iterator it = l.begin(); it != l.end(); ++it)
	{
	    if (it != l.begin())
		s << " ";
	    s << *it;
	}
	s << ">";
	return s;
    }


    template<class Value>
    std::ostream& operator<<(std::ostream& s, const std::set<Value>& l)
    {
	s << "<";
	for (typename std::set<Value>::const_iterator it = l.begin(); it != l.end(); ++it)
	{
	    if (it != l.begin())
		s << " ";
	    s << *it;
	}
	s << ">";
	return s;
    }


    template<class F, class S>
    std::ostream& operator<<(std::ostream& s, const std::pair<F, S>& p)
    {
	s << "[" << p.first << ":" << p.second << "]";
	return s;
    }


    template<class Key, class Value>
    std::ostream& operator<<(std::ostream& s, const std::map<Key, Value>& m)
    {
	s << "<";
	for (typename std::map<Key,Value>::const_iterator it = m.begin(); it != m.end(); ++it)
	{
	    if (it != m.begin())
		s << " ";
	    s << it->first << ":" << it->second;
	}
	s << ">";
	return s;
    }


    template <typename Type>
    void log_diff(std::ostream& log, const char* text, const Type& lhs, const Type& rhs)
    {
	static_assert(!std::is_enum<Type>::value, "is enum");

	if (lhs != rhs)
	    log << " " << text << ":" << lhs << "-->" << rhs;
    }


    template <typename Type>
    void log_diff_hex(std::ostream& log, const char* text, const Type& lhs, const Type& rhs)
    {
	static_assert(std::is_integral<Type>::value, "not integral");

	if (lhs != rhs)
	    log << " " << text << ":" << std::hex << lhs << "-->" << rhs << std::dec;
    }


    template <typename Type>
    void log_diff_enum(std::ostream& log, const char* text, const Type& lhs, const Type& rhs)
    {
	static_assert(std::is_enum<Type>::value, "not enum");

	if (lhs != rhs)
	    log << " " << text << ":" << toString(lhs) << "-->" << toString(rhs);
    }


    inline
    void log_diff(std::ostream& log, const char* text, bool lhs, bool rhs)
    {
	if (lhs != rhs)
	{
	    if (rhs)
		log << " -->" << text;
	    else
		log << " " << text << "-->";
	}
    }


    template<class Type>
    struct deref_less : public std::binary_function<const Type*, const Type*, bool>
    {
	bool operator()(const Type* x, const Type* y) const { return *x < *y; }
    };


    template<class Type>
    struct deref_equal_to : public std::binary_function<const Type*, const Type*, bool>
    {
	bool operator()(const Type* x, const Type* y) const { return *x == *y; }
    };


    template <class Type>
    void pointerIntoSortedList(list<Type*>& l, Type* e)
    {
	l.insert(lower_bound(l.begin(), l.end(), e, deref_less<Type>()), e);
    }


    template <class Type>
    void clearPointerList(list<Type*>& l)
    {
	for (typename list<Type*>::iterator i = l.begin(); i != l.end(); ++i)
	    delete *i;
	l.clear();
    }


    template <typename ListType, typename Type>
    bool contains(const ListType& l, const Type& value)
    {
	return find(l.begin(), l.end(), value) != l.end();
    }


    template <typename ListType, typename Predicate>
    bool contains_if(const ListType& l, Predicate pred)
    {
	return find_if(l.begin(), l.end(), pred) != l.end();
    }


    template<typename Map, typename Key, typename Value>
    typename Map::iterator mapInsertOrReplace(Map& m, const Key& k, const Value& v)
    {
	typename Map::iterator pos = m.lower_bound(k);
	if (pos != m.end() && !typename Map::key_compare()(k, pos->first))
	    pos->second = v;
	else
	    pos = m.insert(pos, typename Map::value_type(k, v));
	return pos;
    }


    template<typename List, typename Value>
    typename List::const_iterator addIfNotThere(List& l, const Value& v)
	{
	typename List::const_iterator pos = find( l.begin(), l.end(), v );
	if (pos == l.end() )
	    pos = l.insert(l.end(), v);
	return pos;
	}


    template<class Type>
    Type
    read_sysfs_property(const string& path)
    {
	Type ret;

	std::ifstream file(path);
	classic(file);
	file >> ret;
	file.close();

	if (file.fail())
	    ST_THROW(Exception(sformat("failed to read %s", path.c_str())));

	return ret;
    }


    template <class T, unsigned int sz>
    inline unsigned int lengthof(T (&)[sz]) { return sz; }

}

#endif
