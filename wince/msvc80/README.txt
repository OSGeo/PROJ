-------------------------------------------------------------------------------
PROJ.4 port for Windows CE
Author: Mateusz Loskot (mateusz@loskot.net)
-------------------------------------------------------------------------------

TODO - explain usage details.

FAST NOTE:
Add project for WCELIBCEX library to the solution.
WCELIBCEX library sources are required to build PROJ.4 port for Windows CE.
You can configure projce_dll and projce_lib projects to use WCELIBCEX through
separate project file set as dependency or in binary form,
using prepared static library.

Configure path to WCELIBCEX sources
-----------------------------------
1. Open View -> Property Manager.
2. Expand one of node under projce_dll or projce_lib project
3. Double-click on projce_common property sheet
4. Go to User Macros
5. Select WCELIBCEX_DIR macro and set path pointing directly to directory
where you downloaded the WCELIBCEX library sources tree.
6. Click OK and close the dialog box