// Copyright (c) 2013, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#ifndef BIN_STDIO_H_
#define BIN_STDIO_H_

#include "bin/builtin.h"
#include "bin/utils.h"

#include "platform/globals.h"


namespace dart {
namespace bin {

class Stdin {
 public:
  static int ReadByte();

  static bool GetEchoMode();
  static void SetEchoMode(bool enabled);

  static bool GetLineMode();
  static void SetLineMode(bool enabled);

 private:
  DISALLOW_ALLOCATION();
  DISALLOW_IMPLICIT_CONSTRUCTORS(Stdin);
};


class Stdout {
 public:
  static bool GetTerminalSize(int size[2]);

 private:
  DISALLOW_ALLOCATION();
  DISALLOW_IMPLICIT_CONSTRUCTORS(Stdout);
};

}  // namespace bin
}  // namespace dart

#endif  // BIN_STDIO_H_

