// Copyright (c) 2013, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

// Library tag to be able to run in html test framework.
library float32x4_two_arg_shuffle_test;

import "package:expect/expect.dart";
import 'dart:typed_data';

testWithZWInXY() {
  Float32x4 a = new Float32x4(1.0, 2.0, 3.0, 4.0);
  Float32x4 b = new Float32x4(5.0, 6.0, 7.0, 8.0);
  Float32x4 c = a.withZWInXY(b);
  Expect.equals(7.0, c.x);
  Expect.equals(8.0, c.y);
  Expect.equals(3.0, c.z);
  Expect.equals(4.0, c.w);
}

testInterleaveXY() {
  Float32x4 a = new Float32x4(1.0, 2.0, 3.0, 4.0);
  Float32x4 b = new Float32x4(5.0, 6.0, 7.0, 8.0);
  Float32x4 c = a.interleaveXY(b);
  Expect.equals(1.0, c.x);
  Expect.equals(5.0, c.y);
  Expect.equals(2.0, c.z);
  Expect.equals(6.0, c.w);
}

testInterleaveZW() {
  Float32x4 a = new Float32x4(1.0, 2.0, 3.0, 4.0);
  Float32x4 b = new Float32x4(5.0, 6.0, 7.0, 8.0);
  Float32x4 c = a.interleaveZW(b);
  Expect.equals(3.0, c.x);
  Expect.equals(7.0, c.y);
  Expect.equals(4.0, c.z);
  Expect.equals(8.0, c.w);
}

testInterleaveXYPairs() {
  Float32x4 a = new Float32x4(1.0, 2.0, 3.0, 4.0);
  Float32x4 b = new Float32x4(5.0, 6.0, 7.0, 8.0);
  Float32x4 c = a.interleaveXYPairs(b);
  Expect.equals(1.0, c.x);
  Expect.equals(2.0, c.y);
  Expect.equals(5.0, c.z);
  Expect.equals(6.0, c.w);
}

testInterleaveZWPairs() {
  Float32x4 a = new Float32x4(1.0, 2.0, 3.0, 4.0);
  Float32x4 b = new Float32x4(5.0, 6.0, 7.0, 8.0);
  Float32x4 c = a.interleaveZWPairs(b);
  Expect.equals(3.0, c.x);
  Expect.equals(4.0, c.y);
  Expect.equals(7.0, c.z);
  Expect.equals(8.0, c.w);
}

main() {
  for (int i = 0; i < 4000; i++) {
    testWithZWInXY();
    testInterleaveXY();
    testInterleaveZW();
    testInterleaveXYPairs();
    testInterleaveZWPairs();
  }
}