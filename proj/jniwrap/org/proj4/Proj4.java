/**=====================================================================================

 FILE:  Proj4.java

 DESCRIPTION:  

 NOTES:  ---
 AUTHOR:          Antonello Andrea
 EMAIL:               andrea.antonello@hydrologis.com
 COMPANY:       HydroloGIS / Engineering, University of Trento / CUDAM
 COPYRIGHT:    Copyright (C) 2004 HydroloGIS / University of Trento / CUDAM, ITALY, GPL
 VERSION:         $version$
 CREATED OR MODIFIED:  Oct 15, 2004
 REVISION:  ---
 =====================================================================================*/

/* 
 This library is free software; you can redistribute it and/or 
 modify it under the terms of the GNU Library General Public 
 License as published by the Free Software Foundation; either 
 version 2 of the License, or (at your option) any later version. 

 This library is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 Library General Public License for more details. 

 You should have received a copy of the GNU Library General Public 
 License along with this library; if not, write to the Free 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
 USA 

 1. Redistributions of source code must retain the above copyright   
 notice, this list of conditions and the following disclaimer.   
 2. Redistributions in binary form must reproduce the above copyright   
 notice, this list of conditions and the following disclaimer in the   
 documentation and/or other materials provided with the distribution.   
 */
package org.proj4;

import java.util.LinkedHashMap;

/**
 * This is the public 
 * test class to try the jproj api. Reads data from file.
 * The input file has three lines of header:<BR>
 * 1) source projection code or option <BR>
 * 2) destination projection code or option<BR>
 * 3) rows of the data<BR>
 * and after the header the data are put in the format:
 * x0 y0 z0
 * x1 y1 z1
 * x2 y2 z2
 *...etc etc
 *
 * example:
 * srcProj: +proj=latlong +datum=WGS84
 * destProj: +init=epsg:32632
 * rows: 2
 * ...datatriplets
 * 
 */
public class Proj4 implements Proj4Factory
{
  Projections srcProjection = null;
  Projections destProjection = null;
  Projections projection = null;
  
  /**
   * constructor used to instatiate a single projection. This is used
   * if the goal is to simply get information about a projection definition
   * 
   * @param proj the projection definition
   */
  public Proj4(String proj)
  {
    if (proj.indexOf("latlong") != -1)
    {
      projection = new LatLong(proj);
    }
    else
    {
      projection = new Others(proj);
    }
  }
  
  /**
   * constructor used to instantiate the object for a further reproiection.
   * From the definitions the source and destination projection are 
   * created.
   * 
   * @param srcProj
   * @param destProj
   */
  public Proj4(String srcProj, String destProj)
  {
    if (srcProj.indexOf("latlong") != -1)
    {
      srcProjection = new LatLong(srcProj);
    }
    else
    {
      srcProjection = new Others(srcProj);
    }
    if (destProj.indexOf("latlong") != -1)
    {
      destProjection = new LatLong(destProj);
    }
    else
    {
      destProjection = new Others(destProj);
    }
  }

  /**
   * method to reproject a dataset from the source projection to the destination
   * projection as defined in the constructor
   * 
   * @param dataTP the data set to reproject
   * @param point_count
   * @param point_offset
   */
  public void transform(ProjectionData dataTP, long point_count, int point_offset)
  {
    srcProjection.prepareData(dataTP);
    destProjection.doTheTransform(srcProjection, dataTP, point_count,
        point_offset);
    destProjection.prepareTransformedData(dataTP);

    System.out.println("Transformed coordinates and values:");
    for (int i = 0; i < dataTP.rows; i++)
    {
      System.out.println("x = " + dataTP.x[i] + " y = " + dataTP.y[i] + " z = "
          + dataTP.z[i]);
    }
  }
  
  /**
   * @return the projection info as a hashmap
   */
  public LinkedHashMap getProjInfo()
  {
    return projection.mapProjInfo();
  }

  /**
   * @return the source projection info as a hashmap
   */
  public LinkedHashMap getSrcProjInfo()
  {
    return srcProjection.mapProjInfo();
  }
  
  /**
   * @return the destination projection info as a hashmap
   */
  public LinkedHashMap getDestProjInfo()
  {
    return destProjection.mapProjInfo();
  }
  

  /**
   * print the projection info to standard output
   */
  public void printProjInfo()
  {
    projection.printProjInfo();
  }
  
  /**
   * print the source projection info to standard output
   */
  public void printSrcProjInfo()
  {
    srcProjection.printProjInfo();
  }

  /**
   * print the destination projection info to standard output
   */
  public void printDestProjInfo()
  {
    destProjection.printProjInfo();
  }

}
