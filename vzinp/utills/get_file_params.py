import cv2
import pathlib


def get_frames_from_video_capture(video_capture):
    """
    Function that returns frame list by video capture
    :param video_capture: video capture
    :return: frame list
    """
    while video_capture.isOpened():
        success, frame = video_capture.read()
        if not success:
            break
        else:
            yield frame


def get_file_type(file_path):
    """
    Function that returns file type
    :param file_path: path to file
    :return: file type
    """
    ext = pathlib.Path(file_path).suffix
    return "video" if ext == ".avi" else "image"


def parse_path(file_path):
    """
    Function that returns file type and windows absolute file path
    :param file_path: path to file
    :return: file path, file type
    """
    fpath, ftype = None, None
    try:
        fpath = pathlib.Path(file_path).absolute()
        ftype = get_file_type(file_path=fpath)
    except FileNotFoundError:
        pass

    return str(fpath), ftype


def get_video_params(video_path):
    """
    Function that returns frame set of uint8
    :param video_path: video path
    :return: frame set of uint8
    """
    video_capture = cv2.VideoCapture(video_path)
    frames = get_frames_from_video_capture(video_capture=video_capture)
    frame_list = [frame for frame in frames]

    height = video_capture.get(cv2.CAP_PROP_FRAME_HEIGHT)
    width = video_capture.get(cv2.CAP_PROP_FRAME_WIDTH)
    video_len = video_capture.get(cv2.CAP_PROP_FRAME_COUNT)
    rgb_channels = 3

    return frame_list, tuple(map(int, (height, width, video_len, rgb_channels)))


def get_image_params(image_path):
    """
    Function that returns image uint8 array
    :param image_path: image path
    :return: image uint8 array
    """
    image = cv2.imread(image_path)

    return image, image.shape


def get_file_params(file_path):
    """
    Function that returns file volume and dimension
    :param file_path: file path
    :return: volume, dimension
    """
    fpath, ftype = parse_path(file_path=file_path)

    if ftype == "video":
        volume, dims = get_video_params(video_path=fpath)
    else:
        volume, dims = get_image_params(image_path=fpath)

    return volume, dims
