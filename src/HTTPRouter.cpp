#include "HTTPRouter.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Python.h"

#include "Util.hpp"

using std::operator""sv;

namespace {

std::size_t common_prefix(std::string_view a, std::string_view b) {
  std::size_t m {std::min(a.size(), b.size())};
  std::size_t i {0};
  while(i < m && a[i] == b[i])
    ++i;
  return i;
}

std::string_view right_slice(std::string_view sv, std::size_t idx) {
  return {sv.data() + idx, sv.size() - idx};
}

std::string_view left_slice(std::string_view sv, std::size_t idx) {
  return {sv.data(), idx};
}

std::string_view slice(std::string_view sv, std::size_t beg, std::size_t end) {
  return {sv.data() + beg, end - beg};
}

} // namespace

namespace nanoroute {

thread_local ParamQueue params_q;

std::vector<std::string_view>* ParamQueue::pop() {
  if(q.empty()) {
    auto ptr = new std::vector<std::string_view>;
    ptr->reserve(16);
    return ptr;
  }
  auto ptr = q.front();
  q.pop();
  return ptr;
}

void ParamQueue::push(std::vector<std::string_view>* ptr) {
  ptr->clear();
  q.push(ptr);
}

void HTTPRouter::reg_route(HTTPMethod meth, std::string_view route,
    PyObject* val) {
  Node* p {&routes_[static_cast<std::size_t>(meth)]};
  Node::add_route(p, route, std::move(val));
}

std::optional<HTTPRouter::val_ret> HTTPRouter::get_route(HTTPMethod meth,
    std::string_view route) {
  Node* p {&routes_[static_cast<std::size_t>(meth)]};
  return Node::get_value(p, route);
}

void HTTPRouter::clear() {
  for(auto& node : routes_)
    node.clear();
}

void HTTPRouter::Node::clear() {
  prefix_.clear();
  indices_.clear();
  children_.clear();
  val_ = nullptr;
  priority_ = 0;
  type_ = NodeType::STATIC;
  wildchild_ = false;
  keys_.clear();
}

void HTTPRouter::Node::insert_new_branch(Node* node, std::string_view prefix,
    PyObject* val, std::vector<std::string> keys) {
  while(!prefix.empty()) {

    if(prefix.front() == ':') {
      auto end {prefix.find_first_of(":*/"sv, 1)};

      if(end != prefix.npos) {
        if(prefix[end] != '/')
          throw std::runtime_error {"Invalid wildcard"};

        keys.emplace_back(slice(prefix, 1, end));
        prefix = right_slice(prefix, end);
      } else {
        keys.emplace_back(right_slice(prefix, 1));
        prefix = ""sv;
      }

      Node& child {node->children_.emplace_back()};
      child.priority_ = 1;
      child.type_ = NodeType::PARAM;
      node->wildchild_ = true;
      node = &child;

    } else if(prefix.front() == '*') {
      auto end {prefix.find_first_of(":*/"sv, 1)};

      if(end != prefix.npos)
        throw std::runtime_error {"Invalid wildcard"};

      keys.emplace_back(right_slice(prefix, 1));

      Node& child {node->children_.emplace_back()};
      child.priority_ = 1;
      child.type_ = NodeType::CATCHALL;
      node->wildchild_ = true;
      node = &child;
      break;

    } else {
      auto end {prefix.find_first_of(":*")};

      Node& child {node->children_.emplace_back()};
      child.priority_ = 1;

      if(end != prefix.npos) {
        child.prefix_ = left_slice(prefix, end);
        prefix = right_slice(prefix, end);
      } else {
        child.prefix_ = prefix;
        prefix = ""sv;
      }

      node->indices_ += child.prefix_[0];
      node = &child;
    }
  }

  node->val_ = val;
  node->keys_ = std::move(keys);
}

std::size_t HTTPRouter::Node::increment_child_prio(std::size_t offset) {
  auto prio {++children_[offset].priority_};
  auto cur {offset};

  for(; cur > 0 && children_[cur - 1].priority_ < prio; --cur)
    std::swap(children_[cur - 1], children_[cur]);

  if(cur != offset) {
    char c {indices_[offset]};
    char* start {indices_.data() + cur};
    std::memmove(start + 1, start, offset - cur);
    *start = c;
  }

  return cur;
}

void HTTPRouter::Node::add_route(Node* node, std::string_view path,
    PyObject* val) {
  ++node->priority_;

  if(node->prefix_.empty() && node->indices_.empty()) {
    auto end {path.find_first_of(":*")};
    if(end != path.npos) {
      node->prefix_ = left_slice(path, end);
      insert_new_branch(node, right_slice(path, end), val);
    } else {
      node->prefix_ = path;
      insert_new_branch(node, ""sv, val);
    }
    return;
  }

  std::vector<std::string> keys;
  for(;;) {
    if(node->type_ == NodeType::STATIC) {
      auto i {common_prefix(node->prefix_, path)};

      if(i < node->prefix_.size())
        split_child(node, i);

      if(i < path.size()) {
        path = right_slice(path, i);
        char idxc {path[0]};

        bool is_param {idxc == ':' || idxc == '*'};

        if(is_param) {
          if(node->wildchild_) {
            ++node->children_[0].priority_;
            node = &node->children_[0];
            continue;
          } else if(node->children_.empty()) {
            insert_new_branch(node, path, val, std::move(keys));
            return;
          }

          throw std::runtime_error {"Param conflicts with absolute path"};
        } // is_param

        if(node->wildchild_)
          throw std::runtime_error {"Absolute conflicts with param path"};

        if(auto j {node->indices_.find(idxc)}; j != node->indices_.npos) {
          j = node->increment_child_prio(j);
          node = &node->children_[j];
          continue;
        }

        insert_new_branch(node, path, val, std::move(keys));
        return;
      } // i < path.size()

      break;
    } else if(node->type_ == NodeType::PARAM) {
      if(path[0] == '*')
        throw std::runtime_error {"Catchall conflicts with existing param"};

      auto end {path.find_first_of(":*/", 1)};
      if(end == path.npos) {
        keys.emplace_back(right_slice(path, 1));
        break;
      }

      if(path[end] != '/')
        throw std::runtime_error {"Invalid wildcard"};

      keys.emplace_back(slice(path, 1, end));
      path = right_slice(path, end);

      if(!node->children_.empty()) {
        ++node->children_[0].priority_;
        node = &node->children_[0];
        continue;
      }

      insert_new_branch(node, path, val, std::move(keys));
      return;
    } else {
      throw std::runtime_error {"Path conflicts with existing catchall"};
    }
  } // for(;;)

  if(node->val_)
    throw std::runtime_error {"Handle already exists"};

  node->val_ = val;
  node->keys_ = std::move(keys);
}

void HTTPRouter::Node::split_child(Node* parent, std::size_t i) {
  char b {parent->prefix_[i]};
  Node child {
      std::string(right_slice(parent->prefix_, i)),
      std::move(parent->indices_),
      std::move(parent->children_),
      parent->val_,
      parent->priority_ - 1,
      NodeType::STATIC,
      parent->wildchild_,
      std::move(parent->keys_),
  };
  parent->prefix_.resize(i);
  parent->indices_.clear();
  parent->indices_ += b;
  parent->children_.clear();
  parent->children_.emplace_back(std::move(child));
  parent->val_ = nullptr;
  parent->wildchild_ = false;
  parent->keys_.clear();
}

std::optional<HTTPRouter::val_ret> HTTPRouter::Node::get_value(Node* node,
    std::string_view route) {
  std::vector<std::string_view>* params {params_q.pop()};
  for(;;) {
    if(node->type_ == NodeType::STATIC) {
      if(route.size() > node->prefix_.size()) {

        if(!route.starts_with(node->prefix_))
          return {};

        route = right_slice(route, node->prefix_.size());

        if(node->wildchild_) {
          node = &node->children_[0];
          continue;
        }

        auto idx {node->indices_.find(route[0])};
        if(idx == node->indices_.npos)
          return {};

        node = &node->children_[idx];
        continue;
      } // route.size() > node->prefix_.size()

      if(route == node->prefix_ && node->val_)
        return val_ret {node->val_, {&node->keys_, params}};

      return {};
    } else if(node->type_ == NodeType::PARAM) {
      if(auto end {route.find('/')}; end != route.npos) {
        params->push_back(left_slice(route, end));
        route = right_slice(route, end);
        node = &node->children_[0];
        continue;
      }
    }

    params->push_back(route);
    if(node->val_)
      return val_ret {node->val_, {&node->keys_, params}};

    return {};
  }
}

} // namespace nanoroute
