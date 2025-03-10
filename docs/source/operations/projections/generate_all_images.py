import os
import pathlib

all_images_path = (pathlib.Path(__file__).parent.resolve() / 'all_images.rst').as_posix()
images_path = (pathlib.Path(__file__).parent.resolve() / 'images').as_posix()

exceptions = {
    "grieger_triptychial" : "peirce_q",
    "peirce_q_square" : "peirce_q",
    "peirce_q_diamond" : "peirce_q",
    "peirce_q_horizontal" : "peirce_q",
}

with open(all_images_path, 'w') as all_images:
    all_images.write(""".. _all_images:

********************************************************************************
List of all projection images
********************************************************************************

""")
    first = True
    for filename in sorted(os.listdir(images_path)):
        if filename.endswith('.png'):
            name = filename[:-4]
            if name in exceptions:
                name = exceptions[name]
            if not first:
                all_images.write(f"""
********************************************************************************
""")
            first = False
            all_images.write(f"""

:ref:`{name}`

.. figure:: ./images/{filename}
   :width: 500 px
   :align: center
   :alt:   {name}

""")
