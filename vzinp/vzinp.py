from vzinp.inpaint_video import inpaint_video
from vzinp.utills.get_file_params import get_file_params


def seed_random_numbers():
    pass


def read_input_file(file_path, file_type):
    print("Reading input %s" % file_type)

    return get_file_params(file_path=file_path)


def start_inpaint(**kwargs):
    input_video_volume = read_input_file(file_path=kwargs["video_file"], file_type="video")
    occlusion_volume = read_input_file(file_path=kwargs["occlusion_file"], file_type="occlusion")

    print("Starting the video inpainting... ")

    inpainting_params = (kwargs["level"], kwargs["asd"], kwargs["act"], kwargs["qwd"], 'file', kwargs["video_file"])

    image_volume_output = inpaint_video(input_video_volume, occlusion_volume, inpainting_params)
