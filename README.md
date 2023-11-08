# coro

Basic C++ co-routine C++20 module.

It is composed of some clang-based STL classes - `coroutine_handle` etc - and a low-level `coro` class which can be used as a return type. `coro` is low-level, exposing handles' methods directly and can be used for more complex algos - generator, task, etc - if needed.

