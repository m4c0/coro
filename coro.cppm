export module coro;

export namespace std {
template <typename Tp, typename...> struct coroutine_traits {
  using promise_type = Tp::promise_type;
};
template <typename Tp = void> class coroutine_handle;

template <> class coroutine_handle<void> {
  void *addr{};

public:
  constexpr coroutine_handle() = default;
  static constexpr coroutine_handle from_address(void *addr) {
    coroutine_handle res{};
    res.addr = addr;
    return res;
  };

  explicit operator bool() const noexcept { return addr != nullptr; }

  constexpr void *address() const noexcept { return addr; }

  bool done() const { return __builtin_coro_done(addr); }
  void destroy() const { return __builtin_coro_destroy(addr); }
  void resume() const { return __builtin_coro_resume(addr); }
};

struct suspend_always {
  constexpr bool await_ready() const noexcept { return false; }
  constexpr void await_suspend(std::coroutine_handle<>) const noexcept {}
  constexpr void await_resume() const noexcept {}
};
struct suspend_never {
  constexpr bool await_ready() const noexcept { return true; }
  constexpr void await_suspend(std::coroutine_handle<>) const noexcept {}
  constexpr void await_resume() const noexcept {}
};

template <typename Tp> class coroutine_handle {
  void *addr{};

public:
  constexpr coroutine_handle() = default;

  static constexpr coroutine_handle from_address(void *addr) {
    coroutine_handle res{};
    res.addr = addr;
    return res;
  };
  static constexpr coroutine_handle from_promise(Tp &p) {
    coroutine_handle res{};
    res.addr = __builtin_coro_promise(&p, alignof(Tp), true);
    return res;
  };

  constexpr operator coroutine_handle<>() const noexcept {
    coroutine_handle<> res{};
    res.addr = addr;
    return res;
  }

  explicit operator bool() const noexcept { return addr != nullptr; }

  constexpr void *address() const noexcept { return addr; }

  bool done() const { return __builtin_coro_done(addr); }
  void destroy() const { return __builtin_coro_destroy(addr); }
  void resume() const { return __builtin_coro_resume(addr); }

  Tp &promise() const {
    return *static_cast<Tp *>(__builtin_coro_promise(addr, alignof(Tp), false));
  }
};
} // namespace std

namespace coro {
export template <typename P> class t {
  std::coroutine_handle<P> m_handle;

  t(std::coroutine_handle<P> h) : m_handle{h} {}

public:
  using handle_type = std::coroutine_handle<P>;
  using promise_type = P;

  ~t() { m_handle.destroy(); }

  [[nodiscard]] auto done() const noexcept { return m_handle.done(); }
  [[nodiscard]] auto resume() const noexcept { return m_handle.resume(); }
  [[nodiscard]] auto promise() const noexcept { return m_handle.promise(); }

  [[nodiscard]] static t<P> from_promise(P &p) {
    return handle_type::from_promise(p);
  }
};
} // namespace coro

namespace coro::promises {
export template <typename Tp> struct basic_valued {
  using coro = coro::t<basic_valued<Tp>>;

  Tp value;

  coro get_return_object() { return coro::from_promise(*this); }
  std::suspend_always initial_suspend() { return {}; }
  std::suspend_always final_suspend() noexcept { return {}; }
  std::suspend_always yield_value(Tp v) {
    value = v;
    return {};
  }
  void return_void() {}
  void unhandled_exception() {}
};
} // namespace coro::promises
