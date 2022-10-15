import cv2


def rescale(picture, scale):
    if scale == 1.0:
        return picture

    newWidth = int(picture.shape[1] * scale)
    newHeight = int(picture.shape[0] * scale)

    if scale < 1.0:
        return cv2.resize(picture, (newWidth, newHeight), interpolation=cv2.INTER_AREA)
    else:
        return cv2.resize(picture, (newWidth, newHeight), interpolation=cv2.INTER_CUBIC)


def save(picture, name, path):
    cv2.imwrite(path + '/' + name + '.jpg', picture)


def show(picture, name):
    cv2.imshow(name, picture)
