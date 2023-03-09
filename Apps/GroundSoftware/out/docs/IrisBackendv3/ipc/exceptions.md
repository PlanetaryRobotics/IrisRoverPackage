Module IrisBackendv3.ipc.exceptions
===================================
Various exceptions used throughout the IPC package.

@author: Connor W. Colombo (CMU)
@last-updated: 03/04/2023

Classes
-------

`DecodedInterProcessMessageContentTypeException(content: Any, expected_type: Type)`
:   The content of a decoded `InterProcessMessage` did not match the
    expected type.
    
    Create an `InterProcessMessageContentTypeException`.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.exceptions.IpcException
    * builtins.Exception
    * builtins.BaseException

`IpcEndAppRequest(why: str = '')`
:   Exception to raise when a task / coroutine wants to end execution of the
    entire app.
    NOTE: Depending on how `return_when` of the app is configured, this may or
    may not actually end the app. The app may close when it gets an
    exception/completion from any task or it may end only when all of its
    managed tasks end.
    
    Create a `IpcEndAppRequest`.
    
    Args:
        why (str, optional): Why the app is being ended. Defaults to "".

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.exceptions.IpcException
    * builtins.Exception
    * builtins.BaseException

`IpcException(*args, **kwargs)`
:   Parent for all IPC exceptions.

    ### Ancestors (in MRO)

    * builtins.Exception
    * builtins.BaseException

    ### Descendants

    * IrisBackendv3.ipc.exceptions.DecodedInterProcessMessageContentTypeException
    * IrisBackendv3.ipc.exceptions.IpcEndAppRequest
    * IrisBackendv3.ipc.exceptions.MessageIntegrityError

`MessageIntegrityError(info: str = '')`
:   Error to raise when a received message fails integrity check. 
    
    Create a `MessageIntegrityError`.
    
    Args:
        info (str, optional): Info about the specific formatting issue. Defaults to "".

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.exceptions.IpcException
    * builtins.Exception
    * builtins.BaseException

`UnhandledTopicException(payload: IpcPayload, info: str = '')`
:   Exception to raise when a `SocketTopicHandler` doesn't have a
    handler for this payload's topic.
    
    Create a `UnhandledTopicException`.
    
    Args:
        info (str, optional): Info about the specific formatting issue. Defaults to "".

    ### Ancestors (in MRO)

    * builtins.Exception
    * builtins.BaseException

    ### Class variables

    `payload: IpcPayload`
    :