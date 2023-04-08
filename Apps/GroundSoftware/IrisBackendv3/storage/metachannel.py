
# if __name__ == "__main__":
# # Serialize ChannelLogs by TelemetryChannel
# `
# # Metachannel < TelemetryChannel
# orientation = MetaChannel(
#     name="orientation",
#     metachannel_id=0x01,
#     datatype=FSWDataType.F64,
#     subscriptions=['IMU_X', 'IMU_Y', 'IMU_Z'],
#     calculate=calculate_orientation,
#     slice_window=slice(-1, -4, -1)
# )

# def calculate_orientation(IMU_X: List[float], IMU_Y: List[float], IMU_Z: List[float]) -> List[]:
#     # TODO: check units

#     class Metachannel(TelemetryChannel):
#         __slots__: List[str] = [
#             # Telemetry Channel names which this should be subscribed to.
#             'subscriptions',
#             # Function to calculate the value for this channel given lists of telemetry from each of the subscribed channels:
#             'calculate',
#             # Slice object indicating which objects to grab from each channel. If None, all are grabbed:
#             'slice_window'
#         ]
#         subscriptions: List[str]
#         calculate: Callable[[KwArg[List[Any]]], Any]
#         slice_window: Optional[slice]

#         # Offset to be added to all metachannel IDs to make sure they won't collide
#         # with regular TelemetryChannel IDs from FPrime.
#         # This is more of a secondary measure to ensure their separation, they
#         # should still be separated by other means as well (i.e. not mixed in a
#         # collection)
#         METACHANNEL_ID_OFFSET: int = 0xFFFF

#         # w.r.t. serialization: this doesn't need to be cached, only need so
#         # frontend can know what's going on.
#         def __init__(self,
#                      name: str,
#                      metachannel_id: int,
#                      datatype: FswDataType,
#                      subscriptions: List[str],
#                      calculate: Callable[[KwArg[List[Any]]], Any],
#                      slice_window: Optional[slice] = None,
#                      metadata_json_str: str = "",
#                      enum: List[EnumItem] = None,
#                      bitfields: BitfieldStruct = None
#                      ) -> None:

#             super().__init__(
#                 name=name,
#                 ID=metachannel_id + METACHANNEL_ID_OFFSET,
#                 datatype=datatype,
#                 enum=enum,
#                 bitfields=bitfields,
#                 metadata_json_str=metadata_json_str
#             )

#             self.subscriptions = subscriptions
#             # TODO: Actually subscribe to the ChannelLogs
#             self.calculate = calculate
#             self.slice_window = slice_window

#         def recompute(self) -> TelemetryPayload:
#             # Grab the latest values for each subscribed channel:
#             latest_vals = dict()
#             for channel in subscriptions:
#                 if self.slice_window is not None:
#                     latest_vals[channel] = storage.get_latest(
#                         channel, slice_window)
#                 else:
#                     latest_vals[channel] = storage.get_stream(channel)

#             # Pass those along to the `calculate_function` and the new value:
#             new_val = self.calculate(**latest_vals)

# `
