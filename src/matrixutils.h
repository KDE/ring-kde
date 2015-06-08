/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef MATRIXUTILS_H
#define MATRIXUTILS_H

//libSTDC++
#include <initializer_list>
#include <type_traits>

template<class T, class E>
struct TypedStateMachine
{
   // no ctor/dtor and one public member variable for easy initialization
   T _data[size_t(E::COUNT__)];

   T& operator[](E v) {
   if (size_t(v) >= size_t(E::COUNT__)) {
      Q_ASSERT(false);
      qDebug() << "State Machine Out of Bound" << size_t(v);
      throw v;
   }
   return _data[size_t(v)];
   }

   const T& operator[](E v) const {
   if (size_t(v) >= size_t(E::COUNT__)) {
      Q_ASSERT(false);
      qDebug() << "State Machine Out of Bound" << size_t(v);
      throw v;
   }
   return _data[size_t(v)];
   }

   T *begin() {
   return _data;
   }

   T *end() {
   return _data + size_t(E::COUNT__);
   }
};


/**
 * This generic class represents a multidimensional enum class array.
 * It safely converts them to integers. Each enum class needs a "COUNT__" item
 * at the end."
 *
 * This struct enforces:
 * * That the rows are indexed using enum_classes
 * * That the size of the matrix matches the enum_class size
 * * That the operators are within the matrix boundary
 */
template<class Row, typename Value, typename A = Value>
struct Matrix1D
{

   Matrix1D(std::initializer_list< std::initializer_list<Value> > s);
   explicit Matrix1D();

   // Row is a built-in type ("int" by default)
   Value operator[](Row v);

   const Value operator[](Row v) const;

   /**
   * An Iterator for enum classes
   */
   class Matrix1DEnumClassIter
   {
   public:
      Matrix1DEnumClassIter (Matrix1D<Row, Value, A>* p_vec, int pos)
         : pos_( pos ), p_vec_( p_vec ) {}

      bool operator!= (const Matrix1DEnumClassIter& other) const;
      bool operator== (const Matrix1DEnumClassIter& other) const;
      void operator=  (Value& other              )      ;
      void operator=  (Value& other              ) const;
      //Row operator* () const;
      //const Matrix1DEnumClassIter& operator++ ();

   private:
      int pos_;
      Matrix1D<Row, Value, A> *p_vec_;
   };

   //Iterators
   Matrix1DEnumClassIter begin();
   Matrix1DEnumClassIter end();

   // Only use for single reverse mappable arrays, will ASSERT otherwise
   Row fromValue(const Value& value) const;

   static void setReverseMapping(Matrix1D<Row,const char *> names);

   //Setter
   void setAt(Row,Value);

private:
   QVector<Value> m_lData;
   static QMap<A, Row> m_hReverseMapping;
};

/**
 * Create a matrix type with 2 enum class dimensions M[I,J] = V
 *                                                     ^ ^    ^
 *                                                     | |    |
 *                                          Rows    <--- |    |
 *                                          Columns <-----    |
 *                                          Value   <----------
 */
template<class Row, class Column, typename Value>
using Matrix2D = Matrix1D<Row, Matrix1D<Column, Value>>;


/**
 * A matrix with no value
 *
 * This is useful to use enum class in C++11 foreach loops
 *
 * @usage
 *   for (const MyEnum& value : EnumIterator<MyEnum>()) {
 *       std::cout << "Name: " << MyEnumNames[value] << std::endl;
 *   }
 */
template<class EnumClass>
struct EnumIterator
{
   /**
   * An Iterator for enum classes
   */
   class EnumClassIter
   {
   public:
      EnumClassIter (const EnumIterator<EnumClass>* p_vec, int pos)
         : pos_( pos ), p_vec_( p_vec ) {}

      bool operator!= (const EnumClassIter& other) const;
      EnumClass operator* () const;
      const EnumClassIter& operator++ ();

   private:
      int pos_;
      const EnumIterator<EnumClass> *p_vec_;
   };

   EnumIterator();

   //Iterators
   EnumClassIter begin();
   EnumClassIter end();
};

#include "matrixutils.hpp"

#endif
