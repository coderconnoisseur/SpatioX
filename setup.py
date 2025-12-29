from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "spatio._spatio_core",
        ["src/record_store.cpp", 
         "src/spatial_index.cpp", 
         "src/temporal_index.cpp", 
         "src/spatio_index_core.cpp",
         "src/bindings.cpp"],
        include_dirs=["include"],
        cxx_std=17,
    ),
]

setup(
    name="spatio",
    version="0.1.0",
    author="Your Name",
    description="Spatial-Temporal Index Engine",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    packages=["spatio"],
    package_dir={"spatio": "python/spatio"},
    install_requires=["pybind11>=2.6.0"],
    python_requires=">=3.7",
)
