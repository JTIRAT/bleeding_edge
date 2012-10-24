#!/usr/bin/env dart
// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#library("utf16_tests");
#import("dunit.dart");
#import("../../string_encoding/unicode_core.dart");
#import("../../string_encoding/utf16.dart");

void main() {
  TestSuite suite = new TestSuite();
  suite.registerTestClass(new Utf16Tests());
  suite.run();
}

class Utf16Tests extends TestClass {
  static const String testKoreanCharSubset = """
가각갂갃간갅갆갇갈갉갊갋갌갍갎갏감갑값갓갔강갖갗갘같갚갛
개객갞갟갠갡갢갣갤갥갦갧갨갩갪갫갬갭갮갯갰갱갲갳갴갵갶갷
갸갹갺갻갼갽갾갿걀걁걂걃걄걅걆걇걈걉걊걋걌걍걎걏걐걑걒걓""";
  static const String testHanWater = "水";

  static const List<int> testKoreanCharSubsetUtf16beBom = const<int>[
      0xfe, 0xff, 0xac, 0x00, 0xac, 0x01, 0xac, 0x02,
      0xac, 0x03, 0xac, 0x04, 0xac, 0x05, 0xac, 0x06,
      0xac, 0x07, 0xac, 0x08, 0xac, 0x09, 0xac, 0x0a,
      0xac, 0x0b, 0xac, 0x0c, 0xac, 0x0d, 0xac, 0x0e,
      0xac, 0x0f, 0xac, 0x10, 0xac, 0x11, 0xac, 0x12,
      0xac, 0x13, 0xac, 0x14, 0xac, 0x15, 0xac, 0x16,
      0xac, 0x17, 0xac, 0x18, 0xac, 0x19, 0xac, 0x1a,
      0xac, 0x1b, 0x00, 0x0a, 0xac, 0x1c, 0xac, 0x1d,
      0xac, 0x1e, 0xac, 0x1f, 0xac, 0x20, 0xac, 0x21,
      0xac, 0x22, 0xac, 0x23, 0xac, 0x24, 0xac, 0x25,
      0xac, 0x26, 0xac, 0x27, 0xac, 0x28, 0xac, 0x29,
      0xac, 0x2a, 0xac, 0x2b, 0xac, 0x2c, 0xac, 0x2d,
      0xac, 0x2e, 0xac, 0x2f, 0xac, 0x30, 0xac, 0x31,
      0xac, 0x32, 0xac, 0x33, 0xac, 0x34, 0xac, 0x35,
      0xac, 0x36, 0xac, 0x37, 0x00, 0x0a, 0xac, 0x38,
      0xac, 0x39, 0xac, 0x3a, 0xac, 0x3b, 0xac, 0x3c,
      0xac, 0x3d, 0xac, 0x3e, 0xac, 0x3f, 0xac, 0x40,
      0xac, 0x41, 0xac, 0x42, 0xac, 0x43, 0xac, 0x44,
      0xac, 0x45, 0xac, 0x46, 0xac, 0x47, 0xac, 0x48,
      0xac, 0x49, 0xac, 0x4a, 0xac, 0x4b, 0xac, 0x4c,
      0xac, 0x4d, 0xac, 0x4e, 0xac, 0x4f, 0xac, 0x50,
      0xac, 0x51, 0xac, 0x52, 0xac, 0x53];

  static const List<int> testKoreanCharSubsetUtf16le = const<int>    [
      0x00, 0xac, 0x01, 0xac, 0x02, 0xac, 0x03, 0xac,
      0x04, 0xac, 0x05, 0xac, 0x06, 0xac, 0x07, 0xac,
      0x08, 0xac, 0x09, 0xac, 0x0a, 0xac, 0x0b, 0xac,
      0x0c, 0xac, 0x0d, 0xac, 0x0e, 0xac, 0x0f, 0xac,
      0x10, 0xac, 0x11, 0xac, 0x12, 0xac, 0x13, 0xac,
      0x14, 0xac, 0x15, 0xac, 0x16, 0xac, 0x17, 0xac,
      0x18, 0xac, 0x19, 0xac, 0x1a, 0xac, 0x1b, 0xac,
      0x0a, 0x00, 0x1c, 0xac, 0x1d, 0xac, 0x1e, 0xac,
      0x1f, 0xac, 0x20, 0xac, 0x21, 0xac, 0x22, 0xac,
      0x23, 0xac, 0x24, 0xac, 0x25, 0xac, 0x26, 0xac,
      0x27, 0xac, 0x28, 0xac, 0x29, 0xac, 0x2a, 0xac,
      0x2b, 0xac, 0x2c, 0xac, 0x2d, 0xac, 0x2e, 0xac,
      0x2f, 0xac, 0x30, 0xac, 0x31, 0xac, 0x32, 0xac,
      0x33, 0xac, 0x34, 0xac, 0x35, 0xac, 0x36, 0xac,
      0x37, 0xac, 0x0a, 0x00, 0x38, 0xac, 0x39, 0xac,
      0x3a, 0xac, 0x3b, 0xac, 0x3c, 0xac, 0x3d, 0xac,
      0x3e, 0xac, 0x3f, 0xac, 0x40, 0xac, 0x41, 0xac,
      0x42, 0xac, 0x43, 0xac, 0x44, 0xac, 0x45, 0xac,
      0x46, 0xac, 0x47, 0xac, 0x48, 0xac, 0x49, 0xac,
      0x4a, 0xac, 0x4b, 0xac, 0x4c, 0xac, 0x4d, 0xac,
      0x4e, 0xac, 0x4f, 0xac, 0x50, 0xac, 0x51, 0xac,
      0x52, 0xac, 0x53, 0xac];

  void registerTests(TestSuite suite) {
    register("Utf16Tests.testEncodeToUtf16", testEncodeToUtf16, suite);
    register("Utf16Tests.testUtf16BytesToString", testUtf16BytesToString, suite);
    register("Utf16Tests.testIterableMethods", testIterableMethods, suite);
  }

  void testEncodeToUtf16() {
    Expect.listEquals([], encodeUtf16be("")); // TODO(dcarlson) should we skip bom if empty?
    Expect.listEquals(testKoreanCharSubsetUtf16beBom,
        encodeUtf16(testKoreanCharSubset),
        "encode UTF-16(BE by default) Korean");

    Expect.listEquals(testKoreanCharSubsetUtf16le,
        encodeUtf16le(testKoreanCharSubset),
        "encode UTF-16LE Korean");
  }

  void testUtf16BytesToString() {
    Expect.stringEquals("", decodeUtf16([]));
    Expect.stringEquals(testHanWater, decodeUtf16([0x6C, 0x34]),
        "Water variation 1");
    Expect.stringEquals(testHanWater, decodeUtf16([0xFE, 0xFF, 0x6C, 0x34]),
        "Water variation 2");
    Expect.stringEquals(testHanWater, decodeUtf16([0xFF, 0xFE, 0x34, 0x6C]),
        "Water variation 3");

    Expect.stringEquals(testHanWater, decodeUtf16be([0x6C, 0x34]),
        "Water variation 4");
    Expect.stringEquals(testHanWater,
        decodeUtf16be([0xFE, 0xFF, 0x6C, 0x34]),
        "Water variation 5");

    Expect.stringEquals(testHanWater, decodeUtf16le([0x34, 0x6C]),
        "Water variation 6");
    Expect.stringEquals(testHanWater,
        decodeUtf16le([0xFF, 0xFE, 0x34, 0x6C]),
        "Water variation 7");

    Expect.stringEquals(testKoreanCharSubset,
        decodeUtf16(testKoreanCharSubsetUtf16beBom), "UTF-16BE Korean");
  }

  void testIterableMethods() {
    // empty input
    Expect.isFalse(decodeUtf16AsIterable([]).iterator().hasNext);

    IterableUtf16Decoder koreanDecoder =
      decodeUtf16AsIterable(testKoreanCharSubsetUtf16beBom);
    // get the first character
    Expect.equals(testKoreanCharSubset.charCodes()[0],
        koreanDecoder.iterator().next());
    // get the whole translation using the Iterable interface
    Expect.stringEquals(testKoreanCharSubset,
        new String.fromCharCodes(new List<int>.from(koreanDecoder)));

    // specify types
    Expect.equals(44032, (new List<int>
        .from(decodeUtf16beAsIterable(testKoreanCharSubsetUtf16beBom)))[0]);
    Expect.equals(44032, (new List<int>
        .from(decodeUtf16leAsIterable(testKoreanCharSubsetUtf16le)))[0]);
    Expect.equals(UNICODE_BOM, (new List<int>
        .from(decodeUtf16beAsIterable(testKoreanCharSubsetUtf16beBom,
              stripBom: false)))[0]);
  }
}
