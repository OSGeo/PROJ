.. option:: +lat_0=<value>

    Geodetic latitude of the geographic point that should land at the
    projected origin ``(0, 0)``. Used together with ``+lon_0``; the pair
    translates the projected output without rotating the polyhedron.
    ``+x_0`` / ``+y_0`` are applied on top of this translation.

    When unset, the projected origin falls on the unfold's root face
    instead (its centroid in general, or its bounding-box centre for
    ``+proj=isea``) rather than at a fixed lat / lon.

    .. note::
        The default convention is to interpret this value as decimal degrees. To
        specify radians instead, follow the value with the "r" character.

        Example: `+lat_0=1.570796r`

        See :ref:`Projection Units <projection_units>` for more information.
