from vzinp.inpaint_video import inpaint_video
from vzinp.utills.get_video_params import get_video_params


def seed_random_numbers():
    pass


def read_video(video_file, file_type):
    print("Reading input %s" % file_type)
    return get_video_params(video_file=video_file)[0]


def start_inpaint(**kwargs):
    image_volume = read_video(video_file=kwargs["video_file"], file_type="video")
    occlusion_volume = read_video(kwargs["occlusion_file"], file_type="occlusion")

    print("Starting the video inpainting... ")

    [video_size, video_name] = get_video_params(video_file=kwargs["video_file"])

    inpainting_params = (kwargs["level"], kwargs["asd"], kwargs["act"], kwargs["qwd"], 'file', video_name)

    image_volume_output = inpaint_video(image_volume, occlusion_volume, inpainting_params)
