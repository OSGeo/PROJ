/**=====================================================================================

 FILE:  LatLong.java

 DESCRIPTION:  class representing a latitude longitude projection

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

/**
 * class representing a latitude longitude projection
 */
public class LatLong extends Projections
{

  public LatLong(String src)
  {
    proj = src;
  }

  /* (non-Javadoc)
   * @see Projections#prepareData(DataToProject)
   */
  public void prepareData(ProjectionData dataTP)
  {
    degreeToRadiant(dataTP.x, dataTP.y);
  }

  /* (non-Javadoc)
   * @see java.Projections#returnTransformedCoordinates()
   */
  public void prepareTransformedData(ProjectionData dataTP)
  {
    radiantToDegree(dataTP.x, dataTP.y);
  }

}
