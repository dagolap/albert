// albert - a simple application launcher for linux
// Copyright (C) 2014 Manuel Schneider
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ABSTRACTINDEXPROVIDER_H
#define ABSTRACTINDEXPROVIDER_H

#include "abstractserviceprovider.h"
#include <string>
#include <list>
#include <locale>
#include <chrono>

class AbstractIndexProvider : public AbstractServiceProvider
{
public:

	class AbstractIndexItem : public AbstractServiceProvider::AbstractItem
	{
	public:
		AbstractIndexItem() = delete;
		AbstractIndexItem(const std::string &title) : AbstractItem(title){}
		virtual ~AbstractIndexItem(){}

		virtual std::string uri() const = 0;
		virtual std::chrono::system_clock::time_point lastAccess() const = 0;
//		virtual string      lastAccess() = 0;
	};

	class CaseInsensitiveCompare
	{
		std::locale _myLocale;
		std::ctype<char> const& myCType;
	public:
		CaseInsensitiveCompare( std::locale const& locale = std::locale( "" ) )
			: _myLocale( locale ), myCType( std::use_facet<std::ctype<char>>( locale ) ){}
		bool operator()( AbstractIndexItem const* lhs, AbstractIndexItem const* rhs ) const	{
			return (*this)( lhs->title(), rhs->title() );
		}
		bool operator()( std::string const& lhs, AbstractIndexItem const* rhs ) const	{
			return (*this)( lhs, rhs->title() );
		}
		bool operator()( AbstractIndexItem const* lhs, std::string const& rhs ) const	{
			return (*this)( lhs->title(), rhs );
		}
		bool operator()( std::string const& lhs, std::string const& rhs ) const	{
			return std::lexicographical_compare(
				lhs.begin(), lhs.end(),
				rhs.begin(), rhs.end(),
				*this);
		}
		bool operator()( char lhs, char rhs ) const	{
			return myCType.tolower(lhs) < myCType.tolower(rhs);
		}
	};
	class CaseInsensitiveComparePrefix
	{
		std::locale _myLocale;
		std::ctype<char> const& myCType;
	public:
		CaseInsensitiveComparePrefix( std::locale const& locale = std::locale( "" ) )
			: _myLocale( locale ), myCType( std::use_facet<std::ctype<char>>( locale ) ){}
		bool operator()( AbstractIndexItem const* pre, AbstractIndexItem const* rhs ) const	{
			return (*this)( pre->title(), rhs->title() );
		}
		bool operator()( std::string const& pre, AbstractIndexItem const* rhs ) const	{
			return (*this)( pre, rhs->title() );
		}
		bool operator()( AbstractIndexItem const* pre, std::string const& rhs ) const	{
			return (*this)( pre->title(), rhs );
		}
		bool operator()( std::string const& pre, std::string const& rhs ) const	{
			size_t m = std::min(pre.length(), rhs.length());
			return std::lexicographical_compare(
				pre.begin(), pre.begin()+m,
				rhs.begin(), rhs.begin()+m,
				*this);
		}
		bool operator()( char pre, char rhs ) const	{
			return myCType.tolower(pre) < myCType.tolower(rhs);
		}
	};



	AbstractIndexProvider(){}
	virtual ~AbstractIndexProvider(){}

	std::vector<AbstractServiceProvider::AbstractItem *> query(const std::string &req) override;
	virtual void buildIndex() = 0;

protected:
	std::vector<AbstractIndexItem*> _index;
	std::list<std::string> _watchPaths;
};

#endif // ABSTRACTINDEXPROVIDER_H