// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include "lof.hpp"

#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include "../common/hash.hpp"
#include "../storage/lsh_util.hpp"
#include "../storage/lsh_vector.hpp"

using jubatus::core::anomaly::lof;
using jubatus::util::data::unordered_map;
using std::numeric_limits;
using std::string;
using std::vector;

#include <iostream>
using namespace std;


namespace jubatus {
namespace core {
namespace anomaly {

namespace {

float calculate_lof(
    float lrd,
    const unordered_map<string, float>& neighbor_lrd) {
  if (neighbor_lrd.empty()) {
    if (lrd == 0) {
      cout << "  => No neighbors, LRD is 0, score is 1" << endl;
      return 1;
    } else {
      cout << "  => No neighbors, LRD is " << lrd << ", score is INF!!" << endl;
      return numeric_limits<float>::infinity();
    }
  }

  float sum_neighbor_lrd = 0;
  for (unordered_map<string, float>::const_iterator it = neighbor_lrd.begin();
       it != neighbor_lrd.end(); ++it) {
    sum_neighbor_lrd += it->second;
  }

  cout << "  -> Num of Neighbors: " << neighbor_lrd.size() << endl;
  cout << "  -> Sum Neighbor LRD: " << sum_neighbor_lrd << endl;
  cout << "  -> Data Point LRD: " << lrd << endl;

  if (std::isinf(sum_neighbor_lrd) && std::isinf(lrd)) {
    cout << "  => Both INF!! returning 1" << endl;
    return 1;
  }

  return sum_neighbor_lrd / (neighbor_lrd.size() * lrd);
}

}  // namespace

lof::lof(
    const lof_storage::config& config,
    jubatus::util::lang::shared_ptr<recommender::recommender_base> nn_engine)
    : mixable_storage_(),
      nn_engine_(nn_engine) {

  if (!(2 <= config.nearest_neighbor_num)) {
    throw JUBATUS_EXCEPTION(
        common::invalid_parameter("2 <= nearest_neighbor_num"));
  }

  if (!(config.nearest_neighbor_num
      <= config.reverse_nearest_neighbor_num)) {
    throw JUBATUS_EXCEPTION(
        common::invalid_parameter(
            "nearest_neighbor_num <= reverse_nearest_neighbor_num"));
  }

  mixable_lof_storage::model_ptr p(new lof_storage(config, nn_engine));
  mixable_storage_.reset(new mixable_lof_storage(p));
}

lof::~lof() {
}

float lof::calc_anomaly_score(const common::sfv_t& query) const {
  cout << "=================================" << endl;
  cout << "CALC_ANOMALY_SCORE for SVF_T" << endl;
  unordered_map<string, float> neighbor_lrds;
  const float lrd = mixable_storage_->get_model()->collect_lrds(
      query, neighbor_lrds);
  cout << "LRD for Given Point: " << lrd << endl;
  cout << "Neighbor LRDs: " << endl;
  for (unordered_map<string, float>::iterator it = neighbor_lrds.begin(); it != neighbor_lrds.end(); ++it) {
    cout << "  " << it->first << " : " << it->second << endl;
  }
  float lof_value = calculate_lof(lrd, neighbor_lrds);

  cout << "LOF Score: " << lof_value << endl;
  cout << "=================================" << endl;

  return lof_value;
}

float lof::calc_anomaly_score(const string& id) const {
  cout << "=================================" << endl;
  cout << "CALC_ANOMALY_SCORE for ID " << id << endl;
  unordered_map<string, float> neighbor_lrds;
  const float lrd = mixable_storage_->get_model()->collect_lrds(
      id, neighbor_lrds);

  cout << "LRD for Given Point: " << lrd << endl;
  cout << "Neighbor LRDs: " << endl;
  for (unordered_map<string, float>::iterator it = neighbor_lrds.begin(); it != neighbor_lrds.end(); ++it) {
    cout << "  " << it->first << " : " << it->second << endl;
  }
  float lof_value = calculate_lof(lrd, neighbor_lrds);

  cout << "LOF Score: " << lof_value << endl;
  cout << "=================================" << endl;

  return lof_value;
}

void lof::clear() {
  mixable_storage_->get_model()->clear();
}

void lof::clear_row(const string& id) {
  mixable_storage_->get_model()->remove_row(id);
}

void lof::update_row(const string& id, const sfv_diff_t& diff) {
  cout << "=================================" << endl;
  cout << "UPDATE_ROW for ID " << id << endl;
  mixable_storage_->get_model()->update_row(id, diff);
  cout << "=================================" << endl;
}

bool lof::set_row(const string& id, const common::sfv_t& sfv) {
  mixable_storage_->get_model()->remove_row(id);
  mixable_storage_->get_model()->update_row(id, sfv);
  return true;
}

void lof::get_all_row_ids(vector<string>& ids) const {
  mixable_storage_->get_model()->get_all_row_ids(ids);
}

string lof::type() const {
  return "lof";
}

std::vector<framework::mixable*> lof::get_mixables() const {
  std::vector<framework::mixable*> mixables;
  mixables.push_back(mixable_storage_.get());
  mixables.push_back(nn_engine_->get_mixable());
  return mixables;
}

void lof::pack(framework::packer& packer) const {
  packer.pack_array(2);
  mixable_storage_->get_model()->pack(packer);
  nn_engine_->pack(packer);
}

void lof::unpack(msgpack::object o) {
  if (o.type != msgpack::type::ARRAY || o.via.array.size != 2) {
    throw msgpack::type_error();
  }

  // clear before load
  mixable_storage_->get_model()->clear();
  nn_engine_->clear();

  mixable_storage_->get_model()->unpack(o.via.array.ptr[0]);
  nn_engine_->unpack(o.via.array.ptr[1]);
}

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus
