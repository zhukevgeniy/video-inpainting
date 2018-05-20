import configparser


def parse_config(path):
    config = configparser.ConfigParser()
    config.read(path)

    video_path = config.get("settings", "video_path")
    mask_path = config.get("settings", "mask_path")

    return video_path, mask_path
