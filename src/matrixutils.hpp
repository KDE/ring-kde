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

template<class EnumClass >
EnumIterator<EnumClass>::EnumIterator() {
   static_assert(std::is_enum<EnumClass>(),"The first template parameter has to be an enum class\n");
}

template<class EnumClass >
EnumClass EnumIterator<EnumClass>::EnumClassIter::operator* () const
{
   Q_ASSERT(pos_ < enum_class_size<EnumClass>());
   return static_cast<EnumClass>(pos_);
}

template<class EnumClass >
const typename EnumIterator<EnumClass>::EnumClassIter& EnumIterator<EnumClass>::EnumClassIter::operator++ ()
{
   ++pos_;
   return *this;
}

template<class EnumClass >
bool EnumIterator<EnumClass>::EnumClassIter::operator!= (const EnumClassIter& other) const
{
   return pos_ != other.pos_;
}

template< class EnumClass >
typename EnumIterator<EnumClass>::EnumClassIter EnumIterator<EnumClass>::begin()
{
   return EnumIterator<EnumClass>::EnumClassIter( this, 0 );
}

template<class EnumClass >
typename EnumIterator<EnumClass>::EnumClassIter EnumIterator<EnumClass>::end()
{
   return EnumIterator<EnumClass>::EnumClassIter( this, enum_class_size<EnumClass>() );
}





template<class Row, typename Value, typename Accessor>
Matrix1D<Row,Value,Accessor>::Matrix1D()
{
   m_lData.resize(enum_class_size<Row>());
}

template<class Row, typename Value, typename Accessor>
Matrix1D<Row,Value,Accessor>::Matrix1D(std::initializer_list< std::initializer_list<Value>> s)
: m_lData(*std::begin(s)) {
   static_assert(std::is_enum<Row>(),"Row has to be an enum class");
   static_assert(static_cast<int>(Row::COUNT__) > 0,"Row need a COUNT__ element");

   // FIXME C++14, use static_assert and make the ctor constexpr
   Q_ASSERT(std::begin(s)->size() == enum_class_size<Row>());//,"Matrix row have to match the enum class size");
}

template<class Row, typename Value, typename Accessor>
Value Matrix1D<Row,Value,Accessor>::operator[](Row v) {
   //ASSERT(size_t(v) >= size_t(Row::COUNT__),"State Machine Out of Bounds\n");
   if (size_t(v) >= enum_class_size<Row>() || static_cast<int>(v) < 0) {
      qWarning() << "State Machine Out of Bounds" << size_t(v);
      Q_ASSERT(false);
      throw v;
   }
   return m_lData[static_cast<int>(v)];
}

template<class Row, typename Value, typename Accessor>
const Value Matrix1D<Row,Value,Accessor>::operator[](Row v) const {
   Q_ASSERT(size_t(v) <= enum_class_size<Row>()+1 && size_t(v)>=0); //COUNT__ is also valid
   if (size_t(v) >= enum_class_size<Row>()) {
      qWarning() << "State Machine Out of Bounds" << size_t(v);
      Q_ASSERT(false);
      throw v;
   }
   return m_lData[static_cast<int>(v)];
}

template <class E, class T, class A> QMap<A,E> Matrix1D<E,T,A>::m_hReverseMapping;

template<class Row, typename Value, typename Accessor>
void Matrix1D<Row,Value,Accessor>::setReverseMapping(Matrix1D<Row,const char*> names)
{
   for ( const Row row : EnumIterator<Row>() )
      m_hReverseMapping[names[row]] = row;
}

template<class Row, typename Value, typename Accessor>
Row Matrix1D<Row,Value,Accessor>::fromValue(const Value& value) const {
    if (!m_hReverseMapping.empty()) {
         for (int i = 0; i < enum_class_size<Row>();i++) {
            const_cast<Matrix1D*>(this)->m_hReverseMapping[(*const_cast<Matrix1D*>(this))[(Row)i]]
               = static_cast<Row>(i);
         }
         Q_ASSERT(m_hReverseMapping.empty() == enum_class_size<Row>());
    }
    if (m_hReverseMapping.count(value) == 0) {
      throw value;
    }
    return m_hReverseMapping[value];
}

template<class Row, typename Value, typename Accessor>
bool Matrix1D<Row,Value,Accessor>::Matrix1DEnumClassIter::operator!= (const Matrix1DEnumClassIter& other) const
{
   return pos_ != other.pos_;
}

template<class Row, typename Value, typename Accessor>
bool Matrix1D<Row,Value,Accessor>::Matrix1DEnumClassIter::operator== (const Matrix1DEnumClassIter& other) const
{
   return pos_ == other.pos_;
}

template<class Row, typename Value, typename Accessor>
void Matrix1D<Row,Value,Accessor>::Matrix1DEnumClassIter::operator= (Value& other) const
{
   p_vec_->m_lData[pos_] = other;
}

template<class Row, typename Value, typename Accessor>
void Matrix1D<Row,Value,Accessor>::Matrix1DEnumClassIter::operator= (Value& other)
{
   p_vec_->m_lData[pos_] = other;
}

template<class Row, typename Value, typename Accessor>
void Matrix1D<Row,Value,Accessor>::setAt(Row row,Value value)
{
   m_lData[(int)row] = value;
}
