// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2016 Preferred Networks and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_CORE_FV_CONVERTER_CHAR_SPLITTER_HPP_
#define JUBATUS_CORE_FV_CONVERTER_CHAR_SPLITTER_HPP_

#include <string>
#include <utility>
#include <vector>

#include "jubatus/util/data/string/ustring.h"

#include "word_splitter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

class char_splitter : public word_splitter {
 public:
  explicit char_splitter(const std::string& separator)
  : separator_(jubatus::util::data::string::string_to_ustring(separator)) {}

  void split(
      const std::string& string,
      std::vector<std::pair<size_t, size_t> >& ret_boundaries) const;

 private:
  const jubatus::util::data::string::ustring separator_;
};

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FV_CONVERTER_CHAR_SPLITTER_HPP_
