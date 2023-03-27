from copy import deepcopy
import argparse
import hashlib
import os
import cv2
import json


# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-w", "--width", type=int, required=True,
    help="Numbers of horizontal squares in ChArUco Board")
ap.add_argument("-l", "--length", type=int, required=True,
    help="Numbers of vertical squares in ChArUco Board")
ap.add_argument("-sl", "--square-length", type=float, required=True,
    help="Square side length in pixels.")
ap.add_argument("-ml", "--marker-length", type=float, required=True,
    help="Marker length in pixels")

ap.add_argument("-d", "--dictionary", type=int, required=True,
    help="Dictionary: "
        "DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
        "DICT_4X4_100=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7"
        "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
        "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL=16"
        )

## Turn the command line agruments into a dictionary
args = vars(ap.parse_args())
image_json = deepcopy(args)

## Generate the ChAruco board
gridboard = cv2.aruco.CharucoBoard_create(
    squaresX=args.get('width'),
    squaresY=args.get('length'),
    squareLength=args.get('square_length'), # 0.04
    markerLength=args.get('marker_length'), # 0.02
    dictionary=cv2.aruco.Dictionary_get(args.get('dictionary'))
)
img = gridboard.draw(outSize=(988, 1400))

filename = 'images.json'

if not os.path.exists(os.path.join(os.getcwd(), 'images')):
    os.mkdir('images')

## Write the png and the JSON file
cv2.imwrite('images/temp.png', img)
image_json['hash'] = hashlib.md5(open('images/temp.png', 'rb').read()).hexdigest()
os.rename('images/temp.png', 'images/{}.png'.format(image_json['hash']))

## Write the meta data about the image to filename
if not os.path.exists(os.path.join(os.getcwd(), filename)):
    f = open(filename, 'w+')
    temp = [image_json]
    f.write(json.dumps(temp))
    f.close()
else:
    f = open(filename, 'r')
    data = json.loads(f.read())
    f.close()

    duplicate = False
    for img in data:
        if img['hash'] == image_json['hash']:
            img = image_json
            duplicate = True

    if not duplicate:
        data.append(image_json)

    f = open(filename, 'w')
    f.write(json.dumps(data))
    f.close()