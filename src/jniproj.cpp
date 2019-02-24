/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Java/JNI wrappers for PROJ API.
 * Author:   Antonello Andrea
 *           Martin Desruisseaux
 *
 ******************************************************************************
 * Copyright (c) 2005, Andrea Antonello
 * Copyright (c) 2011, Martin Desruisseaux
 * Copyright (c) 2018, Even Rouault
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
 *****************************************************************************/

/*!
 * \file jniproj.c
 *
 * \brief
 * Functions used by the Java Native Interface (JNI) wrappers of PROJ.
 *
 *
 * \author Antonello Andrea
 * \date   Wed Oct 20 23:10:24 CEST 2004
 *
 * \author Martin Desruisseaux
 * \date   August 2011
 */

#include "proj_config.h"

#ifdef JNI_ENABLED

#include <math.h>
#include <string.h>
#include "proj_internal.h"
#include "org_proj4_PJ.h"
#include <jni.h>

#define PJ_FIELD_NAME "ptr"
#define PJ_FIELD_TYPE "J"

/*!
 * \brief
 * Internal method returning the address of the PJ structure wrapped by the given Java object.
 * This function looks for a field named "ptr" and of type "long" (Java signature "J") in the
 * given object.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The address of the PJ structure, or nullptr if the operation fails (for example
 *         because the "ptr" field was not found).
 */
static PJ *getPJ(JNIEnv *env, jobject object)
{
    jfieldID id = env->GetFieldID(env->GetObjectClass(object), PJ_FIELD_NAME, PJ_FIELD_TYPE);
    return (id) ? (PJ*) env->GetLongField(object, id) : nullptr;
}

/*!
 * \brief
 * Internal method returning the java.lang.Double.NaN constant value.
 * Efficiency is no a high concern for this particular method, because it
 * is used mostly when the user wrongly attempt to use a disposed PJ object.
 *
 * \param  env - The JNI environment.
 * \return The java.lang.Double.NaN constant value.
 */
static jdouble javaNaN(JNIEnv *env)
{
    jclass c = env->FindClass("java/lang/Double");
    if (c) { // Should never be nullptr, but let be paranoiac.
        jfieldID id = env->GetStaticFieldID(c, "NaN", "D");
        if (id) { // Should never be nullptr, but let be paranoiac.
            return env->GetStaticDoubleField(c, id);
        }
    }
    return 0.0; // Should never happen.
}

/*!
 * \brief
 * Returns the Proj4 release number.
 *
 * \param  env   - The JNI environment.
 * \return The Proj4 release number, or nullptr.
 */
JNIEXPORT jstring JNICALL Java_org_proj4_PJ_getVersion
  (JNIEnv *env, jclass)
{
    const char *desc = pj_get_release();
    return (desc) ? env->NewStringUTF(desc) : nullptr;
}

/*!
 * \brief
 * Allocates a new PJ structure from a definition string.
 *
 * \param  env        - The JNI environment.
 * \param  definition - The string definition to be given to Proj4.
 * \return The address of the new PJ structure, or 0 in case of failure.
 */
JNIEXPORT jlong JNICALL Java_org_proj4_PJ_allocatePJ
  (JNIEnv *env, jclass, jstring definition)
{
    const char *def_utf = env->GetStringUTFChars(definition, nullptr);
    if (!def_utf) return 0; /* OutOfMemoryError already thrown. */
    PJ *pj = pj_init_plus(def_utf);
    env->ReleaseStringUTFChars(definition, def_utf);
    return (jlong) pj;
}

/*!
 * \brief
 * Allocates a new geographic PJ structure from an existing one.
 *
 * \param  env       - The JNI environment.
 * \param  projected - The PJ object from which to derive a new one.
 * \return The address of the new PJ structure, or 0 in case of failure.
 */
JNIEXPORT jlong JNICALL Java_org_proj4_PJ_allocateGeoPJ
  (JNIEnv *env, jclass, jobject projected)
{
    PJ *pj = getPJ(env, projected);
    return (pj) ? (jlong) pj_latlong_from_proj(pj) : 0;
}

/*!
 * \brief
 * Returns the definition string.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The definition string.
 */
JNIEXPORT jstring JNICALL Java_org_proj4_PJ_getDefinition
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        char *desc = pj_get_def(pj, 0);
        if (desc) {
            jstring str = env->NewStringUTF(desc);
            pj_dalloc(desc);
            return str;
        }
    }
    return nullptr;
}

/*!
 * \brief
 * Returns the description associated to the PJ structure.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The description associated to the PJ structure.
 */
JNIEXPORT jstring JNICALL Java_org_proj4_PJ_toString
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        const char *desc = pj->descr;
        if (desc) {
            return env->NewStringUTF(desc);
        }
    }
    return nullptr;
}

/*!
 * \brief
 * Returns the CRS type as one of the PJ.Type enum: GEOGRAPHIC, GEOCENTRIC or PROJECTED.
 * This function should never return nullptr, unless class or fields have been renamed in
 * such a way that we can not find anymore the expected enum values.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The CRS type as one of the PJ.Type enum.
 */
JNIEXPORT jobject JNICALL Java_org_proj4_PJ_getType
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        const char *type;
        if (pj_is_latlong(pj)) {
            type = "GEOGRAPHIC";
        } else if (pj_is_geocent(pj)) {
            type = "GEOCENTRIC";
        } else {
            type = "PROJECTED";
        }
        jclass c = env->FindClass("org/proj4/PJ$Type");
        if (c) {
            jfieldID id = env->GetStaticFieldID(c, type, "Lorg/proj4/PJ$Type;");
            if (id) {
                return env->GetStaticObjectField(c, id);
            }
        }
    }
    return nullptr;
}

/*!
 * \brief
 * Returns the semi-major axis length.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The semi-major axis length.
 */
JNIEXPORT jdouble JNICALL Java_org_proj4_PJ_getSemiMajorAxis
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    return pj ? pj->a_orig : javaNaN(env);
}

/*!
 * \brief
 * Computes the semi-minor axis length from the semi-major axis length and the eccentricity
 * squared.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The semi-minor axis length.
 */
JNIEXPORT jdouble JNICALL Java_org_proj4_PJ_getSemiMinorAxis
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (!pj) return javaNaN(env);
    double a = pj->a_orig;
    return sqrt(a*a * (1.0 - pj->es_orig));
}

/*!
 * \brief
 * Returns the eccentricity squared.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The eccentricity.
 */
JNIEXPORT jdouble JNICALL Java_org_proj4_PJ_getEccentricitySquared
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    return pj ? pj->es_orig : javaNaN(env);
}

/*!
 * \brief
 * Returns an array of character indicating the direction of each axis.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The axis directions.
 */
JNIEXPORT jcharArray JNICALL Java_org_proj4_PJ_getAxisDirections
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        int length = static_cast<int>(strlen(pj->axis));
        jcharArray array = env->NewCharArray(length);
        if (array) {
            jchar* axis = env->GetCharArrayElements(array, nullptr);
            if (axis) {
                /* Don't use memcp because the type may not be the same. */
                int i;
                for (i=0; i<length; i++) {
                    axis[i] = pj->axis[i];
                }
                env->ReleaseCharArrayElements(array, axis, 0);
            }
            return array;
        }
    }
    return nullptr;
}

/*!
 * \brief
 * Longitude of the prime meridian measured from the Greenwich meridian, positive eastward.
 *
 * \param env    - The JNI environment.
 * \param object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The prime meridian longitude, in degrees.
 */
JNIEXPORT jdouble JNICALL Java_org_proj4_PJ_getGreenwichLongitude
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    return (pj) ? (pj->from_greenwich)*(180/M_PI) : javaNaN(env);
}

/*!
 * \brief
 * Returns the conversion factor from linear units to metres.
 *
 * \param env      - The JNI environment.
 * \param object   - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \param vertical - JNI_FALSE for horizontal axes, or JNI_TRUE for the vertical axis.
 * \return The conversion factor to metres.
 */
JNIEXPORT jdouble JNICALL Java_org_proj4_PJ_getLinearUnitToMetre
  (JNIEnv *env, jobject object, jboolean vertical)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        return (vertical) ? pj->vto_meter : pj->to_meter;
    }
    return javaNaN(env);
}

/*!
 * \brief
 * Converts input values from degrees to radians before coordinate operation, or the output
 * values from radians to degrees after the coordinate operation.
 *
 * \param pj        - The PROJ.4 PJ structure.
 * \param data      - The coordinate array to transform.
 * \param numPts    - Number of points to transform.
 * \param dimension - Dimension of points in the coordinate array.
 * \param factor    - The scale factor to apply: M_PI/180 for inputs or 180/M_PI for outputs.
 */
static void convertAngularOrdinates(PJ *pj, double* data, jint numPts, int dimension, double factor) {
    int dimToSkip;
    if (pj_is_latlong(pj)) {
        /* Convert only the 2 first ordinates and skip all the other dimensions. */
        dimToSkip = dimension - 2;
    } else {
        /* Not a geographic CRS: nothing to convert. */
        return;
    }
    double *stop = data + dimension*numPts;
    if (dimToSkip > 0) {
        while (data != stop) {
            (*data++) *= factor;
            (*data++) *= factor;
            data += dimToSkip;
        }
    } else {
        while (data != stop) {
            (*data++) *= factor;
        }
    }
}

/*!
 * \brief
 * Transforms in-place the coordinates in the given array.
 *
 * \param env         - The JNI environment.
 * \param object      - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \param target      - The target CRS.
 * \param dimension   - The dimension of each coordinate value. Must be equals or greater than 2.
 * \param coordinates - The coordinates to transform, as a sequence of (x,y,<z>,...) tuples.
 * \param offset      - Offset of the first coordinate in the given array.
 * \param numPts      - Number of points to transform.
 */
JNIEXPORT void JNICALL Java_org_proj4_PJ_transform
  (JNIEnv *env, jobject object, jobject target, jint dimension, jdoubleArray coordinates, jint offset, jint numPts)
{
    if (!target || !coordinates) {
        jclass c = env->FindClass("java/lang/NullPointerException");
        if (c) env->ThrowNew(c, "The target CRS and the coordinates array can not be null.");
        return;
    }
    if (dimension < 2 || dimension > org_proj4_PJ_DIMENSION_MAX) { /* Arbitrary upper value for catching potential misuse. */
        jclass c = env->FindClass("java/lang/IllegalArgumentException");
        if (c) env->ThrowNew(c, "Illegal number of dimensions.");
        return;
    }
    if ((offset < 0) || (numPts < 0) || (offset + dimension*numPts) > env->GetArrayLength(coordinates)) {
        jclass c = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (c) env->ThrowNew(c, "Illegal offset or illegal number of points.");
        return;
    }
    PJ *src_pj = getPJ(env, object);
    PJ *dst_pj = getPJ(env, target);
    if (src_pj && dst_pj) {
        /* Using GetPrimitiveArrayCritical/ReleasePrimitiveArrayCritical rather than
           GetDoubleArrayElements/ReleaseDoubleArrayElements increase the chances that
           the JVM returns direct reference to its internal array without copying data.
           However we must promise to run the "critical" code fast, to not make any
           system call that may wait for the JVM and to not invoke any other JNI method. */
        double *data = static_cast<double*>(env->GetPrimitiveArrayCritical(coordinates, nullptr));
        if (data) {
            double *x = data + offset;
            double *y = x + 1;
            double *z = (dimension >= 3) ? y+1 : nullptr;
            convertAngularOrdinates(src_pj, x, numPts, dimension, M_PI/180);
            int err = pj_transform(src_pj, dst_pj, numPts, dimension, x, y, z);
            convertAngularOrdinates(dst_pj, x, numPts, dimension, 180/M_PI);
            env->ReleasePrimitiveArrayCritical(coordinates, data, 0);
            if (err) {
                jclass c = env->FindClass("org/proj4/PJException");
                if (c) env->ThrowNew(c, pj_strerrno(err));
            }
        }
    }
}

/*!
 * \brief
 * Returns a description of the last error that occurred, or nullptr if none.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 * \return The last error, or nullptr.
 */
JNIEXPORT jstring JNICALL Java_org_proj4_PJ_getLastError
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        int err = pj_ctx_get_errno(pj->ctx);
        if (err) {
            return env->NewStringUTF(pj_strerrno(err));
        }
    }
    return nullptr;
}

/*!
 * \brief
 * Deallocate the PJ structure. This method is invoked by the garbage collector exactly once.
 * This method will also set the Java "ptr" final field to 0 as a safety. In theory we are not
 * supposed to change the value of a final field. But no Java code should use this field, and
 * the PJ object is being garbage collected anyway. We set the field to 0 as a safety in case
 * some user invoked the finalize() method explicitly despite our warning in the Javadoc to
 * never do such thing.
 *
 * \param env    - The JNI environment.
 * \param object - The Java object wrapping the PJ structure (not allowed to be nullptr).
 */
JNIEXPORT void JNICALL Java_org_proj4_PJ_finalize
  (JNIEnv *env, jobject object)
{
    jfieldID id = env->GetFieldID(env->GetObjectClass(object), PJ_FIELD_NAME, PJ_FIELD_TYPE);
    if (id) {
        PJ *pj = (PJ*) env->GetLongField(object, id);
        if (pj) {
            env->SetLongField(object, id, (jlong) 0);
            pj_free(pj);
        }
    }
}

#endif
