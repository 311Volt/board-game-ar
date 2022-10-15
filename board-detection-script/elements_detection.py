import cv2
import numpy as np

from utilities import rescale
from utilities import show
from utilities import save


def DetectBoard(picture, scale):
    rescaledPicture = rescale(picture, scale)
    thresholdPicture = PreparePictureForDetection(rescaledPicture)

    contours, hierarchy = cv2.findContours(thresholdPicture, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    contoursPicture = rescaledPicture.copy()
    cv2.drawContours(contoursPicture, contours, -1, (0, 255, 0), thickness=2)
    save(contoursPicture, "picture_with_contours", './resources')

    ScaleContours(contours, 1.0 / scale)
    mask = ChooseMask(picture, contours)
    depth = 1 if len(picture.shape) <= 2 else picture.shape[2]
    maskedPicture = picture & np.stack(([mask] * depth), 2)
    save(maskedPicture, "maskedPicture", './resources')


def PreparePictureForDetection(picture):
    b, g, r = cv2.split(picture)
    equalizedHistogramBlue = cv2.equalizeHist(b)
    save(equalizedHistogramBlue, "equalizedHistogramBlue", './resources')
    blurredPicture = cv2.medianBlur(equalizedHistogramBlue, 5)
    save(blurredPicture, "blurredPicture", './resources')
    ret, thresholdPicture = cv2.threshold(blurredPicture, 170, 255, cv2.THRESH_BINARY)
    save(thresholdPicture, "thresholdPicture", './resources')
    return thresholdPicture


def ScaleContours(contours, scale):
    for contour in contours:
        contour[:, :, 0] = contour[:, :, 0] * scale
        contour[:, :, 1] = contour[:, :, 1] * scale


def ChooseMask(picture, contours):
    mask = np.zeros((picture.shape[0], picture.shape[1]), dtype='uint8')
    cv2.drawContours(mask, contours, -1, (255, 2555, 255), thickness=-1)

    save(mask, "mask", './resources')
    return mask


def PreparePawnForDetection(picture):
    # change colors of the picture to HSV
    hsv = cv2.cvtColor(picture, cv2.COLOR_BGR2HSV)
    save(hsv, "hsv", './resources')

    # describe bounds for desired color
    lower_orange = np.array([0, 100, 100])
    upper_orange = np.array([10,  255, 255])
    lower_blue = np.array([100, 105, 115])
    upper_blue = np.array([110,  235, 180])
    lower_white = np.array([110, 0, 110])
    upper_white = np.array([255,  30, 255])
    lower_red = np.array([170, 150, 50])
    upper_red = np.array([255,  255, 255])

    # create mask with described bounds
    mask = cv2.inRange(hsv, lower_orange, upper_orange)
    save(mask, "mask_for_hsv", './resources')

    # put  mask on the picture
    maskedHsv = cv2.bitwise_and(picture, picture, mask=mask)
    save(maskedHsv, "masked_hsv", './resources')


    return maskedHsv


def FASTCornerDetection(picture):
    # grayImage = cv2.cvtColor(picture, cv2.COLOR_BGR2GRAY)
    # save(grayImage, "keypoints_gray", './resources')
    orb = cv2.ORB_create(nfeatures=1000)

    kp, des = orb.detectAndCompute(picture, None)
    kp_img = cv2.drawKeypoints(picture, kp, None, color=(0, 255, 0), flags=0)

    save(kp_img, "keypoints_orb", './resources')


board = cv2.imread('./resources/board/board_top_3.jpg')
# save(board, "initial_board", './resources')
# DetectBoard(board, 0.2)
pawn = cv2.imread('./resources/board/board_top_3.jpg')
rescaledPicture = rescale(pawn, 0.2)
preparedPicture = PreparePawnForDetection(rescaledPicture)
