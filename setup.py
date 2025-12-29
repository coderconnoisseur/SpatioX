from pathlib import Path
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext
import pybind11

ROOT = Path(__file__).parent.resolve()

ext_modules = [
    Pybind11Extension(
        "spatiox._spatio_core",
        [
            "src/record_store.cpp",
            "src/spatial_index.cpp",
            "src/temporal_index.cpp",
            "src/spatio_index_core.cpp",
            "src/bindings.cpp",
        ],
        include_dirs=[
            str(ROOT / "include"),
            pybind11.get_include(),
        ],
        cxx_std=17,
    ),
]

setup(
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
