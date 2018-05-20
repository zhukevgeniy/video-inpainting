import cv2
import os


def get_video_params(video_file):
    video_path = os.path.normpath(os.path.abspath(video_file))
    video_capture = cv2.VideoCapture(video_path)

    height = video_capture.get(cv2.CAP_PROP_FRAME_HEIGHT)
    width = video_capture.get(cv2.CAP_PROP_FRAME_WIDTH)
    video_len = video_capture.get(cv2.CAP_PROP_FRAME_COUNT)
    rgb_channels = 3

    params = [height, width, video_len, rgb_channels]
    video_name = video_file.split("/")[-1].split(".")[0]

    return [params, video_name]
