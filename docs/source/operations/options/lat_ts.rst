.. option:: +lat_ts=<value>

    Latitude of true scale. Defines the latitude where scale is not distorted.
    Takes precedence over ``+k_0`` if both options are used together.

    *Defaults to 0.0.*

    .. note::
        The default convention is to interpret this value as decimal degrees. To
        specify radians instead, follow the value with the "r" character.

        Example: `+lat_ts=1.570796r`

        See :ref:`Projection Units <projection_units>` for more information.
