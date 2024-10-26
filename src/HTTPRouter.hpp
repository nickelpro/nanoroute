#ifndef NANOROUTE_HTTPROUTER_HPP
#define NANOROUTE_HTTPROUTER_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <queue>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Python.h"

#include "Util.hpp"

namespace nanoroute {

struct ParamQueue {
  std::vector<std::string_view>* pop();
  void push(std::vector<std::string_view>* ptr);

private:
  std::queue<std::vector<std::string_view>*> q;
};

extern thread_local ParamQueue params_q;

struct HTTPRouter {

  using param_vals = std::vector<std::string_view>*;
  using param_keys = std::vector<std::string>*;
  using val_ret = std::pair<PyObject*, std::pair<param_keys, param_vals>>;

  void reg_route(HTTPMethod meth, std::string_view route, PyObject* val);
  std::optional<val_ret> get_route(HTTPMethod meth, std::string_view route);

  void clear();

  template <std::invocable<PyObject*> F> void traverse(const F& f) {
    for(auto& node : routes_)
      node.traverse(f);
  }

  template <std::invocable<PyObject*> F,
      std::invocable<std::invoke_result_t<F, PyObject*>&> C>
  auto traverse(const F& f, const C& c) {
    for(auto& node : routes_) {
      auto result {node.traverse(f, c)};
      if(result)
        return result;
    }
    return std::optional<std::invoke_result_t<F, PyObject*>> {};
  }

private:
  struct Node {

    enum struct NodeType {
      STATIC,
      PARAM,
      CATCHALL,
    };

    std::string prefix_;
    std::string indices_;
    std::vector<Node> children_;
    PyObject* val_;
    std::uint32_t priority_;
    NodeType type_;
    bool wildchild_;
    std::vector<std::string> keys_;

    void clear();

    static void insert_new_branch(Node* node, std::string_view prefix,
        PyObject* val, std::vector<std::string> keys = {});

    std::size_t increment_child_prio(std::size_t offset);

    static void add_route(Node* node, std::string_view path, PyObject* val);

    static void split_child(Node* parent, std::size_t i);

    static std::optional<val_ret> get_value(Node* node, std::string_view route);

    template <std::invocable<PyObject*> F> void traverse(const F& f) {
      if(val_)
        std::invoke(f, val_);

      for(auto& child : children_)
        child.traverse(f);
    }

    template <std::invocable<PyObject*> F,
        std::invocable<std::invoke_result_t<F, PyObject*>&> C>
    auto traverse(const F& f, const C& c) {
      if(val_) {
        auto result {std::invoke(f, val_)};
        if(std::invoke(c, result))
          return std::optional {result};
      }

      for(auto& child : children_) {
        auto result {child.traverse(f, c)};
        if(result)
          return result;
      }

      return std::optional<std::invoke_result_t<F, PyObject*>> {};
    }
  };

  std::array<Node, static_cast<std::size_t>(HTTPMethod::MAX)> routes_;
};

} // namespace nanoroute

#endif // NANOROUTE_HTTPROUTER_HPP
