from vzinp.utills.get_image_volume_pyramid import get_image_volume_pyramid
from vzinp.utills.parse_inpaint_params import parse_inpaint_params


def inpaint_video(image_volume, occlusion_volume, inpainting_params):
    """
    This function inpaints a video

    :param image_volume: input video
    :param occlusion_volume: occlusion video
    :param inpainting_params: inpainting params
    :return:
    """

    # Gaussian pyramid params
    filter_size = 3
    scale_step = .5
    sigma = 1.5

    use_all_patches = 0
    reconstruction_type = 0

    # parse inpainting params
    [max_level, patch_size, texture_features_activated, sigma_colour, file] = parse_inpaint_params()

    patch_size["t"] = min(patch_size["t"], image_volume[2])

    patch_size = [patch_size["x"], patch_size["y"], patch_size["t"]]

    patch_match_params = {
        "patch_size": patch_size,
        "w": max(image_volume[1], image_volume[1], image_volume[2]),
        "alpha": .5,
        "full_search": 0,
        "partial_comparison": 1,
        "nb_iters_patch_size": 10,
        "patch_indexing": 0,
        "reconstruction_type": reconstruction_type
    }

    print(patch_match_params)

    max_nb_itarations = 20
    residual_tresh = .1

    img_volume_pyramid = get_image_volume_pyramid(image_volume, filter_size, sigma, max_level, image_volume[2])

    if texture_features_activated:
        print("Calculating texture feature pyramids")
        feature_pyramid = get_video_features(image_volume, occlusion_volume, max_level, file)

    return [12, 3]
