def parse_inpaint_params(**kwargs):
    """
    function to parse the inpainting params
    :param kwargs:
    :return:
    """

    sigma_colour = 75
    max_level = 4

    patch_size = {
        "x": 5,
        "y": 5,
        "t": 5
    }
    texture_feature_activated = 1

    return [max_level, patch_size, texture_feature_activated, sigma_colour, "file"]
