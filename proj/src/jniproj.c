/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Java/JNI wrappers for PROJ.4 API.
 * Author:   Antonello Andrea
 *           Martin Desruisseaux
 *
 ******************************************************************************
 * Copyright (c) 2005, Antonello Andrea
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
 * Functions used by the Java Native Interface (JNI) wrappers of Proj.4
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
#include "projects.h"
#include "org_proj4_PJ.h"
#include <jni.h>

#define PJ_FIELD_NAME "ptr"
#define PJ_FIELD_TYPE "J"
#define PJ_MAX_DIMENSION 100
/* The PJ_MAX_DIMENSION value appears also in quoted strings.
   Please perform a search-and-replace if this value is changed. */

PJ_CVSID("$Id$");


/*!
 * \brief
 * Internal method returning the address of the PJ structure wrapped by the given Java object.
 * This function looks for a field named "ptr" and of type "long" (Java signature "J") in the
 * given object.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be NULL).
 * \return The address of the PJ structure, or NULL if the operation fails (for example
 *         because the "ptr" field was not found).
 */
PJ *getPJ(JNIEnv *env, jobject object)
{
    jfieldID id = (*env)->GetFieldID(env, (*env)->GetObjectClass(env, object), PJ_FIELD_NAME, PJ_FIELD_TYPE);
    return (id) ? (PJ*) (*env)->GetLongField(env, object, id) : NULL;
}

/*!
 * \brief
 * Returns the Proj4 release number.
 *
 * \param  env   - The JNI environment.
 * \param  class - The class from which this method has been invoked.
 * \return The Proj4 release number, or NULL.
 */
JNIEXPORT jstring JNICALL Java_org_proj4_PJ_getVersion
  (JNIEnv *env, jclass class)
{
    const char *desc = pj_get_release();
    return (desc) ? (*env)->NewStringUTF(env, desc) : NULL;
}

/*!
 * \brief
 * Allocates a new PJ structure from a definition string.
 *
 * \param  env        - The JNI environment.
 * \param  class      - The class from which this method has been invoked.
 * \param  definition - The string definition to be given to Proj4.
 * \return The address of the new PJ structure, or 0 in case of failure.
 */
JNIEXPORT jlong JNICALL Java_org_proj4_PJ_allocatePJ
  (JNIEnv *env, jclass class, jstring definition)
{
    const char *def_utf = (*env)->GetStringUTFChars(env, definition, NULL);
    if (!def_utf) return 0; /* OutOfMemoryError already thrown. */
    PJ *pj = pj_init_plus(def_utf);
    (*env)->ReleaseStringUTFChars(env, definition, def_utf);
    return (jlong) pj;
}

/*!
 * \brief
 * Allocates a new geographic PJ structure from an existing one.
 *
 * \param  env       - The JNI environment.
 * \param  class     - The class from which this method has been invoked.
 * \param  projected - The PJ object from which to derive a new one.
 * \return The address of the new PJ structure, or 0 in case of failure.
 */
JNIEXPORT jlong JNICALL Java_org_proj4_PJ_allocateGeoPJ
  (JNIEnv *env, jclass class, jobject projected)
{
    PJ *pj = getPJ(env, projected);
    return (pj) ? (jlong) pj_latlong_from_proj(pj) : 0;
}

/*!
 * \brief
 * Returns the definition string.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be NULL).
 * \return The definition string.
 */
JNIEXPORT jstring JNICALL Java_org_proj4_PJ_getDefinition
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        char *desc = pj_get_def(pj, 0);
        if (desc) {
            jstring str = (*env)->NewStringUTF(env, desc);
            pj_dalloc(desc);
            return str;
        }
    }
    return NULL;
}

/*!
 * \brief
 * Returns the description associated to the PJ structure.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be NULL).
 * \return The description associated to the PJ structure.
 */
JNIEXPORT jstring JNICALL Java_org_proj4_PJ_toString
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        const char *desc = pj->descr;
        if (desc) {
            return (*env)->NewStringUTF(env, desc);
        }
    }
    return NULL;
}

/*!
 * \brief
 * Returns the CRS type as one of the PJ.Type enum: GEOGRAPHIC, GEOCENTRIC or PROJECTED.
 * This function should never return NULL, unless class or fields have been renamed in
 * such a way that we can not find anymore the expected enum values.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be NULL).
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
        jclass c = (*env)->FindClass(env, "org/proj4/PJ$Type");
        if (c) {
            jfieldID id = (*env)->GetStaticFieldID(env, c, type, "Lorg/proj4/PJ$Type;");
            if (id) {
                return (*env)->GetStaticObjectField(env, c, id);
            }
        }
    }
    return NULL;
}

/*!
 * \brief
 * Returns the semi-major axis length.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be NULL).
 * \return The semi-major axis length.
 */
JNIEXPORT jdouble JNICALL Java_org_proj4_PJ_getSemiMajorAxis
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    return pj ? pj->a_orig : NAN;
}

/*!
 * \brief
 * Computes the semi-minor axis length from the semi-major axis length and the eccentricity
 * squared.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be NULL).
 * \return The semi-minor axis length.
 */
JNIEXPORT jdouble JNICALL Java_org_proj4_PJ_getSemiMinorAxis
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (!pj) return NAN;
    double a = pj->a_orig;
    return sqrt(a*a * (1.0 - pj->es_orig));
}

/*!
 * \brief
 * Returns the eccentricity squared.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be NULL).
 * \return The eccentricity.
 */
JNIEXPORT jdouble JNICALL Java_org_proj4_PJ_getEccentricitySquared
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    return pj ? pj->es_orig : NAN;
}

/*!
 * \brief
 * Returns an array of character indicating the direction of each axis.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be NULL).
 * \return The axis directions.
 */
JNIEXPORT jcharArray JNICALL Java_org_proj4_PJ_getAxisDirections
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        int length = strlen(pj->axis);
        jcharArray array = (*env)->NewCharArray(env, length);
        if (array) {
            jchar* axis = (*env)->GetCharArrayElements(env, array, NULL);
            if (axis) {
                /* Don't use memcp because the type may not be the same. */
                int i;
                for (i=0; i<length; i++) {
                    axis[i] = pj->axis[i];
                }
                (*env)->ReleaseCharArrayElements(env, array, axis, 0);
            }
            return array;
        }
    }
    return NULL;
}

/*!
 * \brief
 * Longitude of the prime meridian measured from the Greenwich meridian, positive eastward.
 *
 * \param env    - The JNI environment.
 * \param object - The Java object wrapping the PJ structure (not allowed to be NULL).
 * \return The prime meridian longitude, in degrees.
 */
JNIEXPORT jdouble JNICALL Java_org_proj4_PJ_getGreenwichLongitude
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    return (pj) ? (pj->from_greenwich)*(180/M_PI) : NAN;
}

/*!
 * \brief
 * Returns the conversion factor from linear units to metres.
 *
 * \param env      - The JNI environment.
 * \param object   - The Java object wrapping the PJ structure (not allowed to be NULL).
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
    return NAN;
}

/*!
 * \brief
 * Converts input values from degrees to radians before coordinate operation, or the output
 * values from radians to degrees after the coordinate operation.
 *
 * \param pj        - The Proj.4 PJ structure.
 * \param data      - The coordinate array to transform.
 * \param numPts    - Number of points to transform.
 * \param dimension - Dimension of points in the coordinate array.
 * \param factor    - The scale factor to apply: M_PI/180 for inputs or 180/M_PI for outputs.
 */
void convertAngularOrdinates(PJ *pj, double* data, jint numPts, int dimension, double factor) {
    int dimToSkip;
    if (pj_is_latlong(pj)) {
        /* Convert only the 2 first ordinates and skip all the other dimensions. */
        dimToSkip = dimension - 2;
    } else if (pj_is_geocent(pj)) {
        /* Convert only the 3 first ordinates and skip all the other dimensions. */
        dimToSkip = dimension - 3;
    } else {
        /* Not a geographic or geocentric CRS: nothing to convert. */
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
 * \param object      - The Java object wrapping the PJ structure (not allowed to be NULL).
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
        jclass c = (*env)->FindClass(env, "java/lang/NullPointerException");
        if (c) (*env)->ThrowNew(env, c, "The target CRS and the coordinates array can not be null.");
        return;
    }
    if (dimension < 2 || dimension > PJ_MAX_DIMENSION) { /* Arbitrary upper value for catching potential misuse. */
        jclass c = (*env)->FindClass(env, "java/lang/IllegalArgumentException");
        if (c) (*env)->ThrowNew(env, c, "Illegal dimension. Must be in the [2-100] range.");
        return;
    }
    if ((offset < 0) || (numPts < 0) || (offset + dimension*numPts) > (*env)->GetArrayLength(env, coordinates)) {
        jclass c = (*env)->FindClass(env, "java/lang/ArrayIndexOutOfBoundsException");
        if (c) (*env)->ThrowNew(env, c, "Illegal offset or illegal number of points.");
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
        double *data = (*env)->GetPrimitiveArrayCritical(env, coordinates, NULL);
        if (data) {
            double *x = data + offset;
            double *y = x + 1;
            double *z = (dimension >= 3) ? y+1 : NULL;
            convertAngularOrdinates(src_pj, x, numPts, dimension, M_PI/180);
            int err = pj_transform(src_pj, dst_pj, numPts, dimension, x, y, z);
            convertAngularOrdinates(dst_pj, x, numPts, dimension, 180/M_PI);
            (*env)->ReleasePrimitiveArrayCritical(env, coordinates, data, 0);
            if (err) {
                jclass c = (*env)->FindClass(env, "org/proj4/PJException");
                if (c) (*env)->ThrowNew(env, c, pj_strerrno(err));
            }
        }
    }
}

/*!
 * \brief
 * Returns a description of the last error that occurred, or NULL if none.
 *
 * \param  env    - The JNI environment.
 * \param  object - The Java object wrapping the PJ structure (not allowed to be NULL).
 * \return The last error, or NULL.
 */
JNIEXPORT jstring JNICALL Java_org_proj4_PJ_getLastError
  (JNIEnv *env, jobject object)
{
    PJ *pj = getPJ(env, object);
    if (pj) {
        int err = pj_ctx_get_errno(pj->ctx);
        if (err) {
            return (*env)->NewStringUTF(env, pj_strerrno(err));
        }
    }
    return NULL;
}

/*!
 * \brief
 * Deallocate the PJ structure. This method is invoked by the garbage collector exactly once.
 * This method will also set the Java "ptr" final field to 0 as a safety. In theory we are not
 * supposed to change the value of a final field. But no Java code should use this field, and
 * the PJ object is being garbage collected anyway. We set the field to 0 as a safety in case
 * some user invoked the finalize() method explicitely despite our warning in the Javadoc to
 * never do such thing.
 *
 * \param env    - The JNI environment.
 * \param object - The Java object wrapping the PJ structure (not allowed to be NULL).
 */
JNIEXPORT void JNICALL Java_org_proj4_PJ_finalize
  (JNIEnv *env, jobject object)
{
    jfieldID id = (*env)->GetFieldID(env, (*env)->GetObjectClass(env, object), PJ_FIELD_NAME, PJ_FIELD_TYPE);
    if (id) {
        PJ *pj = (PJ*) (*env)->GetLongField(env, object, id);
        if (pj) {
            (*env)->SetLongField(env, object, id, (jlong) 0);
            pj_free(pj);
        }
    }
}




/* ===============================================================================================
 *
 * Below this point are the previous JNI bindings that existed in the legacy org.proj4.Projections
 * class before the new bindings defined above. We keep those binding for now, but may remove them
 * in a future version. There is a few issues with the code below:
 *
 *  1) Every call to (*env)->GetStringUTFChars(...) shall have a corresponding call to
 *     (*env)->ReleaseStringUTFChars(...).
 *
 *  2) Every call to (*env)->GetFoo(...) shall check for NULL return value.  If the return
 *     value is NULL, than a java.lang.OutOfMemoryError has already been thrown in the JVM;
 *     we just need to return from the C method after releasing allocated objects (if any).
 *
 *  3) If a Proj.4 method fails to execute, we should invoke (*env)->ThrowNew(...) instead
 *     than exit(1) in order to throw an exception in the Java program instead than stopping
 *     the JVM.
 *
 *  4) We should check the user arguments for null values or index out of bounds, and
 *     throw the appropriate Java exception if an argument is invalid.
 */

#include "org_proj4_Projections.h"

#define arraysize 300

/*!
 * \brief
 * executes reprojection
 *
 * JNI informations:
 * Class:     org_proj4_Projections
 * Method:    transform
 * Signature: ([D[D[DLjava/lang/String;Ljava/lang/String;JI)V
 *
 *
 * \param env - parameter used by jni (see JNI specification)
 * \param parent - parameter used by jni (see JNI specification)
 * \param firstcoord - array of x coordinates
 * \param secondcoord - array of y coordinates
 * \param values - array of z coordinates
 * \param src - definition of the source projection
 * \param dest - definition of the destination projection
 * \param pcount
 * \param poffset
*/
JNIEXPORT void JNICALL Java_org_proj4_Projections_transform
  (JNIEnv * env, jobject parent, jdoubleArray firstcoord, jdoubleArray secondcoord, jdoubleArray values, jstring src, jstring dest, jlong pcount, jint poffset)
{
	int i;
	projPJ src_pj, dst_pj;
	char * srcproj_def = (char *) (*env)->GetStringUTFChars (env, src, 0);
	char * destproj_def = (char *) (*env)->GetStringUTFChars (env, dest, 0);

	if (!(src_pj = pj_init_plus(srcproj_def)))
		exit(1);
	if (!(dst_pj = pj_init_plus(destproj_def)))
		exit(1);

	double *xcoord = (* env)-> GetDoubleArrayElements(env, firstcoord, NULL);
	double *ycoord = (* env) -> GetDoubleArrayElements(env, secondcoord, NULL);
	double *zcoord = (* env) -> GetDoubleArrayElements(env, values, NULL);

        pj_transform( src_pj, dst_pj, pcount,poffset, xcoord, ycoord, zcoord);

	(* env)->ReleaseDoubleArrayElements(env,firstcoord,(jdouble *) xcoord, 0);
	(* env)->ReleaseDoubleArrayElements(env,secondcoord,(jdouble *) ycoord, 0);
	(* env)->ReleaseDoubleArrayElements(env,values,(jdouble *) zcoord, 0);

	pj_free( src_pj );
	pj_free( dst_pj );
}

/*!
 * \brief
 * retrieves projection parameters
 *
 * JNI informations:
 * Class:     org_proj4_Projections
 * Method:    getProjInfo
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 *
 *
 * \param env - parameter used by jni (see JNI specification)
 * \param parent - parameter used by jni (see JNI specification)
 * \param projdefinition - definition of the projection
*/
JNIEXPORT jstring JNICALL Java_org_proj4_Projections_getProjInfo
  (JNIEnv * env, jobject parent, jstring projdefinition)
{
	PJ *pj;
	char * pjdesc;
	char info[arraysize];

	char * proj_def = (char *) (*env)->GetStringUTFChars (env, projdefinition, 0);

	if (!(pj = pj_init_plus(proj_def)))
		exit(1);

	/* put together all the info of the projection and free the pointer to pjdesc */
	pjdesc = pj_get_def(pj, 0);
	strcpy(info,pjdesc);
	pj_dalloc(pjdesc);

	return (*env)->NewStringUTF(env,info);
}


/*!
 * \brief
 * retrieves ellipsoid parameters
 *
 * JNI informations:
 * Class:     org_proj4_Projections
 * Method:    getEllipsInfo
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 *
 *
 * \param env - parameter used by jni (see JNI specification)
 * \param parent - parameter used by jni (see JNI specification)
 * \param projdefinition - definition of the projection
*/
JNIEXPORT jstring JNICALL Java_org_proj4_Projections_getEllipsInfo
  (JNIEnv * env, jobject parent, jstring projdefinition)
{
	PJ *pj;
	char * pjdesc;
	char ellipseinfo[arraysize];
	char temp[50];

	char * proj_def = (char *) (*env)->GetStringUTFChars (env, projdefinition, 0);

	if (!(pj = pj_init_plus(proj_def)))
		exit(1);

	/* put together all the info of the ellipsoid  */
/* 	sprintf(temp,"name: %s;", pj->descr); */
	sprintf(temp,"name: not available;");
	strcpy(ellipseinfo,temp);
	sprintf(temp,"a: %lf;", pj->a);
	strcat(ellipseinfo,temp);
	sprintf(temp,"e: %lf;", pj->e);
	strcat(ellipseinfo,temp);
	sprintf(temp,"es: %lf;", pj->es);
	strcat(ellipseinfo,temp);
	sprintf(temp,"ra: %lf;", pj->ra);
	strcat(ellipseinfo,temp);
	sprintf(temp,"one_es: %lf;", pj->one_es);
	strcat(ellipseinfo,temp);
	sprintf(temp,"rone_es: %lf;", pj->rone_es);
	strcat(ellipseinfo,temp);
	sprintf(temp,"lam0: %lf;", pj->lam0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"phi0: %lf;", pj->phi0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"x0: %lf;", pj->x0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"y0: %lf;", pj->y0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"k0: %lf;", pj->k0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"to_meter: %lf;", pj->to_meter);
	strcat(ellipseinfo,temp);
	sprintf(temp,"fr_meter: %lf;", pj->fr_meter);
	strcat(ellipseinfo,temp);

	return (*env)->NewStringUTF(env,ellipseinfo);
}

#endif
