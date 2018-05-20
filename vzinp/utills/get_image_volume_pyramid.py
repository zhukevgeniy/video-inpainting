from scipy import signal
import numpy as np


def matlab_style_gauss2D(shape=(5, 5), sigma=0.5):
    """
    2D gaussian mask - should give the same result as MATLAB's
    fspecial('gaussian',[shape],[sigma])
    """
    m, n = [(ss - 1.) / 2. for ss in shape]
    y, x = np.ogrid[-m:m + 1, -n:n + 1]
    h = np.exp(-(x * x + y * y) / (2. * sigma * sigma))
    h[h < np.finfo(h.dtype).eps * h.max()] = 0
    sumh = h.sum()
    if sumh != 0:
        h /= sumh
    return h


def gaussian_pyramid(shape, scale, step):
    yield shape

    old_w = shape[0]
    old_h = shape[1]

    for i in range(step):
        w = int(old_w / scale)
        h = int(old_h / scale)
        old_w, old_h = w, h
        yield [w, h, shape[2], shape[3]]


def get_image_volume_pyramid(image_volume, filter_size, sigma, level_num, min_temporal_size):
    downsample_step = 2
    temporal_filtering = 0

    gauss = matlab_style_gauss2D((filter_size, 1), sigma)

    return [shape for shape in gaussian_pyramid(shape=image_volume, scale=2, step=downsample_step)]
