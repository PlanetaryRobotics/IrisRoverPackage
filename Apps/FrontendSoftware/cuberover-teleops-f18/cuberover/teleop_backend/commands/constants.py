import enum


@enum.unique
class CommandTypeId(enum.IntFlag):
    ESTOP = 0x00
    DRIVING = 0x01
    IMAGING = 0x02
    TELEMETRY_SETTINGS = 0x03
    RESET_SETTINGS = 0x04
    SETTINGS = 0x05
    DATA_BLOB = 0x10


@enum.unique
class DrivingCommandId(enum.IntFlag):
    FORWARD = 0x00
    REVERSE = 0x01
    LEFT = 0x02
    RIGHT = 0x03


@enum.unique
class ImagingCommandId(enum.IntFlag):
    IMAGE_REQUEST = 0x00
    DRIVING_IMAGE_CONFIG = 0x01
    ISO = 0x02
    SHUTTER_SPEED = 0x03
    WHITE_BALANCE = 0x04


@enum.unique
class TelemetrySettingsCommandId(enum.IntFlag):
    GYROSCOPE_CONFIG = 0x00
    ACCELEROMETER_CONFIG = 0x01
    THERMISTORS_CONFIG = 0x02
    MOTOR_CURRENT_CONFIG = 0x03
    MOTOR_VELOCITY_CONFIG = 0x04
    BATTERY_VOLTAGES_CONFIG = 0x05
    POWER_SUPPLY_CURRENT_CONFIG = 0x06
    RADIO_HEARTBEAT_CONFIG = 0x07
    HERCULES_HEARTBEAT_CONFIG = 0x08
    COULOMB_COUNTER_CONFIG = 0x09


@enum.unique
class ResetSettingsCommandId(enum.IntFlag):
    RESET_ALL = 0x00
    RESET_DRIVE = 0x01
    RESET_CAMERA = 0x02


@enum.unique
class SettingsCommandId(enum.IntFlag):
    ACCELERATION_CURVE = 0x00
    MAX_DRIVE_TIME = 0x01
    MAX_DRIVE_DISTANCE = 0x02


@enum.unique
class DataBlobCommandId(enum.IntFlag):
    INIT_BLOB = 0x00
    BLOB_FRAGMENT = 0x01