#Name: Ana Bindiu
#UCID: 30062098
#this code was referenced from the code provided in class: setup.py under 2-cython-example
from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

ext_modules = [
    Extension( "GPIO_Example",
               sources=["wrapper.pyx"],
               include_dirs = ["/opt/vc/include"],
               libraries = [ "bcm_host" ],
               library_dirs = ["/opt/vc/lib"]
    )
]

    
setup(
    name = "GPIO_Example",
    ext_modules = cythonize( ext_modules )
)
