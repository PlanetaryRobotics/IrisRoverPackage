# if __name__ == "__main__":
# # List of Telemetry Payloads for each channel:
# storage: NameIdDict[List[TelemetryPayload]]()
# meta_storage: NameIdDict[List[Metachannel]]()

# # List of just the latest extracted values for each type.
# # (heavier on memory, which we have a lot of, but speeds up operation, which we need):
# stream: NameIdDict[List[Any]]()
# meta_stream: NameIdDict[List[Any]]()

# # List of metachannel subscribers for each channel:
# subscribers: NameIdDict[List[Metachannel]]()

# def update_channel(id: int, new_payload: TelemetryPayload):
#     if isinstance(new_payload, Metachannel):
#         meta_storage[id].push(new_payload)
#         meta_stream[id].push(new_payload.value)
#     else:
#         storage[id].push(new_payload)
#         stream[id].push(new_payload.value)

#     for sub in subscribers[id]:
#         new_val = sub.recompute()
#         update_channel(sub.ID, new_val)

#     raise NotImplementedError()

# def get_latest(id: Union[int, str], slice_window: stream):
#     """
#     Returns the latest value from the channel with the given ID.
#     If a slice window is given, that slice window is applied to the telemetry
#     stream and returned instead.
#     """"

#     raise NotImplementedError()

# def get_stream(id):
#     # gets the stream from either the metachannel or telemetry channel streams
#     raise NotImplementedError()
