import numpy as np
import cv2


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


def get_gaussian_pyramid(image, step):
    layer = image.copy()
    gauss_pyramid = [layer]
    for i in range(step):
        layer = cv2.pyrDown(layer)
        gauss_pyramid.append(layer)

    return np.array(gauss_pyramid)


def get_image_volume_pyramid(image_volume, filter_size, sigma, level_num, min_temporal_size):
    """
    This function calculates the image pyramid of an (colour) image volume and
    its occlusion volume, for a certain number of levels
    :param image_volume: image volume
    :param filter_size: downsampling filter size
    :param sigma: downsampling filter sigma
    :param level_num: number of levels in pyramid
    :param min_temporal_size: minimum temporal size
    :return: image volume pyramid
    """
    downsample_step = 2

    dim = np.array(image_volume).ndim

    if dim == 3:
        gaussian_pyramid = get_gaussian_pyramid(image=image_volume, step=downsample_step)
    else:
        gaussian_pyramid = [get_gaussian_pyramid(image=frame, step=downsample_step) for frame in image_volume]

    return gaussian_pyramid
