// Copyright (c) 2013, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

/**
 * Help for working with file format identifiers
 * such as `text/html` and `image/png`.
 *
 * More details, including a list of types, are in the Wikipedia article
 * [Internet media type](http://en.wikipedia.org/wiki/Internet_media_type).
 * For information on installing and importing this library, see the
 * [mime package on pub.dartlang.org]
 * (http://pub.dartlang.org/packages/mime).
 */
library mime;

import 'dart:async';
import 'dart:convert';
import 'dart:typed_data';

part 'src/mime_type.dart';
part 'src/extension_map.dart';
part 'src/magic_number.dart';
part 'src/mime_multipart_transformer.dart';
