/*ckwg +29
 * Copyright 2013-2014 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief algorithm regstrar class
 */

#ifndef MAPTK_REGISTRAR_H_
#define MAPTK_REGISTRAR_H_

#include <string>
#include <map>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include <maptk/config.h>
#include <maptk/logging_macros.h>


namespace maptk
{


/// Item map types for a given template type
#define DECL_ITEM_MAP(T) \
  /* map type for string-to-item relationship */ \
  typedef std::map< std::string, boost::shared_ptr<T> > item_map_t
#define DECL_ITEM_PAIR(T) \
  /* convenience type for item_map_t's values */ \
  typedef typename item_map_t::value_type item_pair_t
#define DECL_ITEM_ITR(T) \
  /* convenience type for item_map_t's const iterator */ \
  typedef typename item_map_t::const_iterator item_const_itr_t


/**
 * \brief A singleton class to keep a shared-pointer registry of objects across
 *        multiple types
 */
class MAPTK_LIB_EXPORT registrar
{
public:

  /// Access the singleton instance of this class
  static registrar& instance()
  {
    static registrar instance_ = registrar();
    return instance_;
  }

  /// Register a new name and item with the registrar for a given type
  template <typename T>
  bool register_item(const std::string& name, boost::shared_ptr<T> item)
  {
    DECL_ITEM_MAP(T);
    DECL_ITEM_PAIR(T);

    item_map_t &im = this->get_item_map<T>();

    bool new_insertion = im.insert(item_pair_t(name, item)).second;
    if (new_insertion)
    {
      LOG_DEBUG("registrar::register_item",
                "Registered \"" << name << "\" to instance. Item map now "
                "of size: " << this->get_item_map<T>().size());
    }
    else
    {
      LOG_WARN("registrar::register_item",
               "Warning: duplicate registration of \"" << name << "\"");
    }
    return new_insertion;
  }

  /// Return a vector of registered item names for a given type
  template <typename T>
  std::vector<std::string> registered_names()
  {
    DECL_ITEM_MAP(T);
    DECL_ITEM_PAIR(T);

    std::vector<std::string> names;
    BOOST_FOREACH(item_pair_t i, this->get_item_map<T>())
    {
      names.push_back(i.first);
    }
    return names;
  }

  /// Find the item matching \a name or return NULL
  template <typename T>
  boost::shared_ptr<T> find(const std::string& name)
  {
    DECL_ITEM_MAP(T);
    DECL_ITEM_ITR(T);

    item_map_t &im = this->get_item_map<T>();
    item_const_itr_t it = im.find(name);
    if (it == im.end())
    {
      return boost::shared_ptr<T>();
    }
    return it->second;
  }

  /// For an algorithm_def type, return the associated static item map.
  template <typename T>
  std::map< std::string, boost::shared_ptr<T> >& get_item_map()
  {
    static std::map< std::string, boost::shared_ptr<T> > ad_item_map_
      = std::map< std::string, boost::shared_ptr<T> >();
    return ad_item_map_;
  }

private:

  // TODO Move get_item_map() into private space


  /// Private constructor (this class is a singleton)
  registrar() {}
  /// Private destructor (this class is a singleton)
  ~registrar() {}
  /// Private copy constructor (this class is a singleton)
  registrar(const registrar&);
  /// Private assignment operator (this class is a singleton)
  registrar& operator=(const registrar&);
};


#undef DECL_MAP_TYPES


} // end namespace maptk


#endif // MAPTK_REGISTRAR_H_
