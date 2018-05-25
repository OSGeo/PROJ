set(installdest include/proj)

install(
  FILES util.hh metadata.hh common.hh crs.hh datum.hh
	coordinatesystem.hh coordinateoperation.hh io.hh exception.hh nn.hpp
  DESTINATION ${installdest}
)
