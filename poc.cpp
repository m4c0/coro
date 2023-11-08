#pragma leco tool

import coro;
import silog;

coro::promises::basic_valued<int>::coro fibonacci(int n) {
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
