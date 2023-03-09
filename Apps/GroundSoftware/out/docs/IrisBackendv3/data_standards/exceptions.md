Module IrisBackendv3.data_standards.exceptions
==============================================
Various exceptions used throughout this package.

@author: Connor W. Colombo (CMU)
@last-updated: 04/05/2021

Classes
-------

`StandardsFormattingException(file_uri: str, info: str = '')`
:   Formatting of Some Standards File doesn't Conform to Expectations.
    
    Create a StandardsFormattingException.
    
    Args:
        file_uri (str): URI of the standards file containing the formatting exception.
        info (str, optional): Info about the specific formatting issue. Defaults to "".

    ### Ancestors (in MRO)

    * builtins.Exception
    * builtins.BaseException