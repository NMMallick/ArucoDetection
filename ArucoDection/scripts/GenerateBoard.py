import argparse
import cv2


# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-w", "--width", required=True,
    help="Numbers of horizontal squares in ChArUco Board")
ap.add_argument("-l", "--length", type=int, require=True,
    help="Numbers of vertical squares in ChArUco Board")

ap.add_argument("-d", "--dictionary", type=int, required=True,
    help="Dictionary: "
        "DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
        "DICT_4X4_100=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7"
        "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
        "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL=16"
        )

args = vars(ap.parse_args())



gridboard = cv2.aruco.CharucoBoard_create(
    squaresX=5,
    squaresY=7,
    squareLength=0.04,
    markerLength=0.02,
    dictionary=cv2.aruco.Dictionary_get(cv2.aruco.DICT_5X5_100)
)

img = gridboard.draw(outSize=(988, 1400))
cv2.imshow('Gridboard', img)
cv2.waitKey(0)
cv2.destroyAllWindows()