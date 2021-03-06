/*
 * Copyright (c) 2014, the Dart project authors.
 * 
 * Licensed under the Eclipse Public License v1.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 * 
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */

package com.google.dart.engine.internal.element.angular;

import com.google.dart.engine.element.ElementKind;
import com.google.dart.engine.element.ElementVisitor;
import com.google.dart.engine.element.angular.AngularComponentElement;
import com.google.dart.engine.element.angular.AngularPropertyElement;

/**
 * Implementation of {@code AngularComponentElement}.
 * 
 * @coverage dart.engine.element
 */
public class AngularComponentElementImpl extends AngularSelectorElementImpl implements
    AngularComponentElement {
  /**
   * The array containing all of the properties declared by this component.
   */
  private AngularPropertyElement[] properties;

  /**
   * The the CSS file URI.
   */
  private String styleUri;

  /**
   * The offset of the {@link #styleUri} in the {@link #getSource()}.
   */
  private int styleUriOffset;

  /**
   * The HTML template URI.
   */
  private String templateUri;

  /**
   * The offset of the {@link #templateUri} in the {@link #getSource()}.
   */
  private int templateUriOffset;

  /**
   * Initialize a newly created Angular component to have the given name.
   * 
   * @param name the name of this element
   * @param nameOffset the offset of the name of this element in the file that contains the
   *          declaration of this element
   */
  public AngularComponentElementImpl(String name, int nameOffset) {
    super(name, nameOffset);
  }

  @Override
  public <R> R accept(ElementVisitor<R> visitor) {
    return visitor.visitAngularComponentElement(this);
  }

  @Override
  public ElementKind getKind() {
    return ElementKind.ANGULAR_COMPONENT;
  }

  @Override
  public AngularPropertyElement[] getProperties() {
    return properties;
  }

  @Override
  public String getStyleUri() {
    return styleUri;
  }

  @Override
  public int getStyleUriOffset() {
    return styleUriOffset;
  }

  @Override
  public String getTemplateUri() {
    return templateUri;
  }

  @Override
  public int getTemplateUriOffset() {
    return templateUriOffset;
  }

  /**
   * Set an array containing all of the properties declared by this component.
   * 
   * @param properties the properties to set
   */
  public void setProperties(AngularPropertyElement[] properties) {
    for (AngularPropertyElement property : properties) {
      encloseElement((AngularPropertyElementImpl) property);
    }
    this.properties = properties;
  }

  /**
   * Set the CSS file URI.
   * 
   * @param styleUri the style URI to set
   */
  public void setStyleUri(String styleUri) {
    this.styleUri = styleUri;
  }

  /**
   * Set the offset of the {@link #getStyleUri()} in the {@link #getSource()}.
   * 
   * @param styleUriOffset the style URI offset to set
   */
  public void setStyleUriOffset(int styleUriOffset) {
    this.styleUriOffset = styleUriOffset;
  }

  /**
   * Set the HTML template URI.
   * 
   * @param templateUri the template URI to set
   */
  public void setTemplateUri(String templateUri) {
    this.templateUri = templateUri;
  }

  /**
   * Set the offset of the {@link #getTemplateUri()} in the {@link #getSource()}.
   * 
   * @param templateUriOffset the template URI offset to set
   */
  public void setTemplateUriOffset(int templateUriOffset) {
    this.templateUriOffset = templateUriOffset;
  }
}
