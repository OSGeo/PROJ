/**=====================================================================================

 FILE:  DataToProject.java

 DESCRIPTION:  class representing the dataset to be reprojected

 NOTES:  ---
 AUTHOR:          Antonello Andrea
 EMAIL:               andrea.antonello@hydrologis.com
 COMPANY:       HydroloGIS / Engineering, University of Trento / CUDAM
 COPYRIGHT:    Copyright (C) 2004 HydroloGIS / University of Trento / CUDAM, ITALY, GPL
 VERSION:         $version$
 CREATED OR MODIFIED:  Oct 18, 2004
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
 * class representing the dataset to be reprojected
 */
public class ProjectionData
{

  // the variables are kept public, since they are transformed passing through
  // two classes
  public double[] x = null;
  public double[] y = null;
  public double[] z = null;
  public int rows = 0;

  /**
   * object to hold the data to be transformed. This will be passed from 
   * the starting projection object to the destinantion projection passing through
   * the transformation.
   */
  public ProjectionData(double[][] _coord, double[] _values)
  {
    rows = _coord.length;
    x = new double[rows];
    y = new double[rows];
    
    for (int i = 0; i < rows; i++)
    {
      x[i] = _coord[i][0];
      y[i] = _coord[i][1];
    }
    z = _values;
  }
}
