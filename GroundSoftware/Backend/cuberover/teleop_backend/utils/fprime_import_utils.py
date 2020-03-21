import inspect
import os
import pathlib
import sys

import git


__THIS_FILE_PATH = os.path.realpath(inspect.getfile(inspect.currentframe()))

# This is a workaround for differing behavior of the pathlib.Path.resolve() method between Python <3.6 and >=3.6.
# Calling path.resolve(**__PATH_RESOLVE_KWARGS) gives the behavior of path.resolve() in <3.6 which is equivalent to
# path.resolve(strict=True) in >=3.6
if sys.version_info >= (3, 6):
    __PATH_RESOLVE_KWARGS = {"strict": True}
else:
    __PATH_RESOLVE_KWARGS = {}


def get_fprime_root_path_from_git_root() -> pathlib.Path:
    """Gets the path of the F Prime distribution root directory based on the root of the git repo containing this file.

    This function gets the root of the git repository that contains this file, which is expected to be the
    CubeRoverPackage repository, then constructs the expected path to the F Prime distribution from that git root. This
    will fail if this file is not in the CubeRoverPackage repository, or if the F Prime distribution root directory
    is in a different location than expected within the CubeRoverPackage repository.

    Returns:
        pathlib.Path: The path to the F Prime distribution root directory.

    Throws:
        git.InvalidGitRepositoryError: If this file does not exist within a git repository.
        FileNotFoundError: If the expected directory does not exist, or if a file exists at the expected path instead of
            the expected directory.
        RuntimeError: If an infinite loop is encountered while attempting to resolve the path.
    """
    git_repo = git.Repo(__THIS_FILE_PATH, search_parent_directories=True)
    git_root = git_repo.git.rev_parse("--show-toplevel")
    git_root_path = pathlib.Path(git_root)
    fprime_root_path = git_root_path.joinpath("FlightSoftware", "Apps", "fprime").resolve(**__PATH_RESOLVE_KWARGS)

    if not fprime_root_path.is_dir():
        raise FileNotFoundError("A file exists at the path ({}) expected to contain the"
                                " F Prime distribution root directory".format(str(fprime_root_path)))

    return fprime_root_path


def get_fprime_gds_python_package_path_from_fprime_root(fprime_root_path: pathlib.Path) -> pathlib.Path:
    """Generates the path of the fprime_gds package based on the path of the F Prime root directory.

    This function also verifies that the the fprime_gds Python package directory exists where expected, and will raise an
    exception if that is not the case.

    Args:
        fprime_root_path: The path of the F Prime distribution root directory.

    Returns:
        pathlib.Path: The path of the fprime_gds python package within the F Prime distribution.

    Throws:
        FileNotFoundError: If the expected directory does not exist, or if a file exists at the expected path instead of
            the expected directory
        RuntimeError: If an infinite loop is encountered while attempting to resolve the path.
    """
    fprime_gds_path = fprime_root_path.joinpath("Gds", "src").resolve(**__PATH_RESOLVE_KWARGS)

    if not fprime_gds_path.is_dir():
        raise FileNotFoundError("A file exists at the path ({}) expected to contain the"
                                " fprime_gds Python package".format(str(fprime_gds_path)))

    return fprime_gds_path


def get_fprime_fw_python_package_path_from_fprime_root(fprime_root_path: pathlib.Path) -> pathlib.Path:
    """Generates the path of the fprime package (from the Fw module) based on the path of the F Prime root directory.

    This function also verifies that the the fprime Python package directory exists where expected, and will raise an
    exception if that is not the case.

    Args:
        fprime_root_path: The path of the F Prime distribution root directory.

    Returns:
        pathlib.Path: The path of the fprime Python package within the F Prime distribution.

    Throws:
        FileNotFoundError: If the expected directory does not exist, or if a file exists at the expected path instead of
            the expected directory
        RuntimeError: If an infinite loop is encountered while attempting to resolve the path.
    """
    fprime_fw_path = fprime_root_path.joinpath("Fw", "Python", "src").resolve(**__PATH_RESOLVE_KWARGS)

    if not fprime_fw_path.is_dir():
        raise FileNotFoundError("A file exists at the path ({}) expected to contain the"
                                " fprime Python package (from the Fw module)".format(str(fprime_fw_path)))

    return fprime_fw_path





