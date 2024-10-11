#ifndef NANOROUTE_HTTPROUTER_HPP
#define NANOROUTE_HTTPROUTER_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <absl/container/flat_hash_map.h>

namespace nanoroute {

using std::operator""sv;

enum struct HTTPMethod : std::int8_t {
  INVALID = -1,
#define HTTP_METHOD(code, name, sym) sym = code,
#include "http_method.def"
  MAX,
};

template <typename T> struct HTTPRouter {

  using CapturesType =
      std::vector<std::pair<std::string_view, std::string_view>>;

  std::optional<T> reg_route(HTTPMethod meth, std::string_view route, T val) {
    Node* p {&routes_[static_cast<std::size_t>(meth)]};
    std::vector<std::string> cap_names;
    for(auto seg : std::views::split(route, '/')) {
      if(seg.empty())
        continue;

      std::string_view sv {seg};
      if(sv.front() == '{' && sv.back() == '}') {
        sv = sv.substr(1, sv.size() - 2);

        if(!p->cap_)
          p->cap_ = std::make_unique<Node>();

        cap_names.emplace_back(sv);
        p = p->cap_.get();
        continue;
      }

      p = &p->abs_[sv];
    }

    if(p->val_) {
      std::swap(val, *p->val_);
      return val;
    }

    p->val_.emplace(std::move(val));
    p->cap_names_ = std::move(cap_names);
    return {};
  }

  std::optional<
      std::pair<T*, std::vector<std::pair<std::string_view, std::string_view>>>>
  get_route(HTTPMethod meth, std::string_view route) {
    std::vector<std::string_view> captures;
    Node* p {&routes_[static_cast<std::size_t>(meth)]};

    if(!p)
      return {};

    for(auto seg : std::views::split(route, '/')) {
      if(seg.empty())
        continue;

      std::string_view sv {seg};
      if(auto it {p->abs_.find(sv)}; it != p->abs_.end()) {
        p = &it->second;
        continue;
      }

      if(p->cap_) {
        captures.emplace_back(sv);
        p = p->cap_.get();
        continue;
      }

      return {};
    }

    if(!p->val_)
      return {};

    std::vector<std::pair<std::string_view, std::string_view>> cap_pairs;
    cap_pairs.reserve(captures.size());

    // We want to use views::enumerate, but it isn't supported in libc++ yet
    std::size_t i {0};
    for(auto& val : p->cap_names_) {
      if(val.empty())
        continue;
      cap_pairs.emplace_back(val, captures[i++]);
    }

    return std::pair {&(*p->val_), std::move(cap_pairs)};
  }

  void clear() {
    for(auto& node : routes_)
      node.clear();
  }

  template <std::invocable<T&> F> void traverse(const F& f) {
    for(auto& node : routes_)
      node.traverse(f);
  }

  template <std::invocable<T&> F, std::invocable<std::invoke_result_t<F, T>&> C>
  auto traverse(const F& f, const C& c) {
    for(auto& node : routes_) {
      auto result {node.traverse(f, c)};
      if(result)
        return result;
    }
    return std::optional<std::invoke_result_t<F, T>> {};
  }

private:
  struct Node {
    absl::flat_hash_map<std::string, Node> abs_;
    std::unique_ptr<Node> cap_;
    std::optional<T> val_;
    std::vector<std::string> cap_names_;

    void clear() {
      abs_.clear();
      cap_.reset(nullptr);
      val_.reset();
      cap_names_.clear();
    }

    template <std::invocable<T&> F> void traverse(const F& f) {
      if(val_)
        std::invoke(f, *val_);

      if(cap_)
        cap_->traverse(f);

      for(auto& [_, child] : abs_)
        child.traverse(f);
    }

    template <std::invocable<T&> F,
        std::invocable<std::invoke_result_t<F, T>&> C>
    auto traverse(const F& f, const C& c) {
      if(val_) {
        auto result {std::invoke(f, *val_)};
        if(std::invoke(c, result))
          return std::optional {result};
      }

      if(cap_) {
        auto result {cap_->traverse(f, c)};
        if(result)
          return result;
      }

      for(auto& [_, child] : abs_) {
        auto result {child.traverse(f, c)};
        if(result)
          return result;
      }

      return std::optional<std::invoke_result_t<F, T>> {};
    }
  };

  std::array<Node, static_cast<std::size_t>(HTTPMethod::MAX)> routes_;
};

} // namespace nanoroute

#endif // NANOROUTE_HTTPROUTER_HPP
