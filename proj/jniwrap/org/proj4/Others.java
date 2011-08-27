/**=====================================================================================

 FILE:  Others.java

 DESCRIPTION:  class representing a generic projection, i.e. with no particular preprocessing needs.

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
 * class representing a generic projection, i.e. with no particular
 * preprocessing needs.
 *
 * @deprecated Replaced by {@link PJ}, which doesn't need special subclass for this case.
 * The conversion of angular values, if needed, is performed directly in the {@link PJ}
 * native code, because the native code can more easily exploit the information provided
 * by Proj.4 about the CRS type.
 */
@Deprecated
public class Others extends Projections
{

  public Others(String src)
  {
    proj = src;
  }

  /* (non-Javadoc)
   * @see Projections#prepareData(DataToProject)
   */
  public void prepareData(ProjectionData dataTP)
  {
  }

  /* (non-Javadoc)
   * @see java.Projections#returnTransformedCoordinates()
   */
  public void prepareTransformedData(ProjectionData dataTP)
  {
  }

}
