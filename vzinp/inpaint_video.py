from vzinp.utills.get_image_volume_pyramid import get_image_volume_pyramid
from vzinp.utills.get_video_features import get_video_features
from vzinp.utills.parse_inpaint_params import parse_inpaint_params


def inpaint_video(image_volume, occlusion_volume, inpainting_params):
    """
    This function inpaints a video
    :param image_volume: volume of a video to inpaint
    :param occlusion_volume: volume of an occlusion
    :param inpainting_params: inpainting params
    :return: image output, shift volume output
    """

    # Gaussian pyramid params
    filter_size = 3
    scale_step = .5
    sigma = 1.5

    use_all_patches = 0
    reconstruction_type = 0

    # parse inpainting params
    [max_level, patch_size, texture_features_activated, sigma_colour, file] = parse_inpaint_params()

    dims = image_volume[1]

    patch_size["t"] = min(patch_size["t"], dims[2])

    patch_size = [patch_size["x"], patch_size["y"], patch_size["t"]]

    patch_match_params = {
        "patch_size": patch_size,
        "w": max(dims[1], dims[1], dims[2]),
        "alpha": .5,
        "full_search": 0,
        "partial_comparison": 1,
        "nb_iters_patch_size": 10,
        "patch_indexing": 0,
        "reconstruction_type": reconstruction_type
    }

    max_nb_itarations = 20
    residual_tresh = .1

    # img_volume_pyramid = get_image_volume_pyramid(image_volume, filter_size, sigma, max_level, dims[2])

    occlusion_volume_pyramid = get_image_volume_pyramid(image_volume, filter_size, sigma, max_level, dims[2])

    if texture_features_activated:
        print("Calculating texture feature pyramids")
        feature_pyramid = get_video_features(image_volume, occlusion_volume, max_level, file)

    return [12, 3]
