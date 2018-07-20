/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Java/JNI wrappers for PROJ API.
 * Author:   Martin Desruisseaux
 *
 ******************************************************************************
 * Copyright (c) 2011-2018, Open Geospatial Consortium, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

/**
 * Wrappers for the <a href="https://proj4.org/">PROJ</a> library.
 * The {@link org.proj4.PJ} class contains only native methods delegating their work to the PROJ library.
 * For higher-level methods making use of those native methods, see for example the
 * <a href="http://www.geoapi.org/geoapi-proj4/index.html">GeoAPI bindings for PROJ</a>.
 *
 * @author  Martin Desruisseaux (Geomatys)
 * @version 5.1
 * @since   4.8
 */
package org.proj4;
