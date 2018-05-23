# PROJ bridge to Java

This is the third release of JNI wrappers for the main PROJ functions.
The first release of JNI wrappers were created by http://www.hydrologis.com.
The second release of JNI wrappers were created by http://www.geoapi.org.
This release is compatible with any PROJ versions from 4.8 to 5
provided that PROJ has been compiled as described below.



## What is "PROJ bridge to Java"

_PROJ bridge to Java_ is a small library of Java classes that wrap a few PROJ functions
by using the Java Native Interface (JNI). The main Java class is `org.proj4.PJ`.
A Java code example is given in the _Usage & a fast example_ section below.



### Versions

The PROJ bridge to Java does not follow the same version numbers than the main PROJ library
since the same JAR file can be compatible with a range of PROJ versions.
Version compatibility is given below:


Java bridge | Compatible with PROJ library
----------- | ----------------------------
2.0 and 3.0 | 4.8 to 5+
1.0         | 4.4.9 to 4.8 inclusive



### Compilation

To compile the native part, `configure` has to be run in the PROJ directory like this:

    CFLAGS=-Iinclude2 ./configure --with-jni=include1

where

* `include1` = folder in which the header file `jni.h` resides (usually `$JAVA_HOME/include`)
* `include2` = folder in which the header file `jni_md.h` resides (usually `$JAVA_HOME/include/linux` or whatever)

On MacOS, those two folders are `/Library/Java/JavaVirtualMachines/.../Contents/Home/include/`.


The java part is compiled by running Ant inside the `jniwrap` folder.
This will compile the classes and archive them in a JAR file.
It applies to Linux, MacOS and Windows (and virtually to every system supporting java).



### Requirements

Beyond the ones already put by PROJ, you need:

* For compilation:
  * Java 9+, the Java standard development kit version 9 or above
  * Ant 1.10+, to run the build.
* For execution:
  * If a Java version less than the current version on the local machine is desired,
    add a `release` attribute in the `javac` task of `build.xml` before to compile.
  * Proj version 4.8 or more recent compiled with the `--with-jni` option.



### Documentation

The documentation is held inside the code and can be retrieved by running
`ant javadoc` inside the folder `jniwrap`. This will create the HTML format
documentation inside of `jniwrap/out/apidocs`



### License

GPL for the first release.
Proj.4 license for the second release.



### Authors

* Andrea Antonello (andrea.antonello@hydrologis.com)
* Martin Desruisseaux (martin.desruisseaux@geomatys.com)



## Usage & a fast example:

The `proj.jar` is all is needed to implement PROJ support in java applications.
The whole job is done by the PROJ library, so there are just a couple of functions that be used.

The best way is to see everything through an example.
In the following example we create two Coordinate Reference System and transform 3 points.
The Coordinate Reference Systems and the points are hard-coded for simplicity.
Of course, real applications would read them from a file or other data source.

    import org.proj4.*;
    import java.util.Arrays;

    /**
     * Converts coordinates from EPSG:32632 (WGS 84 / UTM zone 32N) to WGS84,
     * then prints the result to the standard output stream.
     */
    public class Main {
        public static void main(String[] args) throws PJException {
            PJ sourcePJ = new PJ("+init=epsg:32632");           // (x,y) axis order
            PJ targetPJ = new PJ("+proj=latlong +datum=WGS84"); // (λ,φ) axis order
            double[] coordinates = {
                500000,       0,   // First coordinate
                400000,  100000,   // Second coordinate
                600000, -100000    // Third coordinate
            };
            sourcePJ.transform(targetPJ, 2, coordinates, 0, 3);
            System.out.println(Arrays.toString(coordinates));
        }
    }



### Compile the Main code

We assume that PROJ was compiled with the right flag to support the bridge to Java.
Therefore we have a library called `proj.jar`.
Thus we compile the `Main.java` with the command:

    javac --class-path <path to the jar library>/proj.jar Main.java

and execute the created test case with (replace `:` by `;` on the Windows platform):

    java --class-path <path to the jar library>/proj.jar:. Main



### Troubleshooting

If an `java.lang.UnsatisfiedLinkError` is thrown at execution time, add the following line in the Java code:

    System.out.println(System.getProperty("java.library.path"));

Then verify that the `libproj.so` (Linux), `libproj.dylib` (MacOS) or `libproj.dll` (Windows) file is located
in one of the directories listed by above code. If this is not the case, then try configuring the
`LD_LIBRARY_PATH` (Linux), `DYLD_LIBRARY_PATH` (MacOS) or `PATH` (Windows) environment variable.
Alternatively, a `-Djava.library.path=<path to the libproj>` option can be added to above `java` command.
If the problem persist, adding the `-verbose:jni` option to the `java` command may help more advanced diagnostics.
