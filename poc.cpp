#pragma leco tool

import coro;
import silog;

struct promise {
  int value;

  coro<promise> get_return_object() {
    return coro<promise>::from_promise(*this);
  }
  std::suspend_always initial_suspend() { return {}; }
  std::suspend_always final_suspend() noexcept { return {}; }
  std::suspend_always yield_value(int v) {
    value = v;
    return {};
  }
  void return_void() {}
  void unhandled_exception() {}
};

coro<promise> fibonacci(int n) {
  if (n <= 0)
    co_return;

  co_yield 1;
  if (n == 1)
    co_return;

  int a = 1;
  int b = 1;
  while (--n > 0) {
    co_yield a;
    int tmp = a;
    a = a + b;
    b = tmp;
  }
}

int main() {
  auto f = fibonacci(5);
  while (!f.done()) {
    f.resume();
    if (!f.done())
      silog::log(silog::info, "%d", f.promise().value);
  }
}
