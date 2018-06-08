set(installdest include/proj)

install(
  FILES util.hpp metadata.hpp common.hpp crs.hpp datum.hpp
	coordinatesystem.hpp coordinateoperation.hpp io.hpp exception.hpp nn.hpp
  DESTINATION ${installdest}
)
