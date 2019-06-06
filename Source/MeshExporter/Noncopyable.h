//  Boost Noncopyable.hpp header file  --------------------------------------//

//  (C) Copyright Boost.org 1999-2003. Permission to copy, use, modify, sell
//  and distribute this software is granted provided this copyright
//  notice appears in all copies. This software is provided "as is" without
//  express or implied warranty, and with no claim as to its suitability for
//  any purpose.

//  See http://www.boost.org/libs/utility for documentation.

#ifndef BOOST_Noncopyable_HPP_INCLUDED
#define BOOST_Noncopyable_HPP_INCLUDED

//  Private copy constructor and copy assignment ensure classes derived from
//  class Noncopyable cannot be copied.

//  Contributed by Dave Abrahams

class Noncopyable
{
 protected:
    Noncopyable() {}
    ~Noncopyable() {}
 private:  // emphasize the following members are private
    Noncopyable( const Noncopyable& );
    const Noncopyable& operator=( const Noncopyable& );
};



#endif  // BOOST_Noncopyable_HPP_INCLUDED
