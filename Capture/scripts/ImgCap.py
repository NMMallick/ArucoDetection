import cv2 as cv
import os
from getkey import getkey

##
dirPrefix = 'images'

if not os.path.exists(os.path.join(os.getcwd(), 'images')):
    os.mkdir('images')

cap = cv.VideoCapture(0)
cv.namedWindow('test')
cv.destroyAllWindows()
seq = 0 # Sequence number for image captured

while True:
    ret, frame = cap.read()
    if not ret:
        print("failed to grab frame")
        break

    # cv.imshow("test", frame)
    k = getkey()

    if ord(k) == 27:
        print("Escape hit, closing...")
        break
    elif ord(k) == 32:
        cv.imwrite(f'{dirPrefix}/image_{seq}.png', frame)
        print('Image wrote!')
        seq+=1

cap.release()