from IrisBackendv3.codec.files.file import File


class Image(File):
    # override save

    def to_b64(self) -> str:
        pass