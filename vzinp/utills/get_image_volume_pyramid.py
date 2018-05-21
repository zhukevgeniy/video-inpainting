from scipy import signal
import numpy as np
import cv2
from scipy import signal, special, misc


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


def get_gaussian_pyramid(image):
    layer = image.copy()
    gauss_pyramid = [layer]
    for i in range(2):
        layer = cv2.pyrDown(layer, dstsize=(int(layer.shape[1] / 2), int(layer.shape[0] / 2)))
        gauss_pyramid.append(layer)

    return np.array(gauss_pyramid)


def sigmask(image):
    r, c = image.shape
    y = special.expit(np.arange(-c // 2, c // 2))
    return np.tile(y, (r, 1))


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
    temporal_filtering = 0

    gauss = matlab_style_gauss2D((filter_size, 1), sigma)

    img = image_volume[0]

    misc.imshow(img)

    # img_pyr = cv2.pyrDown(img, dstsize=(int(img.shape[1]), int(img.shape[0] / 2)))

    print(sigmask(img))

    return 2

    # return [shape for shape in gaussian_pyramid(shape=image_volume, scale=2, step=downsample_step)]
