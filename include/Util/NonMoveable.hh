#pragma once

class NonMovable {
protected:
  NonMovable() = default;
  ~NonMovable() = default;

public:
  NonMovable(NonMovable &&) = delete;
  NonMovable &operator=(NonMovable &&) = delete;
};